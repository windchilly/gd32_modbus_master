#include "cd_task.h"

static volatile cd_uint32_t system_run_tick = 0;
static volatile cd_uint32_t task_run_tick = 0;

static void cd_system_start(void)
{
    event_asb_t events, ret_events;
    cd_task_t *task_active;

    while(1)
	{
        task_active = cd_task_active();

        if(task_active != NULL)
		{
			task_run_tick++;
            events = task_active->events;
            task_active->events = SYS_EVE_NONE;

            if(events != SYS_EVE_NONE)
			{
                if(task_active->ops->handler != NULL)
				{
                    ret_events = task_active->ops->handler(task_active->task_id, events);
                    task_active->events |= ret_events;
                }
            }
        }

		system_run_tick++;
	}
}

static void cdos_schedule(void)
{
	cd_task_init();
	cd_system_start();
}
INIT_CDOS_EXPORT(cdos_schedule);

static void usage(void)
{
	if(system_run_tick <= task_run_tick)
	{
		system_run_tick = 0;
		task_run_tick = 0;
		cd_printf("SYSTEM USAGE: %s", "NEW CYCLE");
		return;
	}
	
	LOG_I("TASK RUN CNT: %d", task_run_tick);
	LOG_I("SYSTEM RUN CNT: %d", system_run_tick);
	cd_printf("SYSTEM USAGE: %d%", task_run_tick*100*100/system_run_tick);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)
|SHELL_CMD_DISABLE_RETURN, usage, usage, system usage);
