
#include "emu.h"

#include "mobo.h"

#include <assert.h>
#include <stddef.h> /* provide: NULL, size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>


typedef struct emu
{
    /* gtk_asserts_t *gtk; */
    mobo_t *core;
} emu_t;


emu_t *
emu_init (void)
{
    emu_t *self = NULL;

    self = (emu_t *)malloc (sizeof (emu_t));
    assert (self != NULL);
    (void)memset (self, 0, sizeof (emu_t));

    self->core = mobo_init ();

    return self;
}


void
emu_destroy (emu_t *self)
{
    assert (self != NULL);

    mobo_destroy (self->core);

    (void)memset (self, 0, sizeof (emu_t));

    free (self);
}


void
emu_start (void)
{
    emu_t *self = NULL;

    self = emu_init ();

    mobo_start (self->core);

    emu_destroy (self);
    self = NULL;
}



/* end of file */
