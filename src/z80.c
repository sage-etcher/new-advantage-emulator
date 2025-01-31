
#include "z80.h"

#include "mobo.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>


typedef struct z80
{
    mobo_t *parent;
    uint32_t ticks;

    int nothing_here;
} z80_t;


z80_t *
z80_init (mobo_t *parent)
{
    z80_t *self = NULL;

    self = (z80_t *)malloc (sizeof (z80_t));
    assert (self != NULL);
    (void)memset (self, 0, sizeof (z80_t));

    self->parent = parent;

    return self;
}


void
z80_destroy (z80_t *self)
{
    if (self == NULL) { return; }

    (void)memset (self, 0, sizeof (z80_t));

    free (self);
}


void *
z80_start (z80_t *self)
{
    /* expecting cpu of 4mhz, scale ticks acordingly */
    /* wait for 100us or 10khz */
    const struct timespec ts = { .tv_nsec = 100000 };
    uint32_t ticks_before_sleep = 400;

    uint8_t refresh_status = 0;

    self->ticks = 0;

    while (!mobo_should_exit (self->parent))
    {
        if (mobo_get_refresh_register (self->parent))
        {
            printf ("detected screen refresh\n");
        }
        mobo_clear_refresh_register (self->parent);

        self->ticks += 20;
        while (self->ticks>= ticks_before_sleep)
        {
            /* printf ("cpu busy waiting\n"); */
            putc ('.', stdout);
            self->ticks -= ticks_before_sleep;
            (void)thrd_sleep (&ts, NULL);
        }
    }

    thrd_exit (thrd_success);
}


/* end of file */
