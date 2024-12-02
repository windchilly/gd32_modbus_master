#include "cd_timer.h"

static cd_timer_t *timer_head = CD_NULL;
static volatile cd_uint32_t cd_systick = 0;

cd_timer_t *cd_timer_find(task_id_t task_id, event_id_t event_id)
{
    cd_timer_t *timer_sech;

    timer_sech = timer_head;
    while(timer_sech != CD_NULL)
    {
        if(timer_sech->event_id == event_id && timer_sech->task_id == task_id){break;}
        timer_sech = timer_sech->next;
    }

    return timer_sech;
}

static cd_timer_t *cd_timer_create(task_id_t task_id, event_id_t event_id, cd_uint16_t timeout)
{
    cd_timer_t *timer_new;
    cd_timer_t *timer_sech;

    timer_new = cd_timer_find(task_id, event_id);
    if(timer_new != CD_NULL)
    {
        timer_new->timeout = timeout;
        return timer_new;
    }
    else
    {
        timer_new = (cd_timer_t *)cd_malloc(sizeof(cd_timer_t));

        if(timer_new != CD_NULL)
        {
            timer_new->task_id = task_id;
            timer_new->event_id = event_id;
            timer_new->timeout = timeout;
            timer_new->next = (void *)CD_NULL;
            timer_new->reload = 0;

            if(timer_head == CD_NULL)
			{
				timer_head = timer_new;
			}
            else
            {
                timer_sech = timer_head;
                while(timer_sech->next != CD_NULL){timer_sech = timer_sech->next;}
                timer_sech->next = timer_new;
            }
            return timer_new;
        }
        else
		{
			return CD_NULL;
		}
    }
}

void cd_timer_del(cd_timer_t *timer)
{
    if(timer != CD_NULL)
	{
		timer->event_id = SYS_EVE_NONE;
	}
}

cd_err_t cd_timer_startex(task_id_t task_id, event_id_t event_id, cd_uint16_t timeout)
{
    cd_timer_t *timer_new;

    CD_ENTER_CRITICAL();
    timer_new = cd_timer_create(task_id, event_id, timeout);
    CD_EXIT_CRITICAL();

    return ((timer_new != CD_NULL) ? CD_SUCCESS : CD_ERROR);
}

cd_err_t cd_timer_startre(task_id_t task_id, event_id_t event_id, cd_uint16_t timeout)
{
    cd_timer_t *timer_new;

    CD_ENTER_CRITICAL();
    timer_new = cd_timer_create(task_id, event_id, timeout);
    if(timer_new != CD_NULL)
        timer_new->reload = timeout;
    CD_EXIT_CRITICAL();

    return ((timer_new != CD_NULL) ? CD_SUCCESS : CD_ERROR);
}

cd_err_t cd_timer_stopex(task_id_t task_id, event_id_t event_id)
{
    cd_timer_t *found_timer;

    CD_ENTER_CRITICAL();
    found_timer = cd_timer_find(task_id, event_id);
    if(found_timer != CD_NULL)
        cd_timer_del(found_timer);
    CD_EXIT_CRITICAL();

    return ((found_timer != CD_NULL) ? CD_SUCCESS : CD_ERROR);
}

cd_uint16_t cd_timer_gettimeoutex(task_id_t task_id, event_id_t event_id)
{
    cd_uint16_t tout = 0;
    cd_timer_t *timer;

    CD_ENTER_CRITICAL();
    timer = cd_timer_find(task_id, event_id);
    if(timer != CD_NULL)
	{
		tout = timer->timeout;
	}
    CD_EXIT_CRITICAL();

    return tout;
}

cd_uint8_t cd_timer_num(void)
{
    cd_uint8_t num = 0;
    cd_timer_t *timer_sech;

    CD_ENTER_CRITICAL();
    timer_sech = timer_head;
    while(timer_sech != CD_NULL)
    {
        num++;
        timer_sech = timer_sech->next;
    }
    CD_EXIT_CRITICAL();

    return num;
}

void cd_timer_update(void)
{
    cd_timer_t *timer_sech;
    cd_timer_t *timer_prev;

    CD_ENTER_CRITICAL();
    cd_systick++;
    CD_EXIT_CRITICAL();

    if(timer_head != CD_NULL)
    {
        timer_sech = timer_head;
        timer_prev = (void *)CD_NULL;

        while(timer_sech != CD_NULL)
        {
            cd_timer_t *timer_free = CD_NULL;

            CD_ENTER_CRITICAL();
            timer_sech->timeout--;

            if((0 == timer_sech->timeout) && (timer_sech->reload != 0) && (timer_sech->event_id != SYS_EVE_NONE))
            {
                cd_task_seteve(timer_sech->task_id, timer_sech->event_id);
                timer_sech->timeout = timer_sech->reload;
            }

            if(0 == timer_sech->timeout || SYS_EVE_NONE == timer_sech->event_id)
            {
                if(CD_NULL == timer_prev)
				{
					timer_head = timer_sech->next;
				}
                else
				{
					timer_prev->next = timer_sech->next;
				}

                timer_free = timer_sech;
                timer_sech = timer_sech->next;
            }
            else
            {
                timer_prev = timer_sech;
                timer_sech = timer_sech->next;
            }
            CD_EXIT_CRITICAL();

            if(timer_free != CD_NULL)
            {
                if(timer_free->timeout == 0)
				{
					cd_task_seteve(timer_free->task_id, timer_free->event_id);
				}
                cd_free(timer_free);
            }
        }
    }
}

cd_uint32_t cd_get_systemclock(void)
{
    return cd_systick;
}
