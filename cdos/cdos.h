#ifndef _CD_OS_H_
#define _CD_OS_H_

#include "heap.h"
#include "shell.h"
#include "log.h"
#include "cm_backtrace.h"

#define CD_TRUE				1
#define CD_FALSE			0
#define CD_NULL				0

#define CD_SUCCESS			1
#define INVALID_TASK		2
#define INVALID_MSG_POINTER	3
#define INVALID_EVENT_ID	4
#define INVALID_TIMER		5
#define CD_ERROR			(0xFFFFFFFF)

#define CD_DEV_NAME_MAX		(10)

#define CD_CONSOLEBUF_SIZE	(128)

#define CDOS_MVERSION		(1L)
#define CDOS_SUBVERSION		(0L)
#define CDOS_REVISION		(0L)
#define CDOS_VERSION		((CDOS_MVERSION * 10000) + (CDOS_SUBVERSION * 100) + CDOS_REVISION)

#define CD_ASSERT(expr) 	((expr) ? (void)0U : assert_fn())

typedef unsigned char cd_bool_t;
typedef unsigned char cd_uint8_t;
typedef unsigned short cd_uint16_t;
typedef unsigned int cd_uint32_t;
typedef char cd_int8_t;
typedef short cd_int16_t;
typedef int cd_int32_t;
typedef int cd_err_t;

#include <stdarg.h>
#define CD_SECTION(x)				__attribute__((section(x)))
#define CD_UNUSED					__attribute__((unused))
#define CD_USED						__attribute__((used))
#define CD_ALIGN(n)					__attribute__((aligned(n)))
#define CD_PRE_MAIN_INIT			__attribute__((constructor))
#define CD_POST_MAIN_INIT			__attribute__((destructor))

#define CD_WEAK						__attribute__((weak))
#define cd_inline					static __inline

#define cd_container_of(ptr, type, member) ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

typedef void (*init_fn_t)(void);
#define INIT_EXPORT(fn, level) CD_USED const init_fn_t __cdos_init_##fn CD_SECTION(".cdsec_fn." level) = fn
#define INIT_BOARD_EXPORT(fn)		INIT_EXPORT(fn, "1")
#define INIT_PREV_EXPORT(fn)		INIT_EXPORT(fn, "2")
#define INIT_DEVICE_EXPORT(fn)		INIT_EXPORT(fn, "3")
#define INIT_COMPONENT_EXPORT(fn)	INIT_EXPORT(fn, "4")
#define INIT_ENV_EXPORT(fn)			INIT_EXPORT(fn, "5")
#define INIT_APP_EXPORT(fn)			INIT_EXPORT(fn, "6")
#define INIT_CDOS_EXPORT(fn)		INIT_EXPORT(fn, "7")

#define CD_ENTER_CRITICAL()			__asm__ __volatile__("cpsid i");
#define CD_EXIT_CRITICAL()  		__asm__ __volatile__("cpsie i");

#define cd_malloc(x)				pvHeapMalloc(x)
#define cd_free(x)					vHeapFree(x)
#define cd_delay(ms)				HAL_Delay(ms)

extern volatile uint32_t segger_jscope_count;

char *cd_strdup(const char *s);
unsigned long cd_strnlen(const char *s, unsigned long maxlen);
cd_int32_t cd_strcmp(const char *cs, const char *ct);
cd_int32_t cd_strncmp(const char *cs, const char *ct, unsigned long count);
char *cd_strncpy(char *dst, const char *src, unsigned long n);
cd_int32_t cd_strcasecmp(const char *a, const char *b);
char *cd_strstr(const char *s1, const char *s2);
unsigned long cd_strlen(const char *s);

cd_int32_t cd_memcmp(const void *cs, const void *ct, unsigned long count);
void *cd_memmove(void *dest, const void *src, unsigned long n);
void *cd_memcpy(void *dst, const void *src, unsigned long count);
void *cd_memset(void *s, int c, unsigned long count);

void assert_fn(void);
void cd_printf(const char *fmt, ...);
cd_int32_t cd_snprintf(char *buf, unsigned long size, const char *fmt, ...);

#endif
