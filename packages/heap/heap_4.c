#include "heap.h"

/* �˽ṹΪ���п�����ṹ,�������ӿ��п� */
typedef struct BLOCK_LINK
{
	struct BLOCK_LINK *pxNext;  /* ��һ�����п��ַ */
    size_t			   xSize;   /* �˿��п��С */
}BlockLink_t;

/* ��������Ľṹ���С */
#define heapSTRUCT_SIZE			((sizeof(BlockLink_t) + (heapALIGNMENT_BYTE - 1)) & ~heapALIGNMENT_MASK)

/* ������С�Ŀ��С */
#define heapMINIMUM_BLOCK_SIZE	((size_t)(heapSTRUCT_SIZE * 2))

/* һ���ֽڵ�λ�� */
#define heapBITS_PER_BYTE		((size_t)8)

static uint8_t ucHeap[heapTOTAL_HEAP_SIZE];


/* ����������ʼ�ڵ�������յ�ڵ�ָ�� */
static BlockLink_t xStart, *pxEnd = NULL;

/* ʣ������ֽ��� */
static size_t xFreeBytesRemaining = 0U;

/* ��ʷ��С�����ֽ��� */
static size_t xMinimumEverFreeBytesRemaining = 0U;

/* �˿��Ƿ񱻷����־��ʹ�ÿ�Ĵ�С���λ����ʶ */
static size_t xBlockAllocatedBit = 0;

/******************************************************************************
* �� �� ����prvHeapInit
* ��    �ܣ��ڴ�����ʼ��
* �����������
* ������������
* ʹ�þ�����prvHeapInit();
*******************************************************************************/
static void prvHeapInit(void)
{
    BlockLink_t *pxFirstFreeBlock;                  /* ��һ�����п�ָ�� */
    uint8_t *pucAlignedHeap;                        /* �����Ķѵ�ַ */
    size_t uxAddress;                               /* ��ʱ��ŵ�ֵַ */
    size_t xTotalHeapSize = heapTOTAL_HEAP_SIZE;    /* �ܶѴ�С */

    /* ��ʱ����ѵ�ַ */
    uxAddress = (size_t)ucHeap;
    /* ����ѵ�ַû���� */
    if ((uxAddress & heapALIGNMENT_MASK) != 0)
    {
        /* ���ж������ */
        uxAddress += (heapALIGNMENT_BYTE - 1);
        uxAddress &= ~((size_t)heapALIGNMENT_MASK);
        /* ����Ϊ�������ܶѴ�С */
        xTotalHeapSize -= uxAddress - (size_t)ucHeap;
    }

    /* �����Ķѵ�ַ */
    pucAlignedHeap = (uint8_t *)uxAddress;

    /* ����������ʼ�ڵ����һ���ڵ�ָ������Ķѵ�ַ */
    xStart.pxNext = (void *)pucAlignedHeap;
    /* ��ʼ�ڵ㲻�����κ�������Ϣ��������ָʾ���п�����ͷ�����Դ�СΪ0 */
    xStart.xSize = (size_t)0;

    /* �õ��ѵ�β����ַ���ڴ�ſ������������ */
    uxAddress = ((size_t)pucAlignedHeap) + xTotalHeapSize;
    uxAddress -= heapSTRUCT_SIZE;
    uxAddress &= ~((size_t)heapALIGNMENT_MASK);
    /* ������ָ��ָ��˵�ַ */
    pxEnd = (void *)uxAddress;
    /* �������СΪ0����������ʾ���п�������� */
    pxEnd->xSize = 0;
    /* ����һ���ڵ� */
    pxEnd->pxNext = NULL;

    /* ��һ�����п�Ϊ�����Ķѵ�ַ */
    pxFirstFreeBlock = (void *)pucAlignedHeap;
    /* ���СΪ������ĵ�ַ��ȥ�ѵ�ַ */
    pxFirstFreeBlock->xSize = uxAddress - (size_t)pxFirstFreeBlock;
    /* ����һ�����п�Ϊ������ */
    pxFirstFreeBlock->pxNext = pxEnd;

    /* ��û�н����ڴ���䣬��ʷ��С�����ֽ���Ϊ�������öѿռ� */
    xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xSize;
    /* �����ֽ���ҲΪ�������öѿռ� */
    xFreeBytesRemaining = pxFirstFreeBlock->xSize;

    /* ��λ���λ����ʾ�ڴ��Ѿ����䣨��31λ�� */
    xBlockAllocatedBit = ((size_t)1) << ((sizeof(size_t)* heapBITS_PER_BYTE) - 1);
}

/******************************************************************************
* �� �� ����prvInsertBlockIntoFreeList
* ��    �ܣ������ڵ㵽����������
* �����������
* ������������
* ʹ�þ�����prvHeapInit();
*******************************************************************************/
static void prvInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert)
{
    BlockLink_t *pxIterator;    /* �����ڵ㣬���ڱ��� */
    uint8_t *puc;               /* ��ʱ�������ָ�� */

    /* ���ڿ��п��ǰ���ַ��С���еģ�������Ҫ��ͷ�����ҵ������ */
    for (pxIterator = &xStart; pxIterator->pxNext < pxBlockToInsert; pxIterator = pxIterator->pxNext)
    {
        /* ����ʲô��������ֻ��Ϊ�˱���������㣬�������˻��˳�ѭ�� */
    }

    /* �鿴������Ŀ��п��Ƿ��ڿ������������ڵ���������� */
    puc = (uint8_t *)pxIterator;
    if ((puc + pxIterator->xSize) == (uint8_t *)pxBlockToInsert)
    {
        /* ������ھͽ��кϲ���ֻ��Ҫ�������Ŀ��С�ϲ����� */
        pxIterator->xSize += pxBlockToInsert->xSize;
        /* �ϲ��󽫺ϲ���Ŀ��ַ����������ڵ�ָ�� */
        pxBlockToInsert = pxIterator;
    }

    /* �鿴���п������ϵ�ǰ�����Ƿ���Ժ���һ���ڵ�ϲ� */
    puc = (uint8_t *)pxBlockToInsert;
    if ((puc + pxBlockToInsert->xSize) == (uint8_t *)pxIterator->pxNext)
    {
        /* [a]������ھͽ��кϲ����ϲ�ʱ��Ҫ�ж��ǲ���β��� */
        if (pxIterator->pxNext != pxEnd)
        {
            /* [b]�������β��㣬���кϲ���ֻ��Ҫ���������ϲ���ɾ����һ�ڵ㼴�� */
            pxBlockToInsert->xSize += pxIterator->pxNext->xSize;
            pxBlockToInsert->pxNext = pxIterator->pxNext->pxNext;
        }
        else
        {
            /* [b]�����β��㣬����ǰ������һ������β��� */
            pxBlockToInsert->pxNext = pxEnd;
        }
    }
    else
    {
        /* [a]��������ھͽ���ǰ���������� */
        pxBlockToInsert->pxNext = pxIterator->pxNext;
    }

    /* �ж��Ƿ���й�ǰ��ϲ� */
    if (pxIterator != pxBlockToInsert)
    {
        /* ���û�У����в������ */
        pxIterator->pxNext = pxBlockToInsert;
    }
}

/******************************************************************************
* �� �� ����pvHeapMalloc
* ��    �ܣ��ڴ�����
* ���������xWantedSize - �����С
* ����������void * ���͵ĵ�ַָ��
* ʹ�þ�����p = pvHeapMalloc(100);
*******************************************************************************/
void* pvHeapMalloc( size_t xWantedSize )
{
	BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
	void *pvReturn = NULL;

	heapINTO_CRITICAL();
	{
		/* If this is the first call to malloc then the heap will require
		initialisation to setup the list of free blocks. */
		if( pxEnd == NULL )
		{
			prvHeapInit();
		}

		if( ( xWantedSize & xBlockAllocatedBit ) == 0 )
		{
			if( xWantedSize > 0 )
			{
				xWantedSize += heapSTRUCT_SIZE;

				if( ( xWantedSize & heapALIGNMENT_MASK ) != 0x00 )
				{
					xWantedSize += ( heapALIGNMENT_BYTE - ( xWantedSize & heapALIGNMENT_MASK ) );
				}
			}

			if( ( xWantedSize > 0 ) && ( xWantedSize <= xFreeBytesRemaining ) )
			{
				/* Traverse the list from the start	(lowest address) block until
				one	of adequate size is found. */
				pxPreviousBlock = &xStart;
				pxBlock = xStart.pxNext;
				while( ( pxBlock->xSize < xWantedSize ) && ( pxBlock->pxNext != NULL ) )
				{
					pxPreviousBlock = pxBlock;
					pxBlock = pxBlock->pxNext;
				}

				/* If the end marker was reached then a block of adequate size
				was	not found. */
				if( pxBlock != pxEnd )
				{
					/* Return the memory space pointed to - jumping over the
					BlockLink_t structure at its start. */
					pvReturn = ( void * ) ( ( ( uint8_t * ) pxPreviousBlock->pxNext ) + heapSTRUCT_SIZE );

					/* This block is being returned for use so must be taken out
					of the list of free blocks. */
					pxPreviousBlock->pxNext = pxBlock->pxNext;

					/* If the block is larger than required it can be split into
					two. */
					if( ( pxBlock->xSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
					{
						/* This block is to be split into two.  Create a new
						block following the number of bytes requested. The void
						cast is used to prevent byte alignment warnings from the
						compiler. */
						pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + xWantedSize );
						
						/* Calculate the sizes of two blocks split from the
						single block. */
						pxNewBlockLink->xSize = pxBlock->xSize - xWantedSize;
						pxBlock->xSize = xWantedSize;

						/* Insert the new block into the list of free blocks. */
						prvInsertBlockIntoFreeList( pxNewBlockLink );
					}

					xFreeBytesRemaining -= pxBlock->xSize;

					if( xFreeBytesRemaining < xMinimumEverFreeBytesRemaining )
					{
						xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
					}

					/* The block is being returned - it is allocated and owned
					by the application and has no "next" block. */
					pxBlock->xSize |= xBlockAllocatedBit;
					pxBlock->pxNext = NULL;
				}
			}
		}
	}
	heapEXIT_CRITICAL();

	return pvReturn;
}

/******************************************************************************
* �� �� ����vHeapFree
* ��    �ܣ��ڴ��ͷ�
* ���������pv - ���ͷ�ָ��
* ������������
* ʹ�þ�����vHeapFree(p);
*******************************************************************************/
void vHeapFree( void *pv )
{
    uint8_t *puc = (uint8_t *)pv;   /* ��ʱ��������ָ�� */
    BlockLink_t *pxLink;            /* ���нڵ���ʱ����ָ�� */

    /* ������ͷŵ�ַ��Ч */
	if( pv != NULL )
	{
        /* �õ����ͷŵ�ʵ�ʽڵ��׵�ַ */
		puc -= heapSTRUCT_SIZE;

		/* ��ȡ���ͷŽڵ��ַ */
		pxLink = ( void * ) puc;

        /* ����ڴ��Ƿ���� */
		if( ( pxLink->xSize & xBlockAllocatedBit ) != 0 )
		{
            /* ����ѷ���*/
			if( pxLink->pxNext == NULL )
			{
				/* ȡ���ڴ����λ */
				pxLink->xSize &= ~xBlockAllocatedBit;
                
                /* �����ٽ��� */
				heapINTO_CRITICAL();
				{
					/* ����ʣ��ռ��С */
					xFreeBytesRemaining += pxLink->xSize;

                    /* ���˿��������������� */
					prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
				}
                /* �˳��ٽ��� */
				heapEXIT_CRITICAL();
			}
		}
	}
}

/******************************************************************************
* �� �� ����xHeapGetFreeSize
* ��    �ܣ���ȡ�����ڴ�
* �����������
* ���������������ڴ��С
* ʹ�þ�����vHeapFree(p);
*******************************************************************************/
size_t xHeapGetFreeSize(void)
{
	return xFreeBytesRemaining;
}

/******************************************************************************
* �� �� ����xHeapGetMinimumEverFreeSize
* ��    �ܣ���ȡ��ʷ��С����
* �����������
* ������������ʷ��С���д�С
* ʹ�þ�����xHeapGetMinimumEverFreeSize(p);
*******************************************************************************/
size_t xHeapGetMinimumEverFreeSize(void)
{
	return xMinimumEverFreeBytesRemaining;
}
