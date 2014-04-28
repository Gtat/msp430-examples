#include "global.h"

#ifdef CONFIG_ENABLE_STORAGE_MODE

#include "drivers/flash.h"
#include "utility.h"
#include "ram_symbols.h"

/* forward declarations for operations that must reside in RAM */
static void flash_erase_block 
  (uint8_t *p)
  __attribute__(( section(".ram_symbols") 
                , noinline 
               ));

static void flash_write
  (struct flash_record * r,
   const uint8_t * src, size_t count)
  __attribute__(( section(".ram_symbols") 
                , noinline 
               )); 

int flash_record_init
  (struct flash_record *r, 
   uint8_t * const origin, uint16_t length)
{
  if ((origin < FLASH_ADDR_LO) || (origin > FLASH_ADDR_HI))
  {
    return -FLASH_ERR_BAD_ADDR;
  }
  if (length > FLASH_ADDR_RANGE)
  {
    return -FLASH_ERR_BAD_LENGTH;
  }

  r->head = r->tail = r->origin = origin;
  r->available = r->length = length; 
 
  return 0;
}

int flash_record_append
  (struct flash_record * r, 
   const uint8_t * src, size_t count)
{
  size_t written;
  size_t rem, chunk;

  for (written = 0; written < count; written += chunk)
  {
    if (!r->available)
    {
      return -FLASH_ERR_NO_SPACE;
    }

    chunk = min(count, r->available);
    rem   = BIN_POW_MOD(r->tail, FLASH_SEGMENT_SIZE);
    if (!rem)
    {
      chunk = min(chunk, FLASH_SEGMENT_SIZE);
      (*RAM_CODE_PTR(flash_erase_block))(r->tail);
    }
    else
    {
      chunk = min(chunk, rem);
    }
    (*RAM_CODE_PTR(flash_write))(r, src + written, chunk);
  }
  return written;
}

int flash_record_destructive_read
  (struct flash_record * r, 
   uint8_t * const dst, size_t count)
{
  size_t i, read;
  size_t rem, chunk;

  uint8_t * old_segment;
  
  for (read = 0; read < count; read += chunk)
  {
    if (r->available == r->length)
    {
      break;
    }

    old_segment = (uint8_t *)ALIGN_DOWN(r->head, FLASH_SEGMENT_SIZE);
    rem = FLASH_SEGMENT_SIZE - BIN_POW_MOD(r->head, FLASH_SEGMENT_SIZE);
    chunk = min(count, r->length - r->available);
    chunk = min(chunk, rem);

    for (i = read; i < read + chunk; i++)
    {
      dst[i] = *r->head++;
    }

    if (old_segment != (uint8_t *)ALIGN_DOWN(r->head, FLASH_SEGMENT_SIZE))
    {
      (*RAM_CODE_PTR(flash_erase_block))(old_segment);
    } 
    r->available += chunk;
  }
  return read;
}

/* operations that must reside in RAM */
static void flash_erase_block  
  (uint8_t *p)
{
  FCTL3 = FWKEY;         /* clear flash lock */
  FCTL1 = FWKEY | ERASE; /* erase will occur on next write */
  *p = 0;                /* dummy write */
  while (FCTL3 & BUSY);
}

static void flash_write
  (struct flash_record * r,
   const uint8_t * src, size_t count)
{
  size_t i;

  FCTL1 = FWKEY | BLKWRT | WRT; 

  for (i = 0; i < count; ++i)
  {
    r->available -= 1;
    *r->tail++ = *src++; 
    while (!(FCTL3 & WAIT));
  } 

  FCTL1 = FWKEY;
  while (FCTL3 & BUSY);
  FCTL3 = FWKEY | LOCK;
}

#endif /* #ifdef CONFIG_ENABLE_STORAGE_MODE */

