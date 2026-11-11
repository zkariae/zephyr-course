#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);

#define STACK_SIZE 1024

#define PRIO_LOW 7
#define PRIO_MED 5 
#define PRIO_HIGH 3
#define PRIO_COOP -1


void thread_t_low_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        LOG_DBG("T_LOW running");
        k_msleep(300);
    }
}

void thread_t_med_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        LOG_DBG("T_MED running");
        k_msleep(200);
    }
}

void thread_t_high_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        LOG_DBG("T_HIGH running");
        k_msleep(100);
    }
}

void thread_t_coop_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        for (int i = 0; i < 5; i++) {
            LOG_DBG("T_COOP running %d", i);
            k_busy_wait(500);
        }
        LOG_DBG("T_COOP finished, yielding");
        k_yield();

        /* k_yield() only hands off to threads of the same or higher
         * priority (see k_yield() doc) -- since T_LOW/MED/HIGH are all
         * lower priority than this cooperative thread, k_yield() alone
         * never lets them run. k_msleep() actually blocks this thread,
         * which is what frees the CPU for them.
         */
        k_msleep(500);
    }
}

K_THREAD_DEFINE(thread_t_low, STACK_SIZE, thread_t_low_fn,
                NULL, NULL, NULL, PRIO_LOW, 0, 0);
K_THREAD_DEFINE(thread_t_med, STACK_SIZE, thread_t_med_fn,
                NULL, NULL, NULL, PRIO_MED, 0, 0);
K_THREAD_DEFINE(thread_t_high, STACK_SIZE, thread_t_high_fn,
                NULL, NULL, NULL, PRIO_HIGH, 0, 0);
K_THREAD_DEFINE(thread_t_coop, STACK_SIZE, thread_t_coop_fn,
                NULL, NULL, NULL, PRIO_COOP, 0, 0);

int main(void)
{
    return 0;
}
