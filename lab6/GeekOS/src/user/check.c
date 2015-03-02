#include <geekos/errno.h>
#include <conio.h>
#include <process.h>
#include <string.h>
#include <fileio.h>

#define BUFSIZE 79
#define PASSWORD_PATH "/c/Password.txt"

#define ISSPACE(c) ((c) == ' ' || (c) == '\t')

static void Print_Error(const char *msg, int err) {
    Print("%s: %s\n", msg, Get_Error_String(err));
    Exit(1);
}

/*
 * Skip leading whitespace characters in given string.
 * Returns pointer to first non-whitespace character in the string,
 * which may be the end of the string.
 */
char *Strip_Leading_Whitespace(char *s) {
    while (ISSPACE(*s))
        ++s;
    return s;
}

/*
 * Destructively trim newline from string
 * by changing it to a nul character.
 */
void Trim_Newline(char *s) {
    char *c = strchr(s, '\n');
    if (c != 0)
        *c = '\0';
}

int main(){
	char commandBuf[BUFSIZE + 1];
    char *command;

    int password_fd;
    char buffer[BUFSIZE + 1];
    char *password;
    int ret;
    int read;

    /************************check for password*************************/

    // read password from file
    password_fd = Open(PASSWORD_PATH, O_READ);
    if (password_fd < 0) Print_Error("Could not open file", password_fd);
    ret = Read(password_fd, buffer, sizeof(buffer) - 1);
    if (ret < 0) Print_Error("error reading password file", ret);

    buffer[ret] = '\0';
    password = Strip_Leading_Whitespace(buffer);
    Trim_Newline(password);
    //Print("Real password: %s", buffer);

    if (Close(password_fd) < 0) Print_Error("Could not close file", password_fd);

    /* Print shell prompt (bright cyan on black background) */
    Print("\x1B[1;36mType password:\x1B[37m ");
    /* Read a line of input */
    Read_Line(commandBuf, sizeof(commandBuf));
    command = Strip_Leading_Whitespace(commandBuf);
    Trim_Newline(command);
    
    //Print("Typed password: %s", command);
    if (strcmp(command, password) != 0) {
        /* Exit the shell, not authenticated */
        Print_Error("Authentication Failed!",-11);
    }else{
        Print("Authentication Success!\n");
    }

    /*******************************************************************/
    return 0;
}