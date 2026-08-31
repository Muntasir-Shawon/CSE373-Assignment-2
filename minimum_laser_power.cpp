#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <limits>

using namespace std;

const long long MOD = 1000000005LL;
const long long INF = 4000000000000000000LL;

// Relax all normal East/South moves.
// dp already contains the best cost before this function.
void relaxNormalMoves(vector<long long>& dp,
                     const vector<vector<long long>>& E,
                     int M, int N) {
    for (int r = 0; r < M; r++) {
        for (int c = 0; c < N; c++) {
            int id = r * N + c;

            // Move from the cell above.
            if (r > 0) {
                int up = (r - 1) * N + c;

                if (dp[up] != INF) {
                    long long cost = max(0LL, E[r][c] - E[r - 1][c]);
                    dp[id] = min(dp[id], dp[up] + cost);
                }
            }

            // Move from the cell on the left.
            if (c > 0) {
                int left = r * N + (c - 1);

                if (dp[left] != INF) {
                    long long cost = max(0LL, E[r][c] - E[r][c - 1]);
                    dp[id] = min(dp[id], dp[left] + cost);
                }
            }
        }
    }
}

// Returns the minimum laser power modulo (1,000,000,005).
long long minimumLaserPower(const vector<vector<long long>>& E, int K) {
    int M = E.size();
    int N = E[0].size();
    int total = M * N;

    // A useful solution never needs more than total-1 teleports.
    // Extra teleports can be removed from a repeated/redundant path.
    K = min(K, total - 1);

    // Sort cells by energy from high to low.
    // This lets us find, for every target, the cheapest reachable
    // source whose energy is >= target energy.
    vector<pair<long long, int>> cells;

    for (int r = 0; r < M; r++) {
        for (int c = 0; c < N; c++) {
            cells.push_back({E[r][c], r * N + c});
        }
    }

    sort(cells.begin(), cells.end(),
         [](const pair<long long, int>& a,
            const pair<long long, int>& b) {
             return a.first > b.first;
         });

    // prev[id] = minimum cost to reach id using at most
    // the current number of teleports.
    vector<long long> prev(total, INF);

    // Start at (0,0) with cost 0.
    prev[0] = 0;

    // First calculate paths using 0 teleports.
    relaxNormalMoves(prev, E, M, N);

    // Try using 1, 2, ... K teleports.
    for (int used = 1; used <= K; used++) {
        // Start with the previous answer.
        // This means using a teleport is optional.
        vector<long long> cur = prev;

        long long bestSource = INF;

        int i = 0;

        while (i < total) {
            int j = i;

            // Find all cells having the same energy.
            while (j < total && cells[j].first == cells[i].first) {
                j++;
            }

            // First include ALL sources with this energy.
            // This is important because teleport is allowed when
            // E[target] <= E[source], including equal energies.
            for (int p = i; p < j; p++) {
                int source = cells[p].second;
                bestSource = min(bestSource, prev[source]);
            }

            // Now every cell in this energy group can be a teleport target.
            for (int p = i; p < j; p++) {
                int target = cells[p].second;

                if (bestSource != INF) {
                    cur[target] = min(cur[target], bestSource);
                }
            }

            i = j;
        }

        // After teleporting, continue with normal East/South moves.
        relaxNormalMoves(cur, E, M, N);

        prev = cur;
    }

    // The assignment asks for the answer modulo 1,000,000,005.
    return prev[total - 1] % MOD;
}

int main() {
    int M, N, K;

    cout << "Enter M N K: ";
    cin >> M >> N >> K;

    vector<vector<long long>> E(M, vector<long long>(N));

    cout << "Enter the energy matrix:\n";
    for (int r = 0; r < M; r++) {
        for (int c = 0; c < N; c++) {
            cin >> E[r][c];
        }
    }

    cout << "Minimum laser power = "
         << minimumLaserPower(E, K) << endl;

    return 0;
}

/*
Sample Test 1 from the assignment:

M = 3, N = 3, K = 1

1 3 3
2 5 4
4 3 5

Output:
4

Sample Test 2 from the assignment:

M = 3, N = 2, K = 2

1 2
2 3
3 4

Output:
3

My own test case:

M = 3, N = 3, K = 1

1  7  5
14 16  3
7  3  4

Output:
6

Reason:
Without teleport, one optimal path is:
(0,0) -> (0,1) -> (0,2) -> (1,2) -> (2,2)
Cost = 6 + 0 + 0 + 1 = 7

With one teleport:
(0,0) -> (0,1) costs 7 - 1 = 6
Then teleport from energy 7 to the target energy 4.
Because 4 <= 7, teleport cost = 0.

Total = 6.
*/
