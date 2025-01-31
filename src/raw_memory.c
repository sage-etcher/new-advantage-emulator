
#include "raw_memory.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>


typedef struct raw_memory
{
    uint8_t *raw;
    size_t size;
    mtx_t memory_lock;
} raw_memory_t;


raw_memory_t *
raw_memory_init (size_t size)
{
    raw_memory_t *self = NULL;
    size_t buf_size = 0;
    size_t alloc_size = 0;

    buf_size = sizeof (uint8_t) * size;
    alloc_size = buf_size + sizeof (raw_memory_t);
    self = (raw_memory_t *)malloc (alloc_size);
    assert (self != NULL);

    /* 0 out the whole allocated chunk */
    (void)memset (self, 0, alloc_size);

    /* extra space was allocated after the structure, for memory buffer.
     * store that pointer into self->raw */
    self->raw = (uint8_t *)&self[1];
    self->size = size;

    (void)mtx_init (&self->memory_lock, mtx_plain);

    return self;
}


void
raw_memory_destroy (raw_memory_t *self)
{
    /* NULL safe */
    if (self == NULL) { return; }
  
    mtx_destroy (&self->memory_lock);

    /* zero out allocated */
    (void)memset (self->raw, 0, self->size);
    (void)memset (self, 0, sizeof (raw_memory_t));

    /* free the allocation */
    free (self);
}


uint8_t
raw_memory_read (raw_memory_t *self, size_t raw_addr)
{
    uint8_t data = 0;

    assert (self != NULL);
    assert (raw_addr < self->size);

    (void)mtx_lock (&self->memory_lock);
    data = self->raw[raw_addr];
    (void)mtx_unlock (&self->memory_lock);

    return data;
}


void
raw_memory_write (raw_memory_t *self, size_t raw_addr, uint8_t data)
{
    assert (self != NULL);
    assert (raw_addr < self->size);

    (void)mtx_lock (&self->memory_lock);
    self->raw[raw_addr] = data;
    (void)mtx_unlock (&self->memory_lock);
}


size_t
raw_memory_size (raw_memory_t *self)
{
    assert (self != NULL);

    return self->size;
}


/* end of file */
