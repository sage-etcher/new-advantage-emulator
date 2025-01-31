
#ifndef EMU_HEADER 
#define EMU_HEADER

typedef struct emu emu_t;


emu_t *emu_init (void);

void emu_destroy (emu_t *self);

void emu_start (void);


#endif
/* end of file */
