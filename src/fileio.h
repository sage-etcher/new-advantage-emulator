
/** provides basic file input/output to the host.
 *
 * Copyright (c) 2025 Sage I. Hendricks 
 * License: MIT
 */

#ifndef ADV_FILEIO_HEADER
#define ADV_FILEIO_HEADER

#include <stddef.h>
#include <stdint.h>


size_t file_get_length (char *filename);

size_t file_read (char *filename, uint8_t *buffer, size_t buf_max);



#endif
/* end of file */
