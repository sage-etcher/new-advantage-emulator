

#include "control_panel.h"

#include "crt.h"
#include "mmu.h"
#include "mobo.h"
#include "z80.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GETLINE_OK         0
#define GETLINE_NO_INPUT  -1
#define GETLINE_TOO_LARGE -2

static int
my_getline (char *prompt, char *buffer, size_t buf_size)
{
    int character = 0;
    int extra = 0;

    /* get line with buffer overrun protection */
    if (prompt)
    {
        printf ("%s", prompt);
        (void)fflush (stdout);
    }

    if (fgets (buffer, (int)buf_size, stdin) == NULL)
    {
        return GETLINE_NO_INPUT;
    }

    if (buffer[strlen (buffer) - 1] != '\n')
    {
        extra = 0;
        while (((character = getchar ()) != '\n') && (character != EOF))
        {
            extra = 1;
        }
        return (extra == 1) ? GETLINE_TOO_LARGE : GETLINE_OK;
    }

    buffer[strlen (buffer) - 1] = '\0';
    return GETLINE_OK;

}

#define CMD_SIZE 128
#define CMD_NEWLINE "\n"
#define CMD_EOS     "\0"

#define CMD_REQUIRES_PARAMETER " "

#define CMD_QUIT       "quit"
#define CMD_HALT       "halt"
#define CMD_CONTINUE   "continue"
#define CMD_STATUS     "status"
#define CMD_LOAD_WORD  "load"
#define CMD_LOAD       (CMD_LOAD_WORD CMD_REQUIRES_PARAMETER)


void
control_panel_start (mobo_t *self)
{
    int retcode = 0;
    const char *PROMPT_TEXT = "> ";
    char command[CMD_SIZE] = { 0 };
    char *arguement = NULL;

    mobo_t *mobo = NULL;
    z80_t  *cpu  = NULL;
    crt_t  *crt  = NULL;
    mmu_t  *ram  = NULL;

    assert (self);

    mobo = self;
    cpu = mobo_get_cpu (mobo);
    crt = mobo_get_crt (mobo);
    ram = mobo_get_ram (mobo);

    /* note: this whole mess is gross, will refactor later, rn function over
     *       nicetys lol. disgusting tho frfr ew. */
    while (!mobo_should_exit (mobo))
    {
        /* get input */
        retcode = my_getline ((char *)PROMPT_TEXT, command, CMD_SIZE);
        if ((retcode == GETLINE_NO_INPUT) ||
            (strcmp (command, CMD_NEWLINE) == 0) ||
            (strcmp (command, CMD_EOS) == 0))
        {
            continue;
        }
        if (retcode == GETLINE_TOO_LARGE)
        {
            (void)fprintf (stderr, "command overflow\n");
            continue;
        }

        /* proccess the input */
        if (strcmp (command, CMD_QUIT) == 0)
        {
            mobo_exit (mobo);
        }
        else if (strcmp (command, CMD_HALT) == 0)
        {
            z80_halt (cpu);
        }
        else if (strcmp (command, CMD_CONTINUE) == 0)
        {
            z80_unhalt (cpu);
        }
        else if (strncmp (command, CMD_LOAD, sizeof (CMD_LOAD) - 1) == 0)
        {
            arguement = command + sizeof (CMD_LOAD) - 1;
            retcode = mobo_load_prom_file (self, arguement);
            if (retcode)
            {
                printf ("failed to load prom.\n");
            }
        }
        else if (strcmp (command, CMD_LOAD_WORD) == 0)
        {
            printf ("no arguement\n");
            printf ("usage: %s <file>\n", CMD_LOAD_WORD);
        }
        else if (strcmp (command, CMD_STATUS) == 0)
        {
            printf ("unimplimented, sorry!\n");
        }
        else
        {
            printf ("%s?\n", command);
        }
    } /* main loop */
}
