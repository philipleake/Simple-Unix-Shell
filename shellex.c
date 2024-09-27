/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
void sigint_handler(int sig);
void statusex(int stat);
void printpid();
void printppid();
void help();
void cd(char *inp);
void leave();

char *user = "sh257";

int main(int argc, char *argv[], char *environ[]) 
{
    char cmdline[MAXLINE]; /* Command line */
    if (argc > 2 && strcmp(argv[1], "-p") == 0) {
        /* If -p is provided and there is a prompt argument */
        user = argv[2];
    }
    signal(SIGINT, sigint_handler);
    while (1) {
	/* Read */
	printf("%s> ",user);                   
	Fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
	eval(cmdline);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */

    if (!builtin_command(argv)) { 
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            if (execvp(argv[0], argv) < 0) {
                printf("Execution failed (in fork)\n");
                printf("%s: Command not found.\n", argv[0]);
                exit(1);
            }
        }

	/* Parent waits for foreground job to terminate */
	if (!bg) {
	    int status;
	    if (waitpid(pid, &status, 0) < 0)
		unix_error("waitfg: waitpid error");

        statusex(status);
	}
	else
	    printf("%d %s", pid, cmdline);
    }
    return;
}



/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "exit")) { /* quit command */
	    leave();
    }  
    else if (!strcmp(argv[0], "pid")) {
        printpid();
        return 1;
    }
    else if (!strcmp(argv[0], "ppid")) {
        printppid();
        return 1;
    }
    else if (!strcmp(argv[0], "cd")) {
        cd(argv[1]);
        return 1;
    }
    else if(!strcmp(argv[0], "help")) {
        help();
        return 1;
    }
    if (!strcmp(argv[0], "&")){    /* Ignore singleton & */
	    return 1;
    }
    return 0;
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    return bg;
}
/* $end parseline */

void sigint_handler(int sig) {
    signal(SIGINT, sigint_handler); 
    printf("\n%s> ",user);
    fflush(stdout);
}

void statusex(int stat){
    if(WIFEXITED(stat)){
        printf("Process exited with status code: %d\n", WEXITSTATUS(stat));
    }
}

void leave() {
    raise(SIGTERM);
}

void printpid() {
    printf("%d\n", (int)getpid());
}

void printppid() {
    printf("%d\n", (int)getppid());
}

void cd(char *inp){
    if (inp == NULL) {
        printf("%s\n", getcwd(NULL,0));
    }
    else if (chdir(inp) == 0);
}

void help() {
    printf("**********************************************************************\n");
    printf("A Custom Shell of CMSC 257\n");
    printf("%s\n",user);
    printf("Usage:\n");
    printf("You can change this by <executable> -p <prompt> \n");
    printf("**********************************************************************\n");
    printf("BUILTIN COMMANDS:\n");
    printf("exit: Will terminate and close the shell.\n");
    printf("pid: Will print the process ID.\n");
    printf("ppid: Will print the parent process ID.\n");
    printf("cd: Will print the current directory or change to desired directory.\n");
    printf("SYSTEM COMMANDS:\n");
    printf("Use man pages\n");
}

