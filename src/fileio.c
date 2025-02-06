

#include "fileio.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


size_t
file_get_length (char *filename)
{
    errno_t errno_backup = 0;
    FILE *fp = NULL; /* NOLINT: fp is a known mnemonic */
    long fp_offset = 0L;
    size_t filesize = 0;

    fp = fopen (filename, "rb");
    if (!fp)
    {
        return 0;
    }

    (void)fseek (fp, 0L, SEEK_END);
    fp_offset = ftell (fp);
    errno_backup = errno;

    (void)fclose (fp);
    fp = NULL;
   
    if (fp < 0)
    {
        errno = errno_backup;
        return 0;
    }
    filesize = (size_t)fp_offset;

    return filesize;
}

size_t 
file_read (char *filename, uint8_t *buffer, size_t buf_max)
{
    FILE *fp = NULL; /* NOLINT: fp is a known mnemonic */
    size_t read_count = 0;

    assert (filename);
    assert (buffer);
    assert (buf_max);

    fp = fopen (filename, "rb");
    if (!fp)
    {
        return 0;
    }

    read_count = fread (buffer, sizeof (uint8_t), buf_max, fp);

    /* if read bytes is less than the expected amount, check for an error.
     * if the file does have an error, return 0 bytes read. */
    if ((read_count != buf_max) &&
        (ferror (fp)))
    {
        read_count = 0;
    }

    (void)fclose (fp);
    fp = NULL;

    return read_count;
}


/* end of file */
