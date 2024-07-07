#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXARGS 128



struct command {
    int argc;
    char *argv[MAXARGS];
    enum builtin_t { NONE, QUIT, JOBS, BG, FG } builtin;
};

int parse(const char *cmdline, struct command *cmd);
enum builtin_t parseBuiltin(struct command *cmd);
void runSystemCommand(struct command *cmd, int bg);
void runBuiltinCommand(struct command *cmd);
void eval(char *cmdline);



int MAXLINE = 1024;
char prompt[] = "minishell$ ";  // command line prompt



void runSystemCommand(struct command *cmd, int bg)
{
    pid_t childPid;

    // Fork !!!
    if ((childPid = fork()) < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (childPid == 0)
    {
        if(execvp(cmd->argv[0], cmd->argv) == -1)
        {
            perror("execvp");
            exit(0);
        }
    }else{
        if (bg)
            printf("Child in background [%d]\n", childPid);
        else
            waitpid(childPid, NULL, 0);
    }
}

void runBuiltinCommand(struct command *cmd)
{
    switch (cmd->builtin)
    {
    case QUIT:
        exit(0);
        break;
    case JOBS:
        printf("JOBS\n");
        break;
    case BG:
        printf("BG\n");
        break;
    case FG:
        printf("FG\n");
        break;
    default:
        break;
    }
}


void eval(char *cmdline)
{
    int bg;
    struct command cmd;

    // printf("Evaluating '%s' \n", cmdline);
    // parse line into cmd structure
    bg = parse(cmdline, &cmd);

    // parse error
    if (bg == -1) return;

    // empty line -ignore
    if (cmd.argv[0] == NULL) return;

    if (cmd.builtin == NONE)
    {
        runSystemCommand(&cmd, bg);
    }else
    {
        runBuiltinCommand(&cmd);
    }
}

// cmd->builtin = parseBuiltin(cmd);

//parse built function to parse builtin commands

enum builtin_t parseBuiltin(struct command *cmd)
{
    if (strcmp(cmd->argv[0], "quit") == 0)
        return QUIT;
    if (strcmp(cmd->argv[0], "jobs") == 0)
        return JOBS;
    if (strcmp(cmd->argv[0], "bg") == 0)
        return BG;
    if (strcmp(cmd->argv[0], "fg") == 0)
        return FG;
    return NONE;
}

// void eval(char *cmdline)
// {
//     int bg;

//     struct command cmd;

//     printf("Evaluating '%s' \n", cmdline);

//     // parse cmdline into cmd structure
//     bg = parse(cmdline, &cmd);
//     printf("Found Command %s\n", cmd.argv[0]);
// }

int parse(const char *cmdline, struct command *cmd)
{
    char *array = strdup(cmdline); // Duplicate cmdline
    const char delims[10] = " \t\r\n";
    char *line = array;
    char *token;
    char *endline;
    int is_bg;

    if (cmdline == NULL)
    {
        printf("Command line is NULL\n");
        free(array); // Free allocated memory before returning
        return -1;
    }

    endline = line + strlen(line);

    // Build argv list
    cmd->argc = 0;
    while (line < endline)
    {
        // Skip delimiters
        line += strspn(line, delims);
        if (line >= endline) break;

        // Find token delimiter
        token = line + strcspn(line, delims);

        // Record token as argument
        if (token > line) {
            cmd->argv[cmd->argc] = strndup(line, token - line); // Duplicate token
            cmd->argc++;
        }

        // Check if argv is full
        if (cmd->argc >= MAXARGS - 1) break;

        line = token;
    }

    // Argument list must end with a NULL pointer
    cmd->argv[cmd->argc] = NULL;

    if (cmd->argc == 0) // Ignore blank line
    {
        free(array); // Free allocated memory before returning
        return -1;
    }

    cmd->builtin = parseBuiltin(cmd);

    // Should the job run in the background?
    if ((is_bg = (cmd->argv[cmd->argc - 1] && *cmd->argv[cmd->argc - 1] == '&')) != 0)
        cmd->argv[--cmd->argc] = NULL;

    free(array); // Free allocated memory before returning
    return is_bg;
}

int main(int argc, char **argv)
{
    char cmdline[MAXLINE]; // Buffer for fgets

    while (1)
    {
        printf("%s", prompt);

        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            perror("fgets error");

        if (feof(stdin))
        {
            printf("\n");
            exit(0);
        }

        // Remove trailing newline
        cmdline[strcspn(cmdline, "\n")] = '\0';

        // Evaluate command line
        eval(cmdline);
    }
}