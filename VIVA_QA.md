# KernelSim Viva Preparation

## 1. Why did you choose this project?

It combines core operating-system algorithms with object-oriented design. The scheduler policies have genuinely different behaviour, and the Banker's algorithm adds resource-allocation safety analysis.

## 2. Why is `SchedulingPolicy` abstract?

The base class defines the common operations required by the engine without implementing one specific scheduling rule. Concrete policies supply their own selection behaviour.

## 3. Where is runtime polymorphism used?

`SchedulingEngine::run` accepts a reference to `SchedulingPolicy`. Calls to `selectProcess`, `getName`, `isPreemptive`, and `timeQuantum` resolve to the concrete policy implementation.

## 4. What is the difference between SJF and SRTF?

SJF selects the ready process with the shortest original CPU burst and does not preempt the running process. SRTF selects the process with the shortest remaining time and can preempt the current process when a better candidate arrives.

## 5. Why does Round Robin need a time quantum?

The quantum limits how long a process can run before returning to the back of the ready queue. A very large quantum makes the behaviour closer to FCFS; a very small quantum increases switching frequency in a real system. This simulator does not model context-switch overhead.

## 6. What is the difference between waiting, turnaround, and response time?

Turnaround time is completion time minus arrival time. Waiting time is turnaround time minus CPU burst time for this single-burst model. Response time is the first CPU start time minus arrival time.

## 7. Why can CPU utilization be below 100%?

The CPU may be idle before the first process arrives or between periods when no process is ready. Idle time counts toward elapsed time but not CPU busy time.

## 8. Does the priority scheduler prevent starvation?

No. This implementation uses non-preemptive priority scheduling and does not implement aging. A process with a low priority may wait for a long time if higher-priority work continues to arrive.

## 9. What is a safe state?

A state is safe if there exists at least one order in which every process can obtain its remaining required resources, finish, and release its allocated resources.

## 10. Is every unsafe state deadlocked?

No. An unsafe state means the system cannot guarantee that all processes can finish for every possible future request sequence. It does not prove that a deadlock has already occurred.

## 11. How does the Banker's algorithm handle a request?

It checks the process's remaining need and current availability, then tests a candidate allocation. The candidate becomes the live allocation only if the safety test succeeds.

## 12. How does the project roll back an unsafe request?

The resource manager evaluates a copy of the allocation matrix. It changes the actual matrix only after the candidate state is found safe, so rejection leaves the original state unchanged.

## 13. What is the difference between inheritance and composition here?

Inheritance models the relationship between a scheduling policy and its concrete algorithms. Composition is used where an object contains constituent data, such as a simulation result containing Gantt segments and process results.

## 14. How would you add a new scheduling algorithm?

Create a class derived from `SchedulingPolicy` and implement its interface. The engine can then use it through the base-class reference without duplicating the time-progression logic.

## 15. Is KernelSim a real operating system?

No. It is a user-space educational simulator of selected OS algorithms. It does not schedule actual operating-system processes or manage hardware resources.

## 16. What are the major limitations?

It models one CPU, one CPU burst per process, integer time units, and zero context-switch overhead. The resource module is independent of CPU scheduling. It does not model I/O blocking, multicore scheduling, real semaphores, or memory management.

## 17. How did you verify correctness?

The test executable checks expected completion times and metrics for each scheduling policy, idle-time handling, safe and unsafe resource requests, rollback, input validation, and timeline/metric invariants. The CMake build and test suite are run before demonstration.
