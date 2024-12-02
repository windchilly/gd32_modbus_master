#ifndef  _CD_TASK_H_
#define  _CD_TASK_H_

#include "cdos.h"

#define SYS_EVE_NONE    (0)
#define SYS_EVE_MSG     (1 << 15)
#define SYS_EVE_ANY     (0xFFFF)

#define SYS_TSK_INIT    (0xFFFF)

typedef cd_uint16_t task_id_t;
typedef cd_uint16_t event_id_t;
typedef event_id_t event_asb_t;

typedef struct _cd_task
{
    struct _cd_task *next;
    task_id_t task_id;
    event_asb_t events;

    struct _cd_task_ops *ops;
}cd_task_t;

typedef struct _cd_task_ops
{
    void (*init)(task_id_t task_id);
    cd_uint16_t (*handler)(task_id_t task_id, event_asb_t events);
}cd_task_ops_t;

cd_err_t cd_task_create(cd_task_ops_t *ops, task_id_t task_id);
void cd_task_init(void);

cd_task_t *cd_task_active(void);
cd_task_t *cd_task_find(task_id_t task_id);
cd_uint16_t cd_task_cnt(void);

cd_err_t cd_task_seteve(task_id_t task_id, event_id_t event_id);
cd_err_t cd_task_clreve(task_id_t task_id, event_id_t event_id);

#endif
