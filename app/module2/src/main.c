#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE      1024
#define PRIO            5
#define INCREMENTS      1000000 /* each thread increments this many times */
#define TIME_SLICE_MS   1       /* short slices -> frequent preemption */

/* Shared state - intentionally unprotected */
static volatile uint32_t counter;

static struct k_sem done_sem;

void worker_fn(void *p1, void *p2, void *p3)
{
    const char *name = k_thread_name_get(k_current_get());

    for (int i = 0; i < INCREMENTS; i++) {
        counter++;
    }

    printk("[%s] finished\n", name);
    k_sem_give(&done_sem);
}

K_THREAD_DEFINE(worker_a, STACK_SIZE, worker_fn, NULL, NULL, NULL,
                PRIO, 0, 0);
K_THREAD_DEFINE(worker_b, STACK_SIZE, worker_fn, NULL, NULL, NULL,
                PRIO, 0, 0);

int main(void)
{
    /* Workers start only once main (prio 0) blocks on done_sem */
    k_sched_time_slice_set(TIME_SLICE_MS, PRIO);
    k_sem_init(&done_sem, 0, 2);
    int64_t time = k_uptime_get();

    printk("=== Module 2: Shared Counter Race Condition ===\n");
    printk("Expected final value: %u\n", INCREMENTS * 2);

    k_sem_take(&done_sem, K_FOREVER);
    k_sem_take(&done_sem, K_FOREVER);

    printk("Actual   final value: %u\n", counter);

    if (counter == INCREMENTS * 2) {
        printk("No lost updates\n");
    } else {
        printk("Race condition: lost %u updates\n",
               (INCREMENTS * 2) - counter);
    }

    printk("Execution time: %lld ms\n", k_uptime_delta(&time));
    return 0;
}
