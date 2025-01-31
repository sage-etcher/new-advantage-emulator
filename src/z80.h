
#ifndef EMU_Z80_HEADER
#define EMU_Z80_HEADER

#include "mobo.h"


typedef struct z80 z80_t;


z80_t *z80_init (mobo_t *parent);

void z80_destroy (z80_t *self);

void *z80_start (z80_t *self);

#endif
/* end of file */
