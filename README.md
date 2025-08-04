# Scheduling Policies

A C++ simulation framework for visualizing and analyzing classic CPU scheduling algorithms. This tool provides both **execution trace timelines** and **performance statistics** for a range of CPU scheduling policies.

## Features

- Supports **8 scheduling algorithms**:
  - `1` — FCFS (First-Come First-Served)
  - `2` — RR (Round Robin)
  - `3` — SPN (Shortest Process Next)
  - `4` — SRT (Shortest Remaining Time)
  - `5` — HRRN (Highest Response Ratio Next)
  - `6` — FB-1 (Feedback, 1 unit quantum)
  - `7` — FB-2i (Feedback, exponentially increasing quantum)
  - `8` — Aging (priority-based with dynamic aging)
- Two execution modes:
  - `trace` — Visualize process scheduling over time
  - `stats` — Display detailed turnaround and response metrics
- Friendly command-line interface
- Modular class-based structure for easy extension

---

## Getting Started

### Prerequisites

- C++17 or later compiler (e.g., g++)

### Building the Project

```bash
make
```

### Running the Project

```bash
./lab6
```

### Input Format

```bash
<mode>
<policy or policy list>
<simulation_end> 
<number_of_processes>
<process_1>
<process_2>
...
<process_n>
```

### Output Format

#### Trace Mode
Shows a timeline of each process's execution state:

'*' - Executing

'.' - Ready
```bash
FCFS  0 1 2 3 4 5 6 7 8 9 ...
-----------------------------
A     |*|*|*|*|*| | | | | ...
B     | | |.|.|.|*|*|*|*| ...
C     | | | | | |.|.|.|*|* ...
-----------------------------
```

#### Stats Mode
Displays process performance metrics:
```bash
Process    |  A  |  B  |  C  |
Arrival    |  0  |  2  |  4  |
Service    |  5  |  4  |  3  | Mean|
Finish     |  5  |  9  | 12  |-----|
Turnaround |  5  |  7  |  8  | 6.67|
NormTurn   |1.00 |1.75 |2.67 | 1.81|
```

