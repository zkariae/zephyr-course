# Zephyr Intermediate Course — Assignments

Solutions to the Zephyr Intermediate course assignments, one folder per module.
Each module README gives the assignment, the build commands, a console excerpt, the answers and the git tag.

- Board: `stm32f7508_dk` (Cortex-M7), Zephyr 4.4.99, Zephyr SDK 1.0.1
- Starter code: branches `lN-homework` of the course repository

## Modules

| Module | Topic | Target | Tag |
|---|---|---|---|
| [module1](app/module1/README.md) | Threads, priorities, `k_yield` vs `k_sleep` | board | `l1-task1` |
| [module2](app/module2/README.md) | Race condition and mutex | board | `l2-task1` |
| [module3](app/module3/README.md) | Polling vs workqueue, debounce | board | `l3-task1` |
| [module4](app/module4/README.md) | zbus listener and subscriber | board | `l4-task1` |
| [module5](app/module5/README.md) | Message queue, task watchdog, health check | board | `l5-task1` |
| [module6](app/module6/README.md) | Scheduling delay: kernel shell, CTF tracing, fix | board | `l6-task1` |
| [module7](app/module7/README.md) | Userspace isolation, MPU fault | board | `l7-task1` |
| [module8](app/module8/README.md) | Unit tests (ztest, FFF), coverage | `native_sim` | `l8-task0` … `l8-task3` |

## Build and flash (board modules)

```
. /opt/python/venv/bin/activate
west build -p -b stm32f7508_dk -d build app -- -DMODULE=module1
west flash --build-dir build --runner openocd
```

Common files: `app/CMakeLists.txt` and `app/prj.conf`. The build compiles every `app/<MODULE>/src/*.c` and merges `app/<MODULE>/prj.conf` when it exists (modules 6 and 7).
Use `-p` when switching modules (`MODULE` is cached).

## Run the tests (module 8)

```
west twister -T app/module8/tests -p native_sim -O app/module8/twister-out
```
