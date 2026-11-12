# Module 7 — Application isolation

## Assignment

Starter: `l7-homework` (`parser.c` and `credentials.c` unchanged). `get=<n>` has an unchecked index, so an attacker can read `device_key`.
Goal: isolate the parser so that the bug can no longer reach the key. The course runs it in QEMU (`mps2/an521`). Here it runs on `stm32f7508_dk` (Cortex-M7 MPU).

## Build and flash

```
west build -p -b stm32f7508_dk -d build app -- -DMODULE=module7
west flash --build-dir build --runner openocd
```

## Fix

- `prj.conf`: `CONFIG_USERSPACE=y`
- `main.c`: parser thread created with `K_USER`

## Console output

Before (key leaked):
```
parser: setting 0 = led=off
parser: setting 25168441 = EC-PRIVATE-KEY:8
parser: setting 25168442 = f3a91c25d7e04b6
```
After (`device_key` at `0x20010840`):
```
parser: setting 0 = led=off
parser: received 'get=25167078'
<err> os: ***** MPU FAULT *****
<err> os:   Data Access Violation
<err> os:   MMFAR Address: 0x20010840
```

## Answers

- **Untrusted data:** the parser handles network commands.
- **Kernel or user thread:** user. It can only access its own stack and flash (`settings` is `const`), so `device_key` in kernel RAM is outside its MPU regions.
- **Isolation feature:** user mode + MPU (memory domains). No extra partition is needed.

## Git tag

`l7-task1`
