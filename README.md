# Minimum Laser Power Optimization (Dynamic Programming with Teleportation)

An algorithmic solution to find the minimum laser power needed to navigate an energy lattice from the top-left corner $(0, 0)$ to the bottom-right corner $(M - 1, N - 1)$ using normal moves and up to $K$ zero-cost teleportations.

---

## 📂 Repository Structure

- [`minimum_laser_power.cpp`](minimum_laser_power.cpp) — C++ implementation with Dynamic Programming and Prefix-Extremum Optimization.
- [`README.md`](README.md) — Algorithm explanation, complexity analysis, and documentation.
- [`.gitignore`](.gitignore) — Excludes compiled binary artifacts.

---

## 📌 Problem Overview

Given an $M \times N$ grid (lattice) where each cell $(r, c)$ has an associated energy level $E[r][c]$.

A photon starts at $(0, 0)$ and must reach $(M - 1, N - 1)$.

### 🚶 Movement Rules

1. **Normal Moves (Right / Down):**
   - The photon can move to the right cell $(r, c + 1)$ (East) or the downward cell $(r + 1, c)$ (South).
   - **Cost:** If moving from cell $u$ to adjacent cell $v$, the laser power consumed is:
     $$\text{Cost}(u \to v) = \max(0, E[v] - E[u])$$
     *(Moving to a cell with equal or lower energy costs 0 power; moving to a higher energy cell costs the exact energy difference).*

2. **Teleportation Moves:**
   - The photon can teleport from any cell $u$ to **any** other cell $v$ anywhere on the grid if and only if:
     $$E[v] \le E[u] \quad (\text{destination energy} \le \text{source energy})$$
   - **Cost:** Teleportation consumes **$0$ laser power**.
   - **Constraint:** At most **$K$** teleports are permitted during the entire journey.

3. **Goal:**
   - Determine the minimum total laser power needed to reach cell $(M - 1, N - 1)$.
   - Output the result modulo $1,000,000,005$ ($\text{MOD} = 10^9 + 5$).

---

## 🧠 Algorithm & Dynamic Programming Strategy

A naive search or graph construction would be too slow because any cell can potentially teleport to any other cell, leading to $O((M \cdot N)^2)$ edges per teleport layer. This solution uses an optimized **Dynamic Programming with Prefix-Extremum Optimization**.

### 1. State Definition

Let $DP[k][r][c]$ be the minimum laser power needed to reach cell $(r, c)$ using at most $k$ teleports ($0 \le k \le K$).

Since the DP only depends on the results from the previous teleport count $(k-1)$, the algorithm optimizes memory using two 1D arrays of size $\text{total} = M \times N$:
- `prev[id]`: The optimal costs after using up to $k-1$ teleports.
- `cur[id]`: The optimal costs after using up to $k$ teleports.

### 2. Normal Moves Relaxation (`relaxNormalMoves`)

Because normal moves are only directed **Right** (East) and **Down** (South), the dependency graph is a Directed Acyclic Graph (DAG). Processing cells in row-major topological order (from row $0 \to M-1$ and col $0 \to N-1$) guarantees optimal propagation:

$$DP[r][c] = \min \begin{cases} 
DP[r][c] \\
DP[r-1][c] + \max(0, E[r][c] - E[r-1][c]) & \text{if } r > 0 \\
DP[r][c-1] + \max(0, E[r][c] - E[r][c-1]) & \text{if } c > 0
\end{cases}$$

### 3. Efficient Teleportation Optimization ($O(M \cdot N)$ per teleport)

For a teleport step from state $k-1$ to $k$, any reachable source cell $s$ with energy $E[s] \ge E[t]$ can teleport to target $t$ with $0$ additional cost:
$$\text{cur}[t] = \min\Big(\text{prev}[t], \min_{\{s \mid E[s] \ge E[t]\}} \text{prev}[s]\Big)$$

Checking all pairs $(s, t)$ would take $O((M \cdot N)^2)$. We optimize this to $O(M \cdot N)$ by:
1. **Sorting** all grid cells in **descending order of energy**: $E_1 \ge E_2 \ge \dots \ge E_{MN}$.
2. Iterating through grouped cells that share the same energy:
   - Accumulate the running minimum cost `bestSource = min(bestSource, prev[source])` for all cells in the current energy group.
   - Assign `cur[target] = min(cur[target], bestSource)` for all target cells in the current group.
3. Because higher-energy cells were processed before lower-energy cells, `bestSource` always holds the minimum cost among all cells with $E[\text{source}] \ge E[\text{target}]$.

### 4. Alternating Updates

For each teleport count $used = 1, 2, \dots, K$:
1. Initialize `cur = prev`.
2. Apply teleport jumps using the sorted cell prefix minimum.
3. Propagate normal moves via `relaxNormalMoves(cur, E, M, N)`.
4. Update `prev = cur`.

---

## ⏱️ Complexity Analysis

- **Cell Sorting:** $O(MN \log(MN))$ (done once).
- **Base Normal Relaxation (0 teleports):** $O(MN)$.
- **Teleport + Normal Relaxation Loop:** Runs $K$ times. Each iteration takes $O(MN)$ for teleport updates and $O(MN)$ for normal move relaxation $\to O(K \cdot MN)$.
- **Total Time Complexity:** 
  $$\mathcal{O}(MN \log(MN) + K \cdot MN)$$
- **Space Complexity:** 
  $$\mathcal{O}(MN)$$

---

## 🧪 Test Cases & Walkthrough

### Test Case 1
- **Input:**
  ```text
  M = 3, N = 3, K = 1
  1 3 3
  2 5 4
  4 3 5
  ```
- **Output:** `4`

---

### Test Case 2
- **Input:**
  ```text
  M = 3, N = 2, K = 2
  1 2
  2 3
  3 4
  ```
- **Output:** `3`

---

### Test Case 3
- **Input:**
  ```text
  M = 3, N = 3, K = 1
  1  7  5
  14 16  3
  7  3  4
  ```
- **Output:** `6`
- **Explanation:**
  - **Without Teleport:** Optimal path $(0,0) \to (0,1) \to (0,2) \to (1,2) \to (2,2)$ has cost $(7-1) + 0 + 0 + (4-3) = 6 + 0 + 0 + 1 = 7$.
  - **With 1 Teleport:** Move $(0,0) \to (0,1)$ at cost $7 - 1 = 6$. From cell $(0, 1)$ with energy $7$, teleport directly to destination $(2,2)$ with energy $4$ (since $4 \le 7$, cost $= 0$).
  - **Total Minimum Laser Power:** $6$.

---

## 🛠️ How to Compile and Run

### Prerequisites
- C++ compiler supporting C++11 or later (`g++`, `clang++`, or MSVC).

### Compilation
```bash
g++ -O3 -std=c++11 minimum_laser_power.cpp -o minimum_laser_power
```

### Execution
```bash
# On Windows
./minimum_laser_power.exe

# On Linux / macOS
./minimum_laser_power
```

### Input Format
```text
Enter M N K: 3 3 1
Enter the energy matrix:
1 3 3
2 5 4
4 3 5
```
