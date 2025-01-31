
#ifndef ADV_CONST_HEADER
#define ADV_CONST_HEADER


/* mmu config */
#define ADV_PAGE_MAIN_0     MMU_PAGE_0
#define ADV_PAGE_MAIN_1     MMU_PAGE_1
#define ADV_PAGE_MAIN_2     MMU_PAGE_2
#define ADV_PAGE_MAIN_3     MMU_PAGE_3
#define ADV_PAGE_MAIN_SIZE  0x4000

#define ADV_PAGE_DISPLAY_0       MMU_PAGE_4
#define ADV_PAGE_DISPLAY_1       MMU_PAGE_5
#define ADV_PAGE_DISPLAY_0_SIZE  0x4000
#define ADV_PAGE_DISPLAY_1_SIZE  0x1000

#define ADV_PAGE_PROM       MMU_PAGE_6
#define ADV_PAGE_PROM_SIZE  0x0200


/* display config */
#define ADV_DISPLAY_WIDTH        80
#define ADV_DISPLAY_HEIGHT      240
#define ADV_DISPLAY_X_OFFSET 0x0100
#define ADV_DISPLAY_Y_OFFSET 0x0001


#endif /* ADV_CONST_HEADER */
/* end of file */
