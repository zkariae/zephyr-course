/*
 * Module 5 — Reliability under pressure
 *
 * Producer -> k_msgq -> consumer pipeline, supervised by:
 *  - a task watchdog channel fed by the consumer,
 *  - a health-check thread warning at 75 % queue fill.
 * The consumer gets stuck once (long k_sleep) to trigger both.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/task_wdt/task_wdt.h>

#define QUEUE_LEN        8
#define WARN_LEVEL       (QUEUE_LEN * 3 / 4)   /* 75 % = 6 messages */
#define SAMPLES          40
#define PRODUCE_MS       100
#define HEALTH_MS        100
#define WDT_TIMEOUT_MS   1000
#define STUCK_AFTER      10                    /* messages before stalling */
#define STUCK_MS         3000

struct sample {
	uint32_t seq;
	uint32_t tick;
};

K_MSGQ_DEFINE(pipe_q, sizeof(struct sample), QUEUE_LEN, 4);

static int wdt_ch;

/* Runs in the task_wdt timer ISR: print only, never block. */
static void wdt_cb(int channel_id, void *user_data)
{
	ARG_UNUSED(user_data);
	printk("[WDT] consumer channel %d expired, queue %u/%u  tick=%u\n",
	       channel_id, k_msgq_num_used_get(&pipe_q), QUEUE_LEN,
	       k_uptime_get_32());
}

static void producer_fn(void *a, void *b, void *c)
{
	for (uint32_t seq = 0; seq < SAMPLES; seq++) {
		struct sample s = { .seq = seq, .tick = k_uptime_get_32() };

		if (k_msgq_put(&pipe_q, &s, K_NO_WAIT) != 0) {
			printk("[PRODUCER] queue full, drop seq=%u  tick=%u\n",
			       seq, s.tick);
		}
		k_msleep(PRODUCE_MS);
	}
	printk("[PRODUCER] done\n");
}

static void consumer_fn(void *a, void *b, void *c)
{
	struct sample s;
	uint32_t count = 0;

	do {
		k_msgq_get(&pipe_q, &s, K_FOREVER);
		task_wdt_feed(wdt_ch);
		printk("[CONSUMER] seq=%u latency=%u ms\n",
		       s.seq, k_uptime_get_32() - s.tick);

		if (++count == STUCK_AFTER) {
			printk("[CONSUMER] stuck for %d ms  tick=%u\n",
			       STUCK_MS, k_uptime_get_32());
			k_msleep(STUCK_MS);
			printk("[CONSUMER] resumed  tick=%u\n", k_uptime_get_32());
		}
	} while (s.seq < SAMPLES - 1);

	/* Pipeline finished: stop supervision so the idle wait does not fire. */
	task_wdt_delete(wdt_ch);
	printk("[CONSUMER] done, watchdog channel removed\n");
}

static void health_fn(void *a, void *b, void *c)
{
	bool warned = false;

	while (1) {
		uint32_t used = k_msgq_num_used_get(&pipe_q);

		if (!warned && used >= WARN_LEVEL) {
			printk("[HEALTH] WARNING queue %u/%u (>= 75%%)  tick=%u\n",
			       used, QUEUE_LEN, k_uptime_get_32());
			warned = true;
		} else if (warned && used < WARN_LEVEL) {
			printk("[HEALTH] back to normal, queue %u/%u  tick=%u\n",
			       used, QUEUE_LEN, k_uptime_get_32());
			warned = false;
		}
		k_msleep(HEALTH_MS);
	}
}

K_THREAD_DEFINE(producer, 1024, producer_fn, NULL, NULL, NULL, 5, 0, SYS_FOREVER_MS);
K_THREAD_DEFINE(consumer, 1024, consumer_fn, NULL, NULL, NULL, 6, 0, SYS_FOREVER_MS);
K_THREAD_DEFINE(health, 1024, health_fn, NULL, NULL, NULL, 4, 0, SYS_FOREVER_MS);

int main(void)
{
	printk("=== Module 5: Reliability under pressure ===\n");

	task_wdt_init(NULL);   /* software only: callback instead of reset */
	wdt_ch = task_wdt_add(WDT_TIMEOUT_MS, wdt_cb, NULL);
	if (wdt_ch < 0) {
		printk("task_wdt_add failed: %d\n", wdt_ch);
		return 0;
	}

	k_thread_start(health);
	k_thread_start(consumer);
	k_thread_start(producer);
	return 0;
}
