
#include "mobo.h"

#include "adv_system_config.h"
#include "crt.h"
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
    bool should_exit;
    mtx_t exit_lock;
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
    assert (self != NULL);
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
    (void)mtx_init (&self->exit_lock, mtx_plain);

    return self;
}


void
mobo_destroy (mobo_t *self)
{
    if (self == NULL) { return; }

    mmu_destroy (self->ram);
    z80_destroy (self->cpu);
    crt_destroy (self->crt);

    mtx_destroy (&self->exit_lock);

    (void)memset (self, 0, sizeof (mobo_t));

    free (self);
}


void
mobo_start (mobo_t *self)
{
    char input = 0;
    thrd_t threads[4] = { 0 };

    assert (self != NULL);

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
    bool status = false;

    assert (self != NULL);

    (void)mtx_lock (&self->exit_lock);
    status = self->should_exit;
    (void)mtx_unlock (&self->exit_lock);

    return status;
}


void
mobo_exit (mobo_t *self)
{
    assert (self != NULL);

    (void)mtx_lock (&self->exit_lock);
    self->should_exit = true;
    (void)mtx_unlock (&self->exit_lock);

}


uint8_t
mobo_get_scroll_register (mobo_t *self)
{
    assert (self != NULL);

    return crt_get_scroll_register (self->crt);
}


void
mobo_set_scroll_register (mobo_t *self, uint8_t data)
{
    assert (self != NULL);

    crt_set_scroll_register (self->crt, data);
}


uint8_t
mobo_get_refresh_register (mobo_t *self)
{
    assert (self != NULL);

    return crt_get_refresh_register (self->crt);
}


void
mobo_clear_refresh_register (mobo_t *self)
{
    assert (self != NULL);

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

/* end of file */
