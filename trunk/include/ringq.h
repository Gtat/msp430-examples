/**
 * @file    ringq.h
 * @author  Sam Boling <charles.samuel.boling@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 * 
 * Macros to generically define a ring queue/circular buffer/FIFO structure
 * at compile time. POP() routines employ GCC statement expressions, 
 * supported also by the TI compiler suite.
 */
#ifndef __RINGQ_H_GUARD
#define __RINGQ_H_GUARD

#include <stdint.h>

/** \def RING_QUEUE_DECLARE(type, elem_ct)
 *  Generate the declaration for a FIFO structure.
 *
 *  @param type    The type of elements in the FIFO.
 *  @param elem_ct The maximum number of elements in the FIFO.
 */
#define RING_QUEUE_DECLARE(type, elem_ct) \
  struct                                  \
  {                                       \
    type             data[elem_ct];       \
    uint8_t          head,   tail;        \
    volatile uint8_t length;              \
    const uint8_t    depth;               \
  } 

/** \def RING_QUEUE_INIT(elem_ct)
 *  Generate the initializer for a FIFO structure.
 *
 *  @param elem_ct The maximum number of elements in the FIFO.
 */
#define RING_QUEUE_INIT(elem_ct) \
  {                              \
    .head   = 0,                 \
    .tail   = 0,                 \
    .length = 0,                 \
    .depth  = elem_ct,           \
    .data   = { 0 },             \
  }

/** \def RING_QUEUE_CREATE(type, elem_ct, name)
 *  Declare and initialize a FIFO with the given name and attributes.
 *
 *  @param type    The type of elements in the FIFO.
 *  @param elem_ct The maximum number of elements in the FIFO.
 *  @param name    The identifier name for the created FIFO.
 */
#define RING_QUEUE_CREATE(type, elem_ct, name) \
  RING_QUEUE_DECLARE(type, elem_ct) name = RING_QUEUE_INIT(elem_ct)

/** \def RING_QUEUE_EMPTY(q)
 *  Check if the given FIFO is empty.
 *
 *  @param  q  The FIFO to examine.
 *  @retval 0  The queue is not empty.
 *  @retval 1  The queue is empty.
 */
#define RING_QUEUE_EMPTY(q) \
  (!q.length)

/** \def RING_QUEUE_FULL(q)
 *  Check if the given FIFO is full.
 *
 *  @param  q  The FIFO to examine.
 *  @retval 0  The queue is not full.
 *  @retval 1  The queue is full.
 */
#define RING_QUEUE_NOT_FULL(q) \
  (q.length < q.depth)

#define RING_QUEUE_FULL(q) \
  (!RING_QUEUE_NOT_FULL(q))

/** \def RING_QUEUE_PUSH_ALWAYS(q, elem)
 *  Add an element to the given FIFO. Don't check the length -- overwrite
 *  old elements if the buffer is full.
 *
 *  @param q    The FIFO to manipulate, created with RING_QUEUE_CREATE().
 *  @param elem The element to add, of the type appropriate to q.
 */
#define RING_QUEUE_PUSH_ALWAYS(q, elem)          \
  do                                             \
  {                                              \
    q.data[q.head] = elem;                       \
    q.head       = (q.head + 1) & (q.depth - 1); \
  } while (0)

/** \def RING_QUEUE_PUSH(q, elem)
 *  Add an element to the given FIFO. Check the length -- do nothing if the
 *  buffer is full.
 *
 *  @param q    The FIFO to manipulate, created with RING_QUEUE_CREATE().
 *  @param elem The element to add, of the type appropriate to q.
 */
#define RING_QUEUE_PUSH(q, elem)       \
  do                                   \
  {                                    \
    if (!RING_QUEUE_FULL(q))           \
    {                                  \
      RING_QUEUE_PUSH_ALWAYS(q, elem); \
      q.length++;                      \
    }                                  \
  } while (0)

/** \def RING_QUEUE_POP_ALWAYS(q)
 *  Remove an element to the given FIFO. Don't check the length -- the
 *  queue is likely to be broken if it is empty.
 *
 *  @param  q   The FIFO to manipulate, created with RING_QUEUE_CREATE().
 *  @return     The oldest element in the queue.
 */
#define RING_QUEUE_POP_ALWAYS(q)            \
  ({                                        \
     __typeof__(q.data[0]) ret;             \
     ret    = q.data[q.tail];               \
     q.tail = (q.tail + 1) & (q.depth - 1); \
     ret;                                   \
  }) 

/** \def RING_QUEUE_POP(q)
 *  Remove an element to the given FIFO. Check the length and return 0
 *  without manipulating the queue if it is empty.
 *
 *  @param  q    The FIFO to manipulate, created with RING_QUEUE_CREATE().
 *  @return      The oldest element in the queue if nonempty, 0 otherwise.  
 */
#define RING_QUEUE_POP(q)             \
  ({                                  \
    __typeof__(q.data[0]) ret;        \
    ret = 0;                          \
    if (!RING_QUEUE_EMPTY(q))                 \
    {                                 \
      q.length--;                     \
      ret = RING_QUEUE_POP_ALWAYS(q); \
    }                                 \
    ret;                              \
  })

#define RING_QUEUE_POP_MANY(q, dest, count)  \
  ({                                         \
    size_t i, limit;                         \
    __typeof(q.data[0]) *p;                  \
                                             \
    p     = (dest);                          \
    limit = (count);                         \
                                             \
    for (i=0; /* side effects only once */   \
         i < limit && !RING_QUEUE_EMPTY(q);  \
         i++)                                \
    {                                        \
      *p++ = RING_QUEUE_POP_ALWAYS(q);       \
      q.length--;                            \
    }                                        \
    i;                                       \
  })


#endif /* __RINGQ_H_GUARD */

