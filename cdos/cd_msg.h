#ifndef _CD_MSG_H_
#define _CD_MSG_H_

#include "cd_task.h"

#define CD_MSG_NEXT(msg_ptr) 	((cd_msg_hdr_t *)(msg_ptr) - 1)->next
#define CD_MSG_ID(msg_ptr) 	((cd_msg_hdr_t *)(msg_ptr) - 1)->dest_id

typedef void* cd_msg_queue_t;
typedef struct
{
    void *next;
    cd_uint16_t len;
    task_id_t dest_id;
}cd_msg_hdr_t;

typedef struct
{
    event_id_t event_id;
    cd_uint8_t status;
}cd_event_hdr_t;

typedef struct
{
    cd_event_hdr_t hdr;
    cd_uint8_t *data;
}cd_msg_bdy_t;

cd_uint8_t *cd_msg_alloc(cd_uint16_t len);
cd_uint8_t cd_msg_free(void *msg_ptr);
cd_uint8_t cd_msg_send(task_id_t task_id, void *msg_ptr);
cd_uint8_t *cd_msg_receive(task_id_t task_id);

cd_event_hdr_t *cd_msg_find(task_id_t task_id, event_id_t event_id);
cd_uint8_t cd_msg_count(task_id_t task_id, event_id_t event_id);

void cd_msg_enqueue(void *msg_ptr);
void *cd_msg_dequeue(void);
void cd_msg_push(void *msg_ptr);

#endif
