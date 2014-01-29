#ifndef __RINGQ_H_GUARD
#define __RINGQ_H_GUARD

#include <stdint.h>

#define RING_QUEUE_DECLARE(type, elem_ct) \
  struct                                  \
  {                                       \
    type         data[elem_ct];           \
    unsigned int head,   tail;            \
    unsigned int length;                  \
    const unsigned int depth;             \
  } 

#define RING_QUEUE_INIT(elem_ct) \
  {                              \
    .head   = 0,                 \
    .tail   = 0,                 \
    .length = 0,                 \
    .depth  = elem_ct,           \
  }

#define RING_QUEUE_CREATE(type, elem_ct, name) \
  RING_QUEUE_DECLARE(type, elem_ct) name = RING_QUEUE_INIT(elem_ct)

/*#define RING_QUEUE_INIT(q)     \
  ({                           \
    q.depth  = (sizeof(q.data)/sizeof(q.data[0])); \
    q.head   = q.tail = 0;     \
    q.length = 0;              \
    q;                         \
  }) 
*/
#define RING_QUEUE_PUSH_ALWAYS(q, elem)          \
  do                                             \
  {                                              \
    q.data[q.head] = elem;                       \
    q.head       = (q.head + 1) & (q.depth - 1); \
  } while (0)

#define RING_QUEUE_PUSH(q, elem)       \
  do                                   \
  {                                    \
    if (q.length < q.depth)            \
    {                                  \
      RING_QUEUE_PUSH_ALWAYS(q, elem); \
      q.length++;                      \
    }                                  \
  } while (0)

#define RING_QUEUE_POP_ALWAYS(q)            \
  ({                                        \
     __typeof__(q.data[0]) ret;             \
     ret    = q.data[q.tail];               \
     q.tail = (q.tail + 1) & (q.depth - 1); \
     ret;                                   \
  }) 

#define RING_QUEUE_POP(q)             \
  ({                                  \
    __typeof__(q.data[0]) ret;        \
    ret = 0;                          \
    if (q.length > 0)                 \
    {                                 \
      q.length--;                     \
      ret = RING_QUEUE_POP_ALWAYS(q); \
    }                                 \
    ret;                              \
  })

#define RING_QUEUE_EMPTY(q) \
  (!q.length)

#endif

