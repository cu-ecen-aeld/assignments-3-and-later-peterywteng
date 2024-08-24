#include "systemcalls.h"
#include <stdlib.h>  // system()
#include <unistd.h>  // execv(), dup2()
#include <sys/wait.h>  // waitpid()
#include <errno.h>  // errno
#include <string.h>  // strerror()
#include <fcntl.h>  // O_CREAT, O_WRONLY

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/

    bool ret = false;
    if ( system(cmd) != -1 ) {
        ret = true;
    } else {
        printf("%s: system(%s) error!\n", __func__, cmd);
    }

    //printf("system(%s) = %d\n", cmd, ret);

    return ret;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    /* Child process: run command
       Parent process: wait child process; return true only when waitpid & WIFEXITED return ok */
    pid_t pid, pid_wait;
    int status;
    bool ret = false;
    int reti;

    fflush(stdout);  // avoid duplicate prints using fork()
    pid = fork();  // create child process
    if ( pid < 0 ) {
        printf("%s: fork() error - returning %d", __func__, pid);

    } else if ( pid > 0 ) {
        printf("%s: This is parent process of pid#%d\n", __func__, pid);
        pid_wait = waitpid(pid, &status, 0);

        printf("%s: Child process pid#%d terminated with WIFEXITED(status)=%d and WEXITSTATUS=%d\n", __func__, pid_wait, WIFEXITED(status), WEXITSTATUS(status));

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            return false;
        } else {
            ret = true;
        }

    } else {
        printf("%s: This is child process (pid#%d)\n", __func__, pid);
        reti = execv(command[0], command);
        if ( reti < 0 ) {
            printf("%s: execv(%s, ...) error - returning %d\n", __func__, command[0], reti);
            printf("%s: Errorno: %d (%s)\n", __func__, errno, strerror(errno));
            //printf("%s: The exec() function should have returned false since echo was not specified\n", __func__);
            printf("%s: Ready to exit due to error!\n", __func__);
            exit(EXIT_FAILURE);  // if return -1, parent process will return 0
        } else {
            printf("%s: execv(%s, ...) passed - returning %d\n", __func__, command[0], reti);
            ret = true;
        }
    }

    va_end(args);

    printf("%s: return %s\n", __func__, ret ? "true" : "false");
    return ret;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    /* Child process: run command -> output to "outputfile"
       Parent process: wait child process; return true only when waitpid & WIFEXITED return ok */
    pid_t pid, pid_wait;
    int status;
    bool ret = false;
    int reti;

    int fd=open(outputfile, 
                O_CREAT|O_WRONLY,
                0644);
    if( fd == -1 ) {
        printf("%s: Error %d (%s) opening %s\n", __func__, errno, strerror(errno), outputfile);
        close(fd);
        return false;
    }


    fflush(stdout);  // avoid duplicate prints using fork()
    pid = fork();  // create child process
    if ( pid < 0 ) {
        printf("%s: fork() error - returning %d", __func__, pid);
    } else if ( pid > 0 ) {
        printf("%s: This is parent process of pid#%d\n", __func__, pid);
        pid_wait = waitpid(pid, &status, 0);

        printf("%s: Child process pid#%d terminated with WIFEXITED(status)=%d and WEXITSTATUS=%d\n", __func__, pid_wait, WIFEXITED(status), WEXITSTATUS(status));

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            return false;
        } else {
            ret = true;
        }

    } else {
        printf("%s: This is child process (pid#%d)\n", __func__, pid);

        // redirect stdout(1) to fd
        if ( dup2(fd, 1) < 0 ) {
            printf("%s: dup2() error!\n", __func__);
            close(fd);
            return false;
        } else {
            close(fd);  // why close(fd) before run execv()?
        }

        reti = execv(command[0], command);
        if ( reti < 0 ) {
            printf("%s: execv(%s, ...) error - returning %d\n", __func__, command[0], reti);
            printf("%s: Errorno: %d (%s)\n", __func__, errno, strerror(errno));
            //printf("%s: The exec() function should have returned false since echo was not specified\n", __func__);
            printf("%s: Ready to exit due to error!\n", __func__);
            exit(EXIT_FAILURE);  // if return -1, parent process will return 0
        } else {
            printf("%s: execv(%s, ...) passed - returning %d\n", __func__, command[0], reti);
            ret = true;
        }
    }

    va_end(args);

    return ret;














}
