# Six-minute demonstration script

## Optional browser-first opening

Open `web/index.html` in a browser. Enter or edit processes, run SRTF, and show the Gantt chart plus CT/TAT/WT/RT and aggregate metrics. Use Compare All to compare the six policies side by side. This mirrors the simple solve-and-inspect workflow of the reference while the Bankers tab demonstrates the additional resource-allocation topic.

## Before the presentation

- Build the project.
- Run `KernelSimTests` and confirm all tests pass.
- Launch `KernelSim` from the project root.
- Keep `data/sample_workload.txt` and `data/sample_resources.txt` unchanged.
- Know the distinction between SJF and SRTF, and between a safe and unsafe state.

## 0:00–1:00 — Architecture

Explain that `SchedulingPolicy` is abstract. The five concrete policies implement process selection, while `SchedulingEngine` handles arrivals, time progression, preemption, completion, and metrics. This separation is the main OOP design decision.

## 1:00–3:00 — CPU scheduling

Choose menu option 3 to compare algorithms. Explain that the same workload is used for every policy and Round Robin uses quantum 2. Then run SRTF and show the Gantt chart. Point out that a process can be preempted when a shorter remaining job becomes ready.

## 3:00–4:00 — Metrics

Explain:
- TAT = CT - AT
- WT = TAT - BT
- RT = first start - AT

CPU utilization includes all elapsed time, including idle periods. Throughput is completed process count divided by elapsed time.

## 4:00–5:00 — Banker's algorithm

Choose menu option 4, then safety check. The sample scenario should produce a safe sequence beginning with P1. Explain that the sequence is one valid order in which every process can finish and release its allocated resources.

## 5:00–6:00 — Unsafe request and tests

Request A=0, B=0, C=2 for P0. The request does not exceed the current Available vector or P0's remaining declared need, but it leaves the state unsafe. KernelSim must reject it without changing the live allocation. Explain that the test suite verifies this rollback.

Finish by stating the main limitations: one CPU, one CPU burst per process, no context-switch cost, and no actual kernel execution.
