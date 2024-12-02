#ifndef _CD_TIMER_H_
#define _CD_TIMER_H_

#include "cd_task.h"

typedef struct
{
    void *next;
    cd_uint16_t timeout;
    cd_uint16_t reload;
    event_id_t event_id;
    task_id_t task_id;
}cd_timer_t;

cd_err_t cd_timer_startex(task_id_t task_id, event_id_t event_id, cd_uint16_t timeout);
cd_err_t cd_timer_startre(task_id_t task_id, event_id_t event_id, cd_uint16_t timeout);
cd_err_t cd_timer_stopex(task_id_t task_id, event_id_t event_id);
void cd_timer_del(cd_timer_t *timer);

cd_uint8_t cd_timer_num(void);
cd_uint16_t cd_timer_gettimeoutex(task_id_t task_id, event_id_t event_id);

void cd_timer_update(void);

#endif
