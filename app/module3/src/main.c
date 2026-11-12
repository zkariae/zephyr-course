/*
 * Lecture 3 - Homework: event-driven workqueue instead of a polling loop,
 * with bursts debounced by k_work_reschedule (see README.md for the tasks).
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(homework, LOG_LEVEL_DBG);

#define STACK_SIZE    1024
#define SENSOR_MS     100    /* a burst starts every 100ms */
#define BURST_COUNT   3      /* total bursts to produce */
#define BURST_SIZE    5      /* events per burst */
#define BURST_GAP_MS  4      /* 5 events within 16ms */
#define DEBOUNCE_MS   30     /* handler runs 30ms after the last event */

/* Statistics */
static int total_events;
static int total_processed;

/* ------------------------------------------------------------------ */
/*  sensor_handler - runs in the system workqueue, once per burst     */
/* ------------------------------------------------------------------ */

static void sensor_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    total_processed++;
    LOG_INF("[HANDLER] processed burst %d  tick=%u",
            total_processed, k_uptime_get_32());
}

K_WORK_DELAYABLE_DEFINE(debounce_work, sensor_handler);

/* ------------------------------------------------------------------ */
/*  sensor_sim - fires BURST_COUNT bursts of BURST_SIZE events        */
/* ------------------------------------------------------------------ */

static void sensor_sim_fn(void *p1, void *p2, void *p3)
{
    for (int b = 0; b < BURST_COUNT; b++) {
        k_msleep(SENSOR_MS);

        for (int e = 0; e < BURST_SIZE; e++) {
            if (e > 0) {
                k_msleep(BURST_GAP_MS);
            }

            total_events++;

            /* Each event pushes the deadline back: only the last one counts */
            int ret = k_work_reschedule(&debounce_work, K_MSEC(DEBOUNCE_MS));

            if (ret < 0) {
                LOG_ERR("reschedule failed: %d", ret);
            }
            LOG_INF("[SENSOR] burst %d event %d  reschedule tick=%u",
                    b, e, k_uptime_get_32());
        }
    }

    LOG_INF("[SENSOR] all events produced");
}

K_THREAD_DEFINE(sensor_thread, STACK_SIZE, sensor_sim_fn, NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
    LOG_INF("=== L3 Homework: Polling to Workqueue (debounce) ===");
    LOG_INF("%d events per burst, %dms apart, debounce %dms",
            BURST_SIZE, BURST_GAP_MS, DEBOUNCE_MS);

    /* Wait long enough for all bursts to complete */
    k_msleep((BURST_COUNT + 2) * SENSOR_MS + 500);

    LOG_INF("[SUMMARY] events=%d  handler_runs=%d", total_events, total_processed);

    return 0;
}
