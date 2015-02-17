#include <geekos/syscall.h>
#include <string.h>
#include <fmtout.h>
#include <conio.h>

static bool s_echo = true;

static int get_position_of_last_character_in_buffer(char *buf, int n,
                                                    int startcol) {
    int newcol = startcol;
    int i;
    for (i = 0; i < n; ++i) {
        char ch = buf[i];
        if (ch == '\t') {
            int rem = newcol % TABWIDTH;
            newcol += (rem == 0) ? TABWIDTH : (TABWIDTH - rem);
        } else {
            ++newcol;
        }
    }
    return newcol;
}

void Read_Line_New(char *buf, size_t bufSize) {
    char *ptr = buf;
    size_t n = 0;
    Keycode k;
    bool done = false;
    int startrow = 0, startcol = 0;
    Get_Cursor(&startrow, &startcol);
    /*Print("Start column is %d\n", startcol); */

    bufSize--;
    do {
        k = (Keycode) Get_Key();
        if ((k & KEY_SPECIAL_FLAG) || (k & KEY_RELEASE_FLAG))
            continue;

        // k &= 0xff;
        if (k == '\r')
            k = '\n';

        //        Print("k:%d\n", k);

        /* keep k's modifiers while checking for special keys */
        if (k == ASCII_BS || k == (KEY_CTRL_FLAG | 'h') || k == KEY_KPDEL) {
            if (n > 0) {
                char last = *(ptr - 1);
                int newcol = startcol;

                /* Back up in line buffer */
                --ptr;
                --n;

                if (s_echo) {
                    /*
                     * Figure out what the column position of the last
                     * character was
                     */
                    newcol =
                        get_position_of_last_character_in_buffer(buf, n,
                                                                 startcol);

                    /* Erase last character */
                    if (last != '\t')
                        last = ' ';
                    Put_Cursor(startrow, newcol);
                    Put_Char(last);
                    Put_Cursor(startrow, newcol);
                }
            }
            continue;
        }

        /* ^U clears the line. -nspring */
        if (k == (KEY_CTRL_FLAG | 'u')) {
            Put_Cursor(startrow, startcol);
            int lastcol =
                get_position_of_last_character_in_buffer(buf, n, startcol);
            int i;
            for (i = startcol; i < lastcol; i++) {
                Put_Char(' ');
            }
            Put_Cursor(startrow, startcol);
            ptr = buf;
            n = 0;
        }

        if ((k & KEY_CTRL_FLAG) || (k & KEY_ALT_FLAG)) {
            /* we don't know how to handle control characters. */
            /* so don't add them. just do nothing. */
            continue;
        }

        /* discard any remaining modifiers now that we're echoing */
        k &= 0xff;

        if (s_echo)
            Put_Char(k);

        if (k == '@')
            done = true;

        if (n < bufSize) {
            if (k != '@') *ptr++ = k;
            ++n;
        }
    }
    while (!done);

    *ptr = '\0';
}

int main(){
	char inp[1000];
	Read_Line_New(inp, 1000);
	Print("\n");
	Print_String(inp);
	return 0;
}
