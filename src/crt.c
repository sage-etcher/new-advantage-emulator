
#include "crt.h"

#include "mobo.h"
#include "timer.h"

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

    _Atomic uint8_t scroll_offset;
    _Atomic uint8_t refresh_flag;
} crt_t;


static void crt_copy_to_buffer (crt_t *self);


crt_t *
crt_init (mobo_t *parent, size_t width, size_t height, size_t x_offset, 
          size_t y_offset)
{
    crt_t *self = NULL;
    size_t buf_size = 0;
    
    self = (crt_t *)malloc (sizeof (crt_t));
    assert (self);
    (void)memset (self, 0, sizeof (crt_t));

    self->parent = parent;
    
    self->width  = width;
    self->height = height;
    self->x_offset = x_offset;
    self->y_offset = y_offset;

    buf_size = width * height;
    self->buf = (uint8_t *)malloc (sizeof (uint8_t) * buf_size);
    assert (self->buf);
    self->buf_size = buf_size;
    (void)memset (self->buf, 0, self->buf_size);

    return self;
}


void
crt_destroy (crt_t *self)
{
    assert (self);

    free (self->buf);
    self->buf = NULL;
    self->buf_size = 0;

    (void)memset (self, 0, sizeof (crt_t));
    
    free (self);
}


void *
crt_start (crt_t *self)
{ 
    /* 60 hz */
    const struct timespec REFRESH_RATE = { .tv_sec = 0, .tv_nsec = 16666666};
    struct timespec previous_frame = { 0 };
    struct timespec previous_sleep = { 0 };


    while (!mobo_should_exit (self->parent))
    {
        crt_copy_to_buffer (self);
        crt_set_refresh_register (self, 1);

        printf ("crt start\n");

        previous_sleep = timer (&previous_frame, REFRESH_RATE);
    }

    thrd_exit (thrd_success);
}


uint8_t
crt_get_scroll_register (crt_t *self)
{
    assert (self);
    return self->scroll_offset;
}


void
crt_set_scroll_register (crt_t *self, uint8_t data)
{
    assert (self);
    self->scroll_offset = data;
}


uint8_t
crt_get_refresh_register (crt_t *self)
{
    assert (self);
    return self->refresh_flag;
}


void
crt_set_refresh_register (crt_t *self, uint8_t data)
{
    assert (self);

    self->refresh_flag = data;
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

    assert (self);
    assert (self->buf);
    assert (self->parent);

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
