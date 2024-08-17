/* Writer.c for assignment 2 */
/* 3. Write a C application “writer” (finder-app/writer.c)  
   which can be used as an alternative to the “writer.sh” test script 
   created in assignment1 and using File IO as described in LSP chapter 2. 
** 1) See the Assignment 1 requirements for the writer.sh test script 
   and these additional instructions:
** 2) One difference from the write.sh instructions in Assignment 1:  
   You do not need to make your "writer" utility create directories 
   which do not exist.  
** 3) You can assume the directory is created by the caller.
** 4) Setup syslog logging for your utility using the LOG_USER facility.
** 5) Use the syslog capability to write a message 
   "Writing <string> to <file>"
   where <string> is the text string written to file (second argument)
   and <file> is the file created by the script.
** 6) This should be written with LOG_DEBUG level.
** 7) Use the syslog capability to log any unexpected errors 
   with LOG_ERR level.
*/

/* Writer.sh for assignment 1 
** 8) Accepts the following arguments: 
   the first argument is a full path to a file (including filename) 
   on the filesystem, referred to below as writefile; 
   the second argument is a text string which will be written 
   within this file,
** 9) referred to below as writestr
** 10) Requirement (1): Exits with return value 1 error and 
   print statements if any of the parameters were not specified
** 11) Requirement (2): Creates a new file with name and path writefile
   with content writestr, overwriting any existing file and
   creating the path if it doesnt exist. -> overruled by 2-3)
** 12) Requirement (3): Exits with value 1 and error print statement if
   the file could not be created.
*/
#include <stdio.h>  // printf()
#include <syslog.h>  // syslog()
#include <fcntl.h>  // open()
#include <errno.h>
#include <unistd.h> // write()
#include <string.h>  // strerror()





int main(int argc, char **argv)
{
    //int rc=-1;
    int rc=0;
/* 4) Setup syslog logging for your utility using the LOG_USER facility.
** 7) Use the syslog capability to log any unexpected errors 
   with LOG_ERR level. */
    openlog(NULL,0,LOG_USER);

/* 8) Accepts the following arguments: 
   the first argument is a full path to a file (including filename) 
   on the filesystem, referred to below as writefile; 
   the second argument is a text string which will be written 
   within this file, */
    if( argc != 3 ) {
        //printf("Usage: %s filename string\n",argv[0]);
        //printf("    write a file \"filename\" with a string \"string\"\n");
        syslog(LOG_ERR,"Invalid Number of arguments: %d",argc);
        return 1;
    } 
    else {
/* 9) referred to below as writestr
** 10) Requirement (1): Exits with return value 1 error and 
   print statements if any of the parameters were not specified
** 11) Requirement (2): Creates a new file with name and path writefile 
   with content writestr, overwriting any existing file and 
   creating the path if it doesn’t exist. -> overruled by 2-3)
** 12) Requirement (3): Exits with value 1 and error print statement if 
   the file could not be created.
** 2) One difference from the write.sh instructions in Assignment 1:  
   You do not need to make your "writer" utility create directories 
   which do not exist.  
** 3) You can assume the directory is created by the caller.*/
        const char *filename=argv[1];
        const char *string=argv[2];
        int fd=open(filename,
                O_CREAT|O_RDWR,
                S_IRWXU|S_IRWXG|S_IRWXO);
        if( fd == -1 ) {
            //printf("Error %d (%s) opening %s\n",errno,strerror(errno),filename);
            syslog(LOG_ERR,"Error %d (%s) opening %s\n",errno,strerror(errno),filename);
            return 1;
        }
        else {
/* 5) Use the syslog capability to write a message 
   "Writing <string> to <file>"
   where <string> is the text string written to file (second argument)
   and <file> is the file created by the script.
** 6) This should be written with LOG_DEBUG level.*/

            ssize_t bytes_written = write(fd,string,strlen(string));
            size_t string_length = strlen(string);

            //printf("string_length = %ld\n",string_length);
            //printf("bytes_written = %ld\n",bytes_written);
            //printf("Writing %s to %s \n",string,filename);
            syslog(LOG_DEBUG,"Writing %s to %s \n",string,filename);

        }
    }
    return rc;
}











