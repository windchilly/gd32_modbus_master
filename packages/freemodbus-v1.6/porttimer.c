/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
//#include "tim.h"
#include <stdio.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "gd32f30x_timer.h"
/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	(void)usTim1Timerout50us;
//	__HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);              // 先清除一下定时器的中断标记,防止使能中断后直接触发中断
	timer_parameter_struct timer_initpara;

	rcu_periph_clock_enable(RCU_TIMER1);
	timer_deinit(TIMER1);
	timer_initpara.prescaler = 2399;
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = 49999;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_init(TIMER1, &timer_initpara);
	timer_enable(TIMER1);

	nvic_irq_enable(TIMER1_IRQn, 0, 1);
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);

	return TRUE;
}

inline void vMBPortTimersEnable(  )
{
	timer_disable(TIMER1);
	timer_enable(TIMER1);
}

inline void vMBPortTimersDisable(  )
{
	timer_disable(TIMER1);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

void TIMER1_IRQHandler(void)
{
	if(timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) != RESET)
	{
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
		prvvTIMERExpiredISR();
	}
}

void vMBPortTimersDelay(USHORT usTimeOutMS)
{
	(void)usTimeOutMS;
}
