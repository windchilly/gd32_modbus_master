#include "cd_msg.h"

static cd_msg_queue_t msg_queue_head = NULL;

cd_uint8_t *cd_msg_alloc(cd_uint16_t len)
{
    cd_msg_hdr_t *hdr;

    if(0 == len)
        return NULL;

    hdr = (cd_msg_hdr_t *)cd_malloc((cd_uint16_t)(len + sizeof(cd_msg_hdr_t)));
    if(hdr != NULL)
    {
        hdr->next = NULL;
        hdr->len = len;
        hdr->dest_id = SYS_TSK_INIT;
        return (cd_uint8_t *)(hdr + 1);
    }
    else
	{
		 return NULL;
	}
}

cd_uint8_t cd_msg_free(void *msg_ptr)
{
    cd_uint8_t *ptr;

    if(msg_ptr == NULL || CD_MSG_ID(msg_ptr) != SYS_TSK_INIT){return INVALID_MSG_POINTER;}

    ptr = (cd_uint8_t *)((cd_uint8_t *)msg_ptr - sizeof(cd_msg_hdr_t));
    cd_free((void *)ptr);

    return CD_SUCCESS;
}

static void cd_msg_extract(void *msg_ptr, void *prev_ptr)
{
    if(msg_ptr == msg_queue_head)
        msg_queue_head = CD_MSG_NEXT(msg_ptr);
    else
        CD_MSG_NEXT(prev_ptr) = CD_MSG_NEXT(msg_ptr);

    CD_MSG_NEXT(msg_ptr) = NULL;
    CD_MSG_ID(msg_ptr) = SYS_TSK_INIT;
}

cd_uint8_t cd_msg_send(task_id_t task_id, void *msg_ptr)
{
    if(msg_ptr == NULL)
        return INVALID_MSG_POINTER;

    if(NULL == cd_task_find(task_id))
    {
        cd_msg_free(msg_ptr);
        return INVALID_TASK;
    }

    if(CD_MSG_NEXT(msg_ptr) != NULL || CD_MSG_ID(msg_ptr) != SYS_TSK_INIT)
    {
        cd_msg_free(msg_ptr);
        return INVALID_MSG_POINTER;
    }

    CD_MSG_ID(msg_ptr) = task_id;
    cd_msg_enqueue(msg_ptr);
    cd_task_seteve(task_id, SYS_EVE_MSG);

    return CD_SUCCESS;
}

cd_uint8_t *cd_msg_receive(task_id_t task_id)
{
    cd_msg_hdr_t *list_hdr;
    cd_msg_hdr_t *prev_hdr = NULL;
    cd_msg_hdr_t *found_hdr = NULL;

    list_hdr = msg_queue_head;
    while(list_hdr != NULL)
    {
        if((list_hdr - 1)->dest_id == task_id)
        {
            if(found_hdr == NULL)
                found_hdr = list_hdr;
            else
                break;
        }
        if(found_hdr == NULL)
        {
            prev_hdr = list_hdr;
        }
        list_hdr = CD_MSG_NEXT(list_hdr);
    }

    if(list_hdr != NULL)
        cd_task_seteve(task_id, SYS_EVE_MSG);
    else
        cd_task_clreve(task_id, SYS_EVE_MSG);

    if(found_hdr != NULL)
        cd_msg_extract(found_hdr, prev_hdr);
	else
		return NULL;

    return (cd_uint8_t*)found_hdr;
}

cd_event_hdr_t *cd_msg_find(task_id_t task_id, event_id_t event_id)
{
    cd_msg_hdr_t *header;

    header = (cd_msg_hdr_t *)msg_queue_head;

    while(header != NULL)
    {
        if(((header - 1)->dest_id == task_id) && (((cd_event_hdr_t *)header)->event_id == event_id))
            break;

        header = CD_MSG_NEXT(header);
    }

    return (cd_event_hdr_t *)header;
}

cd_uint8_t cd_msg_count(task_id_t task_id, event_id_t event_id)
{
    cd_msg_hdr_t *header;
    cd_uint8_t msg_num = 0;

    header = msg_queue_head;

    while(header != NULL)
    {
        if(((header-1)->dest_id == task_id) && ((event_id == SYS_EVE_ANY) || (((cd_event_hdr_t *)header)->event_id == event_id)))
            msg_num++;
        header = CD_MSG_NEXT(header);
    }

    return msg_num;
}

void cd_msg_enqueue(void *msg_ptr)
{
    void *list;

    CD_MSG_NEXT(msg_ptr) = NULL;

    if(msg_queue_head == NULL)
        msg_queue_head = msg_ptr;
    else
    {
        for(list = msg_queue_head; CD_MSG_NEXT(list) != NULL; list = CD_MSG_NEXT(list));
        CD_MSG_NEXT(list) = msg_ptr;
    }
}

void *cd_msg_dequeue(void)
{
    void *msg_ptr = NULL;

    if(msg_queue_head != NULL)
    {
        msg_ptr = msg_queue_head;
        msg_queue_head = CD_MSG_NEXT(msg_ptr);
        CD_MSG_NEXT(msg_ptr) = NULL;
        CD_MSG_ID(msg_ptr) = SYS_TSK_INIT;
    }

    return msg_ptr;
}

void cd_msg_push(void *msg_ptr)
{
    CD_MSG_NEXT(msg_ptr) = msg_queue_head;
    msg_queue_head = msg_ptr;
}
