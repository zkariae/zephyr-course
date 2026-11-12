# Module 4 — zbus

## Assignment

One zbus channel for sensor data, published every 100 ms.
A listener for fast display updates, a subscriber for slower logging.

## Build and flash

```
west build -b stm32f7508_dk -d build app -- -DMODULE=module4
west flash --build-dir build --runner openocd
```

## Console output

```
[SENSOR]  seq=1 publish  tick=108
[DISPLAY] seq=1 temp=24800 mC  tick=111
[LOGGER]  seq=1 temp=24800 mC  tick=145
[SENSOR]  seq=2 publish  tick=215
[DISPLAY] seq=2 temp=25000 mC  tick=217
[LOGGER]  seq=2 temp=25000 mC  tick=251
```

## Answers

- **Listener:** callback runs synchronously in the publisher's thread, right after publish.
- **Subscriber:** notified through a queue, runs later in its own thread (~36 ms here).

## Git tag

`l4-task1`
