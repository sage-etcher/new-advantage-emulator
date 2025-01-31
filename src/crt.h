
#ifndef EMU_CRT_HEADER
#define EMU_CRT_HEADER

#include <stddef.h>

#include "mobo.h"


typedef struct crt crt_t;


crt_t *crt_init (mobo_t *parent, size_t width, size_t height, size_t x_offset,
                 size_t y_offset);

void crt_destroy (crt_t *self);

void *crt_start (crt_t *self);


void    crt_set_refresh_register (crt_t *self, uint8_t data);
uint8_t crt_get_refresh_register (crt_t *self);

void    crt_set_scroll_register (crt_t *self, uint8_t data);
uint8_t crt_get_scroll_register (crt_t *self);


#endif
/* end of file */
