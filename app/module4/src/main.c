/*
 * Lecture 4 - zbus: one sensor channel, a fast listener (display) and a
 * slower subscriber (logger). See README.md for the tasks.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/zbus/zbus.h>

#define STACK_SIZE      1024
#define SENSOR_PRIO     5
#define LOGGER_PRIO     7
#define SAMPLE_MS       100    /* publish period */
#define SAMPLE_COUNT    10     /* samples to publish */
#define LOGGER_WORK_MS  30     /* simulated slow logging (< SAMPLE_MS) */

struct sensor_msg {
    uint32_t seq;
    int32_t temp_mc;           /* milli-degrees Celsius */
};

ZBUS_SUBSCRIBER_DEFINE(logger_sub, 4);

/* Listener: called synchronously in the publisher's context */
static void display_cb(const struct zbus_channel *chan)
{
    const struct sensor_msg *msg = zbus_chan_const_msg(chan);

    printk("[DISPLAY] seq=%u temp=%d mC  tick=%u\n",
           msg->seq, msg->temp_mc, k_uptime_get_32());
}

ZBUS_LISTENER_DEFINE(display_lis, display_cb);

ZBUS_CHAN_DEFINE(sensor_chan, struct sensor_msg, NULL, NULL,
                 ZBUS_OBSERVERS(display_lis, logger_sub),
                 ZBUS_MSG_INIT(0));

/* Publisher: one simulated sample every SAMPLE_MS */
static void sensor_fn(void *p1, void *p2, void *p3)
{
    for (uint32_t i = 1; i <= SAMPLE_COUNT; i++) {
        k_msleep(SAMPLE_MS);

        /* Triangle wave around 25 C */
        struct sensor_msg msg = {
            .seq = i,
            .temp_mc = 25000 + (int32_t)((i % 5) * 200) - 400,
        };

        printk("[SENSOR]  seq=%u publish  tick=%u\n", msg.seq, k_uptime_get_32());

        int ret = zbus_chan_pub(&sensor_chan, &msg, K_MSEC(10));

        if (ret < 0) {
            printk("[SENSOR]  publish failed: %d\n", ret);
        }
    }
}

/* Subscriber: own thread, woken by zbus, reads the channel itself */
static void logger_fn(void *p1, void *p2, void *p3)
{
    const struct zbus_channel *chan;
    struct sensor_msg msg;

    while (zbus_sub_wait(&logger_sub, &chan, K_FOREVER) == 0) {
        if (chan != &sensor_chan) {
            continue;
        }

        zbus_chan_read(&sensor_chan, &msg, K_MSEC(10));

        /* Simulated slow work (e.g. writing to storage) */
        k_msleep(LOGGER_WORK_MS);

        printk("[LOGGER]  seq=%u temp=%d mC  tick=%u\n",
               msg.seq, msg.temp_mc, k_uptime_get_32());
    }
}

K_THREAD_DEFINE(sensor_thread, STACK_SIZE, sensor_fn, NULL, NULL, NULL,
                SENSOR_PRIO, 0, 0);
K_THREAD_DEFINE(logger_thread, STACK_SIZE, logger_fn, NULL, NULL, NULL,
                LOGGER_PRIO, 0, 0);

int main(void)
{
    printk("=== Module 4: zbus sensor channel ===\n");
    return 0;
}
