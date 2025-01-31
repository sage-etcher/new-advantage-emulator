
#ifndef MOBO_EMU_HEADER
#define MOBO_EMU_HEADER

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


typedef struct mobo mobo_t;


mobo_t *mobo_init (void);

void mobo_destroy (mobo_t *self);

void mobo_start (mobo_t *self);

bool mobo_should_exit (mobo_t *self);

void mobo_exit (mobo_t *self);

uint8_t mobo_read_dram (mobo_t *self, size_t addr);

uint8_t mobo_get_scroll_register (mobo_t *self);
void    mobo_set_scroll_register (mobo_t *self, uint8_t data);

uint8_t mobo_get_refresh_register   (mobo_t *self);
void    mobo_clear_refresh_register (mobo_t *self);


#endif
/* end of file */
