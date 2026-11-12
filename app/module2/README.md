# Module 2 — Synchronization and Shared-State Safety

## Assignment

Two equal-priority threads increment a shared counter (200 000 each).
Observe the race, then fix it with a mutex.

## Build and flash

```
west build -b stm32f7508_dk -d build app -- -DMODULE=module2
west flash --build-dir build --runner openocd
```

## Console output

| Version       | Final value | Time    |
|---------------|-------------|---------|
| Without mutex | 278945      | 22 ms   |
| With mutex    | 400000      | 8629 ms |

## Answers

- **Race:** `counter++` (read, add, write) is preempted mid-operation, so updates are lost.
- **Fix:** the mutex serializes `counter++`, and the result is always 400 000. The cost is a context switch per increment.

## Git tag

`l2-task1`
