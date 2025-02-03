
#include "mmu.h"

#include "raw_memory.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>


typedef uint_fast8_t mmu_page_index_t;

typedef struct mmu_page
{
    size_t offset;
    size_t size;
} mmu_page_t;


typedef struct mmu
{
    raw_memory_t *raw_memory;
    mmu_page_t raw_pages[MMU_ADDRESSABLE_PAGE_CNT];
    mmu_page_index_t mapped[MMU_MAPPED_PAGE_CNT];
    mtx_t map_lock;
} mmu_t;


mmu_t *
mmu_init (void)
{
    mmu_t *self = NULL;

    /* allocate mmu */
    self = (mmu_t *)malloc (sizeof (mmu_t));
    assert (self);
    (void)memset (self, 0, sizeof (mmu_t));
    
    (void)mtx_init(&self->map_lock, mtx_plain);

    return self;
}


void 
mmu_destroy (mmu_t *self)
{
    if (self == NULL) { return; }

    raw_memory_destroy (self->raw_memory);
    mtx_destroy (&self->map_lock);

    (void)memset (self, 0, sizeof (mmu_t));
    free (self);
}


static mmu_page_index_t
mmu_decode_mapped_page (mmu_t *self, size_t addr)
{
    mmu_page_index_t mapped_index = 0;
    mmu_page_index_t page_index = 0;

    assert (self);

    mapped_index = addr / MMU_MAPPED_PAGE_SIZE;
    assert (mapped_index < MMU_MAPPED_PAGE_CNT);

    (void)mtx_lock (&self->map_lock);
    page_index = self->mapped[mapped_index];
    (void)mtx_unlock (&self->map_lock);

    return page_index;
}


static size_t
mmu_decode_page_addr (mmu_page_t *p_page, size_t addr)
{
    return (addr % MMU_MAPPED_PAGE_SIZE) % p_page->size;
}


static size_t
mmu_decode_raw_addr (mmu_page_t *p_page, size_t addr)
{
    size_t raw_addr = 0;
   
    assert (p_page);
    raw_addr = addr + p_page->offset;

    return raw_addr;
}


static size_t
mmu_decode_raw_page (mmu_t *self, mmu_page_index_t page_index, size_t addr)
{
    mmu_page_t *p_page = NULL;
    size_t page_addr = 0;
    size_t raw_addr  = 0;

    assert (page_index < MMU_ADDRESSABLE_PAGE_CNT);
    p_page = &self->raw_pages[page_index];

    page_addr = mmu_decode_page_addr (p_page, addr);
    raw_addr  = mmu_decode_raw_addr (p_page, page_addr);

    return raw_addr;
}



static size_t
mmu_decode_addr (mmu_t *self, size_t addr)
{
    mmu_page_index_t page_index = 0;
    mmu_page_index_t raw_addr = 0;

    assert (self);

    page_index = mmu_decode_mapped_page (self, addr);
    raw_addr   = mmu_decode_raw_page (self, page_index, addr);

    return raw_addr;
}



uint8_t
mmu_read  (mmu_t *self, size_t addr)
{
    size_t raw_addr = 0;
    uint8_t read_byte = 0;

    assert (self);

    raw_addr  = mmu_decode_addr (self, addr);
    read_byte = raw_memory_read (self->raw_memory, raw_addr);

    return read_byte;
}


void
mmu_write (mmu_t *self, size_t addr, uint8_t data)
{
    size_t raw_addr = 0;

    assert (self);

    raw_addr = mmu_decode_addr (self, addr);
    raw_memory_write (self->raw_memory, raw_addr, data);
}


uint8_t
mmu_read_raw (mmu_t *self, int page_index, size_t addr)
{
    size_t raw_addr = addr; /* use raw addressing for all negative indexes */
    uint8_t read_byte = 0;

    assert (self);
  
    /* if page_index is positive, deref from the raw_page */
    if (page_index >= 0)
    {
        raw_addr = mmu_decode_raw_page (self, page_index, addr);
    }

    read_byte = raw_memory_read (self->raw_memory, raw_addr);

    return read_byte;
}


void   
mmu_write_raw (mmu_t *self, int page_index, size_t addr, uint8_t data)
{
    size_t raw_addr = addr; /* use raw addressing for all negative indexes */

    assert (self);
  
    /* if page_index is positive, deref from the raw_page */
    if (page_index >= 0)
    {
        raw_addr = mmu_decode_raw_page (self, page_index, addr);
    }

    raw_memory_write (self->raw_memory, raw_addr, data);
}


int
mmu_create_raw_memory (mmu_t *self, size_t raw_mem_size)
{
    assert (self);

    /* if raw_memory is already set, panic */
    if (self->raw_memory) 
    {
        return -1;
    }

    self->raw_memory = raw_memory_init (raw_mem_size);

    return 0;
}


void
mmu_create_page (mmu_t *self, int page_index, size_t offset, size_t size)
{
    size_t end_addr = 0;

    assert (self);
    assert (self->raw_memory);
    assert (page_index < MMU_ADDRESSABLE_PAGE_CNT);
    assert (page_index >= 0);

    end_addr = offset + size;
    assert (end_addr <= raw_memory_size (self->raw_memory));
    assert (size <= MMU_MAPPED_PAGE_SIZE);

    self->raw_pages[page_index].offset = offset;
    self->raw_pages[page_index].size   = size;
}


void
mmu_map (mmu_t *self, int map_page, int raw_page)
{
    assert (self);
    assert (map_page < MMU_MAPPED_PAGE_CNT);
    assert (map_page >= 0);
    assert (raw_page < MMU_ADDRESSABLE_PAGE_CNT);
    assert (raw_page >= 0);

    (void)mtx_lock (&self->map_lock);
    self->mapped[map_page] = raw_page;
    (void)mtx_unlock (&self->map_lock);
}


mmu_t *
mmu_init_from_template (mmu_template_t *template, size_t n)
{
    mmu_t *self = NULL;
    size_t i = 0; /* NOLINT: i is known, I dont care that it's short. */
    mmu_template_t *iter = NULL;
    size_t offset = 0;
    size_t raw_size = 0;

    assert (template);
    assert (n < MMU_ADDRESSABLE_PAGE_CNT);


    /* calculate the total size needed */
    for (i = 0; i < n; i++)
    {
        raw_size += template[i].size;
    }

    /* initialize mmut */
    self = mmu_init ();

    /* initialize raw_memory */
    mmu_create_raw_memory (self, raw_size);

    /* initialize all pages */
    for (i = 0; i < n; i++)
    {
        iter = &template[i];

        mmu_create_page (self, iter->index, offset, iter->size);

        offset += iter->size;
    }

    return self;
}


/* end of file */
