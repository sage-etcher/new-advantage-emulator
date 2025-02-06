
#ifndef EMU_MMU_HEADER
#define EMU_MMU_HEADER

#include <stddef.h>
#include <stdint.h>


#define MMU_MAPPED_PAGE_SIZE 0x4000
#define MMU_MAPPED_PAGE_CNT 4
#define MMU_ADDRESSABLE_PAGE_CNT 16
enum
{
    MMU_PAGE_0, MMU_PAGE_1, MMU_PAGE_2, MMU_PAGE_3,
    MMU_PAGE_4, MMU_PAGE_5, MMU_PAGE_6, MMU_PAGE_7,
    MMU_PAGE_8, MMU_PAGE_9, MMU_PAGE_A, MMU_PAGE_B,
    MMU_PAGE_C, MMU_PAGE_D, MMU_PAGE_E, MMU_PAGE_F
};

typedef struct mmu mmu_t;

typedef struct mmu_page_map
{
    int index;
    size_t size;
} mmu_template_t;


mmu_t *mmu_init (void);

int mmu_create_raw_memory (mmu_t *self, size_t raw_mem_size);

void mmu_create_page (mmu_t *self, int page_index, size_t offset, size_t size);

mmu_t *mmu_init_from_template (mmu_template_t *template, size_t n);

void mmu_destroy (mmu_t *self);

uint8_t mmu_read  (mmu_t *self, size_t addr);

void mmu_write (mmu_t *self, size_t addr, uint8_t data);

uint8_t mmu_read_raw  (mmu_t *self, int page_index, size_t addr);

void mmu_write_raw (mmu_t *self, int page_index, size_t addr, uint8_t data);

void mmu_map (mmu_t *self, int map_page, int raw_page);

void mmu_memcpy (mmu_t *self, size_t addr, uint8_t *buffer, size_t bufn);

#endif
/* end of file */
