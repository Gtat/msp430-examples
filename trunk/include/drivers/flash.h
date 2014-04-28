#ifndef __FLASH_H_GUARD
#define __FLASH_H_GUARD

#include "global.h"

#ifdef CONFIG_ENABLE_STORAGE_MODE
#include <stdint.h>
#include <stddef.h>

struct flash_record
{
  uint8_t * origin;
  size_t    length;
  
  uint8_t * head;
  uint8_t * tail;
  size_t    available;
};

#define FLASH_SEGMENT_SIZE   ((uint16_t)0x200)
#define FLASH_BLOCK_SIZE     ((uint16_t)0x40)
#define FLASH_STORAGE_LENGTH ((uint16_t)0x400)

extern const uint8_t __start_flash_storage;
extern const uint8_t __end_flash_storage;
#define FLASH_ADDR_LO (&__start_flash_storage)
#define FLASH_ADDR_HI (&__end_flash_storage)

#define FLASH_ADDR_RANGE               \
  ((size_t)(FLASH_ADDR_HI - FLASH_ADDR_LO))

#define BIN_POW_MOD(ptr, boundary) \
  ((size_t)(ptr) & ((boundary)-1))
#define ALIGN_DOWN(ptr, boundary) \
  ((size_t)(ptr) & ~((boundary)-1))

enum flash_errors
{
  FLASH_ERR_BAD_ADDR,
  FLASH_ERR_BAD_LENGTH,
  FLASH_ERR_NO_SPACE,
};

int flash_record_init
  (struct flash_record * r, 
   uint8_t * const origin, uint16_t length);

int flash_record_append
  (struct flash_record *r, 
   const uint8_t * src, size_t count);

int flash_record_destructive_read
  (struct flash_record *r, 
   uint8_t * const dst, size_t count);
#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */

#endif /* __FLASH_H_GUARD */

