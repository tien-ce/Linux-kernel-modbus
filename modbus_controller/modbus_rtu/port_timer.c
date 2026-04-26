/*
 * port_timer.c
 *
 *  Created on: Apr 9, 2026
 *      Author: vantien
 *
 * License: MIT
 * Copyright (c) 2026 Văn Tiến
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "Include/port.h"
#include "Include/mbport.h"
#include "Include/mbrtu.h"
#include <linux/hrtimer.h>
#include <linux/ktime.h>
/* ----------------- Global-Static Variables ---------------------- */
/* High-resolution timer structure managed by the kernel */
static struct hrtimer my_hrtimer;
/* Stores the programmed INTerval in kernel time format for reuse in timer_enable */
static ktime_t active_INTerval; 

/*------------------------ Call back --------------------------------*/
/**
 * @brief Timer callback handler triggered upon expiration.
 * @param timer: PoINTer to the hrtimer structure that expired.
 * @return HRTIMER_NORESTART to indicate the timer shouldn't auto-repeat.
 */
static enum hrtimer_restart test_hrtimer_handler(struct hrtimer *timer) {
    /* * Modbus RTU Logic: This signifies a T35 (3.5 char) silence has occurred.
     */
	xMBRTUTimerT35Expired();
	pr_info("Timer is expired\n");
    return HRTIMER_NORESTART;
}

/*------------------------ Exported Function  --------------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	//	pr_info("Modbustimer init: %d * 50(us)\n",usTimTimerout50us);
    ULONG timeout_us;
    /* Calculate the total silence INTerval in microseconds */
    timeout_us = (ULONG)usTim1Timerout50us * 50;

    /* Initialize the timer structure with a monotonic clock (ignores wall-clock jumps) */
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

    /* Link the handler function to the timer object */
    my_hrtimer.function = &test_hrtimer_handler;

    /* Convert calculated micro-seconds INTo the specialized ktime_t format */
    active_INTerval = ktime_set(0, timeout_us * 1000);

    printk(KERN_INFO "Modbus Timer: Initialized with %lu us INTerval.\n", timeout_us);
	return true;
}

void vMBPortTimersStart( void )
{
    /* * Starts the timer relative to the current moment.
     * If the timer was already running, it is automatically rescheduled.
     */
    hrtimer_start(&my_hrtimer, active_INTerval, HRTIMER_MODE_REL);

}

void vMBPortTimersCancel( void )
{
    /* hrtimer_cancel is synchronous: it waits for any running handler to finish */
    hrtimer_cancel(&my_hrtimer);
}


