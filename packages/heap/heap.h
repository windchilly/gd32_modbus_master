#ifndef __HEAP_H__
#define __HEAP_H__

#include <stdlib.h>
#include <stdint.h>

/* 堆的大小(字节为单位,在1024字节以上时建议采用例如 8*1024 的形式表示 8KB) */
#define	heapTOTAL_HEAP_SIZE			(size_t)(8*1024)

/* 字节对齐长度(1/2/4/8字节) */
#define heapALIGNMENT_BYTE			( 8 )

/* 字节对齐掩码，用于计算 */
#define heapALIGNMENT_MASK	        ( heapALIGNMENT_BYTE - 1 )

/* 进入和退出临界区，也可以创建互斥信号量 */
#define heapINTO_CRITICAL() __asm__ __volatile__("cpsid i");
#define	heapEXIT_CRITICAL() __asm__ __volatile__("cpsie i");

/* API函数 */
void*  pvHeapMalloc( size_t xWantedSize );
void   vHeapFree( void *pv );
size_t xHeapGetFreeSize( void );
size_t xHeapGetMinimumEverFreeSize(void);

#endif /* MEMMANG_HEAP_H */
