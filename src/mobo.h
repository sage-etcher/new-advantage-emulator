
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

int mobo_load_prom_file (mobo_t *self, char *filename);
void mobo_load_prom (mobo_t *self, uint8_t *buffer, size_t buf_size);

uint8_t mobo_read_dram (mobo_t *self, size_t addr);

uint8_t mobo_get_scroll_register (mobo_t *self);
void    mobo_set_scroll_register (mobo_t *self, uint8_t data);

uint8_t mobo_get_refresh_register   (mobo_t *self);
void    mobo_clear_refresh_register (mobo_t *self);

uint8_t mobo_ram_read (mobo_t *self, size_t addr);
void    mobo_ram_write (mobo_t *self, size_t addr, uint8_t data);
uint8_t mobo_output_byte (mobo_t *self, uint8_t port);
void    mobo_input_byte (mobo_t *self, uint8_t port, uint8_t data);


#endif
/* end of file */
