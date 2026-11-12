# Module 8 — Unit testing with ztest and FFF

## Assignment

Starter: `l8-demo-homework`. Unit tests for `temp_alarm` (`temp_alarm/`, unchanged) on `native_sim`.
The sensor calls are inline syscalls, so FFF fakes sit behind a fake `struct device` API table (`tests/temp_alarm/src/sensor_fake.c`).
Spec: `tests/temp_alarm/TEST_SPEC.md`.

## Run

```
west twister -T app/module8/tests -p native_sim -O app/module8/twister-out
west twister -T app/module8/tests -p native_sim -O app/module8/twister-out --coverage --coverage-tool gcovr --gcov-tool gcov --coverage-basedir app/module8/temp_alarm
```

## Results

| Tag | Work | Tests |
|---|---|---|
| `l8-task0` | `sample_fetch` fake | 1 pass, 12 skipped |
| `l8-task1` | 5 happy-path tests | 6 pass, 7 skipped |
| `l8-task2` | 7 error-path tests | 13/13 pass (× 2 scenarios: shuffled, ordered) |

Note: Zephyr > 4.4.0 asserts that `dev->api` is in the sensor API section, so the fake table uses `DEVICE_API(sensor, ...)`.

## Coverage (`temp_alarm.c`)

| Tag | Lines | Functions | Branches |
|---|---|---|---|
| `l8-task1` | 43/54 (79.6 %) | 5/5 | 10/18 (55.6 %) |
| `l8-task2` | 54/54 (100 %) | 5/5 | 18/18 (100 %) |

Report: `twister-out/coverage/index.html`.

## Answers

- **Reached only by Task 2:** the error returns. `init` returns `-ENODEV`/`-EINVAL`, `set_threshold` returns `-EINVAL`, the fetch and `channel_get` failures are handled, and `get_status(NULL)` returns `-EINVAL` (lines 32-33, 37, 54, 71-73, 78-80, 110).
- **Function coverage is 100 % both times:** every function is called, but only line and branch coverage show that their error branches were never taken.

## Git tags

`l8-task0`, `l8-task1`, `l8-task2`, `l8-task3`
