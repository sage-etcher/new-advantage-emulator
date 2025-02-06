
#include "z80.h"

#include "mobo.h"
#include "timer.h"
#include "z80emu.h" /* z80emu external */

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
    _Atomic bool halt;
    Z80_STATE state;
} z80_t;


z80_t *
z80_init (mobo_t *parent)
{
    z80_t *self = NULL;

    self = (z80_t *)malloc (sizeof (z80_t));
    assert (self);
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

    /* check if we are still HALTed every 10ms */
    const struct timespec HALT_CHECK = { .tv_sec = 0, .tv_nsec = 10000000 };

    /* expecting cpu of 4mhz, scale ticks acordingly */
    /* wait for 100us or 10khz every 400 emulated cycles */
    /* current systems cannot wait precise nanoseonds */
    const struct timespec CPU_SPEED = { .tv_sec = 0, .tv_nsec = 100000 };
    const int BUSY_CYCLES = 400;

    struct timespec previous_frame = { 0 };

    assert (self);

    (void)Z80Reset (&self->state);
    while (!mobo_should_exit (self->parent))
    {
        if (self->halt)
        {
            (void)timer (&previous_frame, HALT_CHECK);
            continue;
        }

        (void)Z80Emulate (&self->state, BUSY_CYCLES, self->parent);
        if (self->state.status == Z80_STATUS_HALT) { z80_halt (self); }

        (void)timer (&previous_frame, CPU_SPEED);
    }

    thrd_exit (thrd_success);
}


void
z80_halt (z80_t *self)
{
    assert (self);
    self->halt = true;
}


void
z80_unhalt (z80_t *self)
{
    assert (self);
    self->halt = false;
}


/* end of file */
