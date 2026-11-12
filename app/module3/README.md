# Module 3 — Workqueues

## Assignment

Convert a 10 ms polling loop to an event-driven `k_work` handler.
Bonus: debounce bursts of 5 events with `k_work_reschedule`.

## Build and flash

```
west build -b stm32f7508_dk -d build app -- -DMODULE=module3
west flash --build-dir build --runner openocd
```

## Console output

| Version                 | Events | Consumer runs | Wasted | Latency |
|-------------------------|--------|---------------|--------|---------|
| Polling (10 ms)         | 10     | 100 wake-ups  | 90 %   | 1–9 ms  |
| `k_work_submit`         | 10     | 10            | 0      | 0 ms    |
| `k_work_reschedule` 30 ms | 15 (3 bursts) | 3     | 0      | 30 ms after last event |

```
[SENSOR] burst 0 event 0  reschedule tick=105
...
[SENSOR] burst 0 event 4  reschedule tick=121
[HANDLER] processed burst 1  tick=151
```

## Answers

- **Polling:** wakes up 10 times per event; 90 % of wake-ups do nothing.
- **Workqueue:** the handler runs only on real events, in the same tick.
- **Debounce:** each event pushes the deadline back 30 ms; only one run per burst.

## Git tag

`l3-task1`
