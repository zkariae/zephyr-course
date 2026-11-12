# Module 5 — Reliability under pressure

## Assignment

Producer → `k_msgq` (8 slots) → consumer pipeline, supervised by:
- a task watchdog channel (1000 ms) fed by the consumer, with a callback (no reset);
- a health-check thread warning at 75 % queue fill.

The consumer stalls once (`k_sleep` 3000 ms) after 10 messages.

## Build and flash

```
west build -b stm32f7508_dk -d build app -- -DMODULE=module5
west flash --build-dir build --runner openocd
```

## Console output

```
[CONSUMER] stuck for 3000 ms  tick=912
[HEALTH] WARNING queue 6/8 (>= 75%)  tick=1610
[PRODUCER] queue full, drop seq=18  tick=1814
[WDT] consumer channel 0 expired, queue 8/8  tick=1910
...
[PRODUCER] queue full, drop seq=38  tick=3894
[CONSUMER] resumed  tick=3916
[CONSUMER] seq=10 latency=2908 ms
...
[HEALTH] back to normal, queue 0/8  tick=4017
```

## Answers

- **Queue fills:** at 100 ms per sample, 8 slots are full ~800 ms after the stall; 21 samples are dropped.
- **Watchdog fires:** 1000 ms after the last `task_wdt_feed()`. The callback runs in timer ISR context, so it only logs.
- **Health check:** warns before overflow (75 %) and gives ~200 ms of margin before the first drop.

## Git tag

`l5-task1`
