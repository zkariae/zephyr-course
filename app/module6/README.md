# Module 6 — Scheduling delay investigation

## Assignment

Starter: `l6-homework`. A timer ISR makes `control` (10 ms deadline) and `maintenance` (45 ms busy load) ready at the same time.
Added: latency/deadline-miss counting, kernel shell, CTF tracing (RAM backend), one fix.
Module-specific Kconfig: `app/module6/prj.conf` (merged by `app/CMakeLists.txt`).

## Build and flash

```
west build -p -b stm32f7508_dk -d build app -- -DMODULE=module6
west flash --build-dir build --runner openocd
```

## Response-time guarantee

`control` starts within **10 ms** of `ready_ms` (set in the timer ISR). Every miss is counted; warnings are rate-limited (1st, then every 10th).

## Results

| | `control` prio | `maintenance` prio | Latency | Misses |
|---|---|---|---|---|
| Before | 7 | 4 | 45 ms | every event |
| After  | 3 | 4 | 0 ms  | 0 |

```
before: [CONTROL] processed seq=144 latency=45 ms
after:  [SUMMARY] processed=80 misses=0 max=0 ms (deadline 10 ms)
```

## Kernel shell

`kernel thread list`: `maintenance` priority 4, `control` priority 7 (before), both `pending` between events.

## CTF trace (Babeltrace)

```
openocd ... -c init -c halt -c "dump_image result/ctf/channel0_0 <&ram_tracing> 65536" -c resume -c shutdown
cp zephyr/subsys/tracing/ctf/tsdl/metadata result/ctf/
babeltrace2 result/ctf
```

Before (seq=4):
```
1.500532  evt_ready seq=4
1.500585  thread_switched_in maintenance
1.500605  thread_busy_wait_enter 45000 us
1.545687  thread_switched_in control
1.545722  ctrl_done seq=4 latency=45
```
After (seq=4):
```
1.500530  evt_ready seq=4
1.500584  thread_switched_in control
1.500618  ctrl_done seq=4 latency=0
1.500689  thread_switched_in maintenance
```

## Answers

- **Cause:** both threads become ready in the same ISR. The scheduler runs the highest priority (`maintenance`, 4) first, and its 45 ms busy wait delays `control` (7).
- **Fix:** priority follows the deadline: `control` 3 > `maintenance` 4. Maintenance still runs right after.

## Git tag

`l6-task1`
