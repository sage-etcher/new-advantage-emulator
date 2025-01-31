
#include "crt.h"

#include "mobo.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>


typedef struct crt
{
    mobo_t *parent;

    size_t width;
    size_t height;
    size_t x_offset;
    size_t y_offset;

    uint8_t *buf;
    size_t buf_size;

    uint8_t scroll_offset;
    mtx_t scroll_mtx;

    uint8_t refresh_flag;
    mtx_t refresh_mtx;
} crt_t;


static void crt_copy_to_buffer (crt_t *self);


crt_t *
crt_init (mobo_t *parent, size_t width, size_t height, size_t x_offset, 
          size_t y_offset)
{
    crt_t *self = NULL;
    size_t buf_size = 0;
    
    self = (crt_t *)malloc (sizeof (crt_t));
    assert (self != NULL);
    (void)memset (self, 0, sizeof (crt_t));

    self->parent = parent;
    
    self->width  = width;
    self->height = height;
    self->x_offset = x_offset;
    self->y_offset = y_offset;

    buf_size = width * height;
    self->buf = (uint8_t *)malloc (sizeof (uint8_t) * buf_size);
    assert (self->buf != NULL);
    self->buf_size = buf_size;
    (void)memset (self->buf, 0, self->buf_size);

    (void)mtx_init (&self->scroll_mtx,  mtx_plain);
    (void)mtx_init (&self->refresh_mtx, mtx_plain);

    return self;
}


void
crt_destroy (crt_t *self)
{
    assert (self != NULL);

    free (self->buf);
    self->buf = NULL;
    self->buf_size = 0;

    mtx_destroy (&self->scroll_mtx);
    mtx_destroy (&self->refresh_mtx);

    (void)memset (self, 0, sizeof (crt_t));
    
    free (self);
}


void *
crt_start (crt_t *self)
{ 
    /* 60 hz */
    const struct timespec refresh_rate = { .tv_nsec = 16666666};

    while (!mobo_should_exit (self->parent))
    {
        crt_copy_to_buffer (self);
        crt_set_refresh_register (self, 1);
        printf ("crt start\n");
        (void)thrd_sleep (&refresh_rate, NULL);
    }

    thrd_exit (thrd_success);
}


uint8_t
crt_get_scroll_register (crt_t *self)
{
    uint8_t scroll_offset = 0;

    assert (self != NULL);

    (void)mtx_lock (&self->scroll_mtx);
    scroll_offset = self->scroll_offset;
    (void)mtx_unlock (&self->scroll_mtx);

    return scroll_offset;
}


void
crt_set_scroll_register (crt_t *self, uint8_t data)
{
    assert (self != NULL);

    (void)mtx_lock (&self->scroll_mtx);
    self->scroll_offset = data;
    (void)mtx_unlock (&self->scroll_mtx);
}


uint8_t
crt_get_refresh_register (crt_t *self)
{
    uint8_t refresh_flag = 0;

    assert (self != NULL);

    (void)mtx_lock (&self->refresh_mtx);
    refresh_flag = self->refresh_flag;
    (void)mtx_unlock (&self->refresh_mtx);

    return refresh_flag;
}


void
crt_set_refresh_register (crt_t *self, uint8_t data)
{
    assert (self != NULL);

    (void)mtx_lock (&self->refresh_mtx);
    self->refresh_flag = data;
    (void)mtx_unlock (&self->refresh_mtx);
}


static void
crt_copy_to_buffer (crt_t *self)
{
    size_t x = 0; /* NOLINT */
    size_t y = 0; /* NOLINT */
    size_t cursor = 0;
    size_t addr = 0;
    uint8_t data = 0;
    uint8_t scroll_offset = 0;

    assert (self != NULL);
    assert (self->buf != NULL);
    assert (self->parent != NULL);

    scroll_offset = crt_get_scroll_register (self);

    cursor = 0;
    for (; y < self->height; y++)
    {
        cursor = y * self->width;
        for (x = 0; x < self->width; x++)
        {
            addr = (x * self->x_offset) + (y * self->y_offset);

            assert (cursor < self->buf_size);
            data = mobo_read_dram (self->parent, (addr + scroll_offset));
            self->buf[cursor] = data;
            cursor++;
        }
    }
}


/* end of file */
