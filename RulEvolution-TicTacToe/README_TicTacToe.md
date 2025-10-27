# RulEvolution-TicTacToe

This project implements the **RulEvolution** learning paradigm through a practical case study based on the Tic-Tac-Toe game.

## Features
- Rule-based adaptive learning system
- Parallelization with **OpenMP**
- Super-Training mode for concurrent learning
- Fully implemented in C++

## Compilation
- Requires a C++17 compiler with OpenMP support.
- Visual Studio: enable `/openmp` and define `USE_OMP`.
- To build:
  Open the solution and compile in Debug or Release mode.

## Folder structure
```
/src   →  Source code (.cpp, .h)
/docs  →  Paper and appendix (.tex, .pdf)
```

## Example of performance
| Mode | Matches | Time (s) |
|------|----------|----------|
| Standard — Sequential | 100 | 5.9 |
| Standard — Parallel | 100 | 2.2 |
| Super-Training — Sequential | 100 | 3.6 |
| Super-Training — Parallel | 100 | 1.0 |

## Author
**Alia Rosario**

Licensed under the GNU General Public License v3.0 (GPL-3.0).

