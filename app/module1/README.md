# Module 1 — Kernel and Scheduling

## Assignment

Three preemptive threads + one cooperative thread:

| Thread      | Priority | Output          | Sleep  |
|-------------|----------|-----------------|--------|
| `t_low_fn`  | 7        | `T_LOW running` | 300 ms |
| `t_med_fn`  | 5        | `T_MED running` | 200 ms |
| `t_high_fn` | 3        | `T_HIGH running`| 100 ms |
| `t_coop_fn` | -1       | 5 busy iterations, then `k_yield()` | — |

## Build and flash

```
west build -b stm32f7508_dk -d build app -- -DMODULE=module1
west flash --build-dir build --runner openocd
```

## Console output

```
[16:46:46.331] T_HIGH running
[16:46:46.434] T_MEDT_HIGH running
[16:46:46.435]  running
[16:46:46.498] T_LOW running
[16:46:46.536] T_HIGH running
[16:46:46.837] T_COOP running 0
...
[16:46:46.845] T_COOP running 4
[16:46:46.846] T_COOP finished, yielding
```

## Answers

- **Most frequent:** `T_HIGH` (shortest sleep, ~10 runs/s).
- **Priority and interleaving:** yes, a higher priority preempts a lower one (`T_MEDT_HIGH` above). `T_COOP` is never preempted.
- **Ready vs Waiting:** Ready means it can run and waits for the CPU. Waiting means it is blocked (sleep, semaphore…).
- **`k_yield` vs `k_sleep`:** `k_yield` stays Ready and only yields to equal or higher priority. `k_sleep` goes to Waiting, so lower-priority threads can run. That is why `T_COOP` calls `k_msleep(500)` after `k_yield()`.

## Git tag

`l1-task1`
