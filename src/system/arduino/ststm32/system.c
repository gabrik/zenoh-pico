//
// Copyright (c) 2022 ZettaScale Technology
//
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
// which is available at https://www.apache.org/licenses/LICENSE-2.0.
//
// SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
//
// Contributors:
//   ZettaScale Zenoh Team, <zenoh@zettascale.tech>
//

#include "zenoh-pico/system/platform.h"
#include <hw/driver/delay.h>

// This wrapper is only used for ESP32.
// In FreeRTOS, tasks created using xTaskCreate must end with vTaskDelete.
// A task function should __not__ simply return.
typedef struct
{
    void *(*fun)(void *);
    void *arg;
} _zn_task_arg;

void z_task_wrapper(void *arg)
{
    _zn_task_arg *zn_arg = (_zn_task_arg*)arg;
    zn_arg->fun(zn_arg->arg);
    vTaskDelete(NULL);
    free(zn_arg);
}


/*------------------ Task ------------------*/
int z_task_init(z_task_t *task, z_task_attr_t *attr, void *(*fun)(void *), void *arg)
{
    // _zn_task_arg *zn_arg = (_zn_task_arg *)malloc(sizeof(_zn_task_arg));
    // zn_arg->fun = fun;
    // zn_arg->arg = arg;
    // if (xTaskCreate(z_task_wrapper, "", 1280, zn_arg, 0, task) != pdPASS)
    //     return -1;
    return 0;
}

int z_task_join(z_task_t *task)
{
    return 0;
}

int z_task_cancel(z_task_t *task)
{
    return 0;
}

void z_task_free(z_task_t **task)
{
    z_task_t *ptr = *task;
    free(ptr);
    *task = NULL;
}

/*------------------ Mutex ------------------*/
int z_mutex_init(z_mutex_t *m)
{
    // SemaphoreHandle_t handle = NULL;
    // handle = xSemaphoreCreateMutex();
    // if (handle == NULL)
    //     return -1;
    return 0;
}

int z_mutex_free(z_mutex_t *m)
{
    // vSemaphoreDelete(*m);
    return 0;
}

int z_mutex_lock(z_mutex_t *m)
{
    // while ( xSemaphoreTake( *m, ( TickType_t ) configTICK_RATE_HZ ) != pdTRUE );
    return 0;
}

int z_mutex_trylock(z_mutex_t *m)
{
    // if( xSemaphoreTake( *m, ( TickType_t ) configTICK_RATE_HZ ) != pdTRUE )
    // {
    //     return 0;
    // }
    // return -1;
    return 0;
}

int z_mutex_unlock(z_mutex_t *m)
{
    // if( xSemaphoreGive( *m ) != pdTRUE )
    // {
    //     return -1;
    // }
    //return -1;
    return 0;

}

/*------------------ Condvar ------------------*/
int z_condvar_init(z_condvar_t *cv)
{
    return 0;
}

int z_condvar_free(z_condvar_t *cv)
{
    return 0;
}

int z_condvar_signal(z_condvar_t *cv)
{
    return 0;
}

int z_condvar_wait(z_condvar_t *cv, z_mutex_t *m)
{
    return 0;
}

/*------------------ Sleep ------------------*/
int z_sleep_us(unsigned int time)
{
    return -1;
}

int z_sleep_ms(unsigned int time)
{
    return osDelay(time);
}

int z_sleep_s(unsigned int time)
{
    return osDelay(1000 * time);
}

/*------------------ Instant ------------------*/
void _zn_clock_gettime(clockid_t clk_id, z_clock_t *ts)
{
    uint64_t m = millis();
    ts->tv_sec = m / 1000000;
    ts->tv_nsec = (m % 1000000) * 1000;
}

z_clock_t z_clock_now()
{
    z_clock_t now;
    _zn_clock_gettime(NULL, &now);
    return now;
}

clock_t z_clock_elapsed_us(z_clock_t *instant)
{
    z_clock_t now;
    _zn_clock_gettime(NULL, &now);

    clock_t elapsed = (1000000 * (now.tv_sec - instant->tv_sec) + (now.tv_nsec - instant->tv_nsec) / 1000);
    return elapsed;
}

clock_t z_clock_elapsed_ms(z_clock_t *instant)
{
    z_clock_t now;
    _zn_clock_gettime(NULL, &now);

    clock_t elapsed = (1000 * (now.tv_sec - instant->tv_sec) + (now.tv_nsec - instant->tv_nsec) / 1000000);
    return elapsed;
}

clock_t z_clock_elapsed_s(z_clock_t *instant)
{
    z_clock_t now;
    _zn_clock_gettime(NULL, &now);

    clock_t elapsed = now.tv_sec - instant->tv_sec;
    return elapsed;
}

/*------------------ Time ------------------*/
struct timeval z_time_now()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return now;
}

time_t z_time_elapsed_us(struct timeval *time)
{
    struct timeval now;
    gettimeofday(&now, NULL);

    time_t elapsed = (1000000 * (now.tv_sec - time->tv_sec) + (now.tv_usec - time->tv_usec));
    return elapsed;
}

time_t z_time_elapsed_ms(struct timeval *time)
{
    struct timeval now;
    gettimeofday(&now, NULL);

    time_t elapsed = (1000 * (now.tv_sec - time->tv_sec) + (now.tv_usec - time->tv_usec) / 1000);
    return elapsed;
}

time_t z_time_elapsed_s(struct timeval *time)
{
    struct timeval now;
    gettimeofday(&now, NULL);

    time_t elapsed = now.tv_sec - time->tv_sec;
    return elapsed;
}
