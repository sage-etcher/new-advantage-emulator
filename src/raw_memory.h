
#ifndef EMU_RAW_MEMORY_HEADER
#define EMU_RAW_MEMORY_HEADER

#include <stddef.h>
#include <stdint.h>


typedef struct raw_memory raw_memory_t;


raw_memory_t *raw_memory_init (size_t size);

void raw_memory_destroy (raw_memory_t *self);

uint8_t raw_memory_read (raw_memory_t *self, size_t raw_addr);

void raw_memory_write (raw_memory_t *self, size_t raw_addr, uint8_t data);

size_t raw_memory_size (raw_memory_t *self);


#endif
/* end of file */
