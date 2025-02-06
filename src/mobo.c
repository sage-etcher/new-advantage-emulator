
#include "mobo.h"

#include "adv_system_config.h"
#include "crt.h"
#include "fileio.h"
#include "mmu.h"
#include "z80.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>


typedef struct mobo
{
    mmu_t *ram;
    z80_t *cpu;
    crt_t *crt;
    _Atomic bool should_exit;
} mobo_t;


mobo_t *
mobo_init (void)
{

    mobo_t *self = NULL;

    mmu_template_t RAM_PAGES[] = 
    {
        { ADV_PAGE_MAIN_0,    ADV_PAGE_MAIN_SIZE      },
        { ADV_PAGE_MAIN_1,    ADV_PAGE_MAIN_SIZE      },
        { ADV_PAGE_MAIN_2,    ADV_PAGE_MAIN_SIZE      },
        { ADV_PAGE_MAIN_3,    ADV_PAGE_MAIN_SIZE      },
        { ADV_PAGE_DISPLAY_0, ADV_PAGE_DISPLAY_0_SIZE },
        { ADV_PAGE_DISPLAY_1, ADV_PAGE_DISPLAY_1_SIZE },
        { ADV_PAGE_PROM,      ADV_PAGE_PROM_SIZE      },
    };
    const size_t RAM_PAGE_COUNT = (sizeof (RAM_PAGES) / sizeof (RAM_PAGES[0]));


    /* motherboard */
    self = malloc (sizeof (mobo_t));
    assert (self);
    (void)memset (self, 0, sizeof (mobo_t));


    /* ram */
    self->ram = mmu_init_from_template (RAM_PAGES, RAM_PAGE_COUNT);
    (void)mmu_map (self->ram, MMU_PAGE_0, ADV_PAGE_MAIN_0);
    (void)mmu_map (self->ram, MMU_PAGE_1, ADV_PAGE_MAIN_1);
    (void)mmu_map (self->ram, MMU_PAGE_3, ADV_PAGE_MAIN_2);
    (void)mmu_map (self->ram, MMU_PAGE_3, ADV_PAGE_MAIN_3);

    /* cpu */
    self->cpu = z80_init (self);
    
    /* crt display */
    self->crt = crt_init (self, ADV_DISPLAY_WIDTH, ADV_DISPLAY_HEIGHT,
            ADV_DISPLAY_X_OFFSET, ADV_DISPLAY_Y_OFFSET);

    /* if anything fails, panic */
    if (!self->ram || !self->cpu || !self->crt)
    {
        mobo_destroy (self);
        self = NULL;
    }

    self->should_exit = false;

    return self;
}


void
mobo_destroy (mobo_t *self)
{
    if (self == NULL) { return; }

    mmu_destroy (self->ram);
    z80_destroy (self->cpu);
    crt_destroy (self->crt);

    (void)memset (self, 0, sizeof (mobo_t));

    free (self);
}


int
mobo_load_prom_file (mobo_t *self, char *filename)
{
    uint8_t *prom_data = NULL;
    size_t   prom_size = 0;
    size_t   read_size = 0;

    assert (self);

    if (filename == NULL)
    {
        return -1;
    }

    prom_size = file_get_length (filename);
    if (prom_size == 0)
    {
        return -1;
    }

    prom_data = (uint8_t *)malloc (prom_size);
    assert (prom_data);

    read_size = file_read (filename, prom_data, prom_size); 
    mobo_load_prom (self, prom_data, read_size);

    free (prom_data);
    prom_data = NULL;
    prom_size = 0;
    read_size = 0;

    return 0;
}


void
mobo_load_prom (mobo_t *self, uint8_t *buffer, size_t buf_size)
{
    assert (self);
    assert (buffer);
    mmu_memcpy (self->ram, 0x0000, buffer, buf_size);
}



void
mobo_start (mobo_t *self)
{
    char input = 0;
    int retcode = 0;
    char *prom_file = NULL;
    thrd_t threads[4] = { 0 };

    assert (self);

#ifdef DEBUG
    /* prom_file = (char *)"./roms/debug.com"; */
    (void)fprintf (stderr, "debug mode enabled\n");
#endif

    retcode = mobo_load_prom_file (self, prom_file);
    if (retcode)
    {
        /* if no prom is available, start the cpu halted */
        (void)fprintf (stderr, "mobo: no prom loaded\n");
        z80_halt (self->cpu);
    }

    (void)thrd_create (&threads[0], (thrd_start_t)crt_start, self->crt);
    (void)thrd_create (&threads[1], (thrd_start_t)z80_start, self->cpu);
    
    input = getc (stdin);
    mobo_exit (self);

    (void)thrd_join (threads[0], NULL);
    (void)thrd_join (threads[1], NULL);
}


bool
mobo_should_exit (mobo_t *self)
{
    assert (self);
    return self->should_exit;
}


void
mobo_exit (mobo_t *self)
{
    assert (self);
    self->should_exit = true;
}


uint8_t
mobo_get_scroll_register (mobo_t *self)
{
    assert (self);

    return crt_get_scroll_register (self->crt);
}


void
mobo_set_scroll_register (mobo_t *self, uint8_t data)
{
    assert (self);

    crt_set_scroll_register (self->crt, data);
}


uint8_t
mobo_get_refresh_register (mobo_t *self)
{
    assert (self);

    return crt_get_refresh_register (self->crt);
}


void
mobo_clear_refresh_register (mobo_t *self)
{
    assert (self);

    crt_set_refresh_register (self->crt, 0);
}


uint8_t
mobo_read_dram (mobo_t *self, size_t addr)
{
    int page_index = 0;

    page_index = ((addr < ADV_PAGE_DISPLAY_0_SIZE) ? ADV_PAGE_DISPLAY_0
                                                   : ADV_PAGE_DISPLAY_1);

    return mmu_read_raw (self->ram, page_index, addr);
}


uint8_t
mobo_ram_read (mobo_t *self, size_t addr)
{
    uint8_t data = 0;
    assert (self);
    data = mmu_read (self->ram, addr);
    return data;
}


void
mobo_ram_write (mobo_t *self, size_t addr, uint8_t data)
{
    assert (self);
    mmu_write (self->ram, addr, data);
}


uint8_t
mobo_output_byte (mobo_t *self, uint8_t port)
{
    printf ("output on port %u, not permitted\n", port);
    return 0;
}


void
mobo_input_byte (mobo_t *self, uint8_t port, uint8_t data)
{
    printf ("input on port %u of data %u, not permitted\n", port, data);
}


/* end of file */
