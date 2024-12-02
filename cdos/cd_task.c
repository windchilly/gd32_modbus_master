#include "cd_task.h"

static cd_task_t *task_head = CD_NULL;

void cd_task_init(void)
{
    cd_task_t *task_index;

    task_index = task_head;

    while(task_index != CD_NULL)
    {
        if(task_index->ops->init != CD_NULL)
		{
			 task_index->ops->init(task_index->task_id);
		}
        task_index = task_index->next;
    }

    task_index = (cd_task_t *)CD_NULL;
}

cd_err_t cd_task_create(cd_task_ops_t *ops, task_id_t task_id)
{
    cd_task_t *task_new;
    cd_task_t *task_sech;
    cd_task_t **task_index;

    task_index = &task_head;
    task_sech = task_head;

    task_new = (cd_task_t *)cd_malloc(sizeof(cd_task_t));
	task_new->ops = (cd_task_ops_t *)cd_malloc(sizeof(cd_task_ops_t));
    if(CD_NULL == task_new || CD_NULL == task_new->ops)
	{
		LOG_E("TASK CREATE FAIL");
		return CD_ERROR;
	}

    task_new->ops->init = ops->init;
    task_new->ops->handler = ops->handler;
    task_new->task_id = task_id;
    task_new->events = SYS_EVE_NONE;
    task_new->next = (cd_task_t *)CD_NULL;

    while(task_sech != CD_NULL)
    {
        if(task_new->task_id > task_sech->task_id)
        {
            task_new->next = task_sech;
            *task_index = task_new;
            return CD_SUCCESS;
        }
        task_index = &task_sech->next;
        task_sech = task_sech->next;
    }

    *task_index = task_new;

    return CD_SUCCESS;
}

cd_task_t *cd_task_active(void)
{
    cd_task_t *task_sech;

    task_sech = task_head;
    while(task_sech != CD_NULL)
    {
        if(task_sech->events != 0)
		{
			return task_sech;
		}
        task_sech = task_sech->next;
    }

    return CD_NULL;
}

cd_task_t *cd_task_find(task_id_t task_id)
{
    cd_task_t *task_sech;
    task_sech = task_head;

    while(task_sech != CD_NULL)
    {
        if(task_sech->task_id == task_id)
		{
			return task_sech;
		}
        task_sech = task_sech->next;
    }

    return CD_NULL;
}

cd_uint16_t cd_task_cnt(void)
{
    cd_task_t *task_sech;
    cd_uint16_t num = 0;

    task_sech = task_head;
    while(task_sech != CD_NULL)
    {
        num++;
        task_sech = task_sech->next;
    }

    return num;
}

cd_err_t cd_task_seteve(task_id_t task_id, event_id_t event_id)
{
    cd_task_t *task;

    task = cd_task_find(task_id);
    if(task != CD_NULL)
	{
		task->events |= event_id;
	}
    else
	{
		return CD_ERROR;
	}

    return CD_SUCCESS;
}

cd_err_t cd_task_clreve(task_id_t task_id, event_id_t event_id)
{
    cd_task_t *task;

    task = cd_task_find(task_id);
    if(task != CD_NULL)
	{
		task->events &= ~event_id;
	}
    else
	{
		return INVALID_TASK;
	}

    return CD_SUCCESS;
}
