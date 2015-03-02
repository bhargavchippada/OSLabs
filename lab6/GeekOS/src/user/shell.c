/*
 * A really, really simple shell program
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2003,2013,2014 Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 *
 * All rights reserved.
 *
 * This code may not be resdistributed without the permission of the copyright holders.
 * Any student solutions using any of this code base constitute derviced work and may
 * not be redistributed in any form.  This includes (but is not limited to) posting on
 * public forums or web sites, providing copies to (past, present, or future) students
 * enrolled in similar operating systems courses the University of Maryland's CMSC412 course.
 *
 * $Revision: 1.21 $
 * 
 */

#include <geekos/errno.h>
#include <conio.h>
#include <process.h>
#include <string.h>

#include <fileio.h>

#define BUFSIZE 79
#define DEFAULT_PATH "/c:/a"
#define PASSWORD_PATH "/c/password.txt"



#define ISSPACE(c) ((c) == ' ' || (c) == '\t')

struct Process {
    char program[BUFSIZE + 1];
    char *command;
    int pid;
};

char *Strip_Leading_Whitespace(char *s);
void Trim_Newline(char *s);
char *Copy_Token(char *token, char *s);
void Spawn_Single_Command(struct Process *proc, const char *path);
void Check_Password();
static void Print_Error(const char *msg, int err);

bool Identify_and_Strip_Ampersand(char *command) {
    char *c;
    for (c = command; *c != '\0'; c++);
    c--;
    if (*c == '&') {
        *c = '\0';
        for (c--; ISSPACE(*c); c--) {
            *c = '\0';
        }
        return true;
    }
    return false;
}

static void Print_Error(const char *msg, int err) {
    Print("%s: %s\n", msg, Get_Error_String(err));
    Exit(1);
}

bool background;                /* making it global is lame, but keeps the signatures unaltered */

int exitCodes = 0;

int main(int argc __attribute__ ((unused)), char **argv
         __attribute__ ((unused))) {
    char commandBuf[BUFSIZE + 1];
    struct Process proc;
    char path[BUFSIZE + 1] = DEFAULT_PATH;
    char *command;

    /* Set attribute to gray on black. */
    Print("\x1B[37m");

    Check_Password();

    while (true) {
        /* Print shell prompt (bright cyan on black background) */
        Print("\x1B[1;36m$\x1B[37m ");

        /* Read a line of input */
        Read_Line(commandBuf, sizeof(commandBuf));
        command = Strip_Leading_Whitespace(commandBuf);
        Trim_Newline(command);
        background = Identify_and_Strip_Ampersand(command);

        /*
         * Handle some special commands
         */
        if (strcmp(command, "exit") == 0) {
            /* Exit the shell */
            break;
        } else if (strcmp(command, "pid") == 0) {
            /* Print the pid of this process */
            Print("%d\n", Get_PID());
            continue;
        } else if (strcmp(command, "exitCodes") == 0) {
            /* Print exit codes of spawned processes. */
            exitCodes = 1;
            continue;
        } else if (strncmp(command, "path=", 5) == 0) {
            /* Set the executable search path */
            strcpy(path, command + 5);
            continue;
        } else if (strcmp(command, "") == 0) {
            /* Blank line. */
            continue;
        }

        proc.command = Strip_Leading_Whitespace(command);
        if (!Copy_Token(proc.program, proc.command)) {
            Print("Error: invalid command\n");
            continue;
        }
        Spawn_Single_Command(&proc, path);
    }

    Print_String("DONE!\n");
    return 0;
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

/*
 * Copy a single token from given string.
 * If a token is found, returns pointer to the
 * position in the string immediately past the token:
 * i.e., where parsing for the next token can begin.
 * If no token is found, returns null.
 */
char *Copy_Token(char *token, char *s) {
    char *t = token;

    while (ISSPACE(*s))
        ++s;
    while (*s != '\0' && !ISSPACE(*s))
        *t++ = *s++;
    *t = '\0';

    return *token != '\0' ? s : 0;
}




/*
 * Spawn a single command.
 */
void Spawn_Single_Command(struct Process *proc, const char *path) {
    int pid;

    pid = proc->pid =
        Spawn_With_Path(proc->program, proc->command, path, background);
    if (pid < 0) {
        Print("Could not spawn process: %s\n", Get_Error_String(pid));
    } else {
        if (background) {
            Print("[%d]\n", pid);
        } else {
            int exitCode = Wait(pid);
            if (exitCodes)
                Print("Exit code was %d\n", exitCode);
        }
    }
}

void Check_Password(){
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
    Print("\x1B[1;36m$\x1B[37m Type password: ");
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
}
