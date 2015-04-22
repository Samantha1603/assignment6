#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 64
#define DURATION 30

int timeup = 0;
  // create 5 child processes, start with 2 for now.
  // each process may write to its pipe
  // each message will have its own timestamp
  // sleep for random 0-2 seconds
  // process ends after 30 seconds
  // this means we want to use a timer handler

  // parent will use select to read
  // timestamp when it reads
  // write out the line to output.txt.

  //terminate parent after all child processes are completed.

int main(int argc, char *argv[]){
  // create 5 pipes
  // pid_t pid1, pid2, pid3, pid4, pid5;
  pid_t pid1;
  pid_t pid2;

  int fd1[2];
  int fd2[2];
  int result, nread;
  fd_set inputs, inputfds;  // sets of file descriptors

  FD_ZERO(&inputs);
  FD_SET(fd1[WRITE_END], &inputs);
  FD_SET(fd2[WRITE_END], &inputs);

  char write_msg[BUFFER_SIZE];
  char read_msg[BUFFER_SIZE];


  if (pipe(fd1) == -1) {
    fprintf(stderr,"pipe() failed");
    return 1;
  }
  if (pipe(fd2) == -1) {
    fprintf(stderr,"pipe() failed");
    return 1;
  }
  pid1 = fork();
  if (pid1 > 0) {  // this is the parent
    // Close the unused READ end of the pipe.
    close(fd1[WRITE_END]);
    // loop while not finished
    // Read from READ end of the pipe.
    result = select(FD_SETSIZE, &inputfds, 
		    (fd_set *) 0, (fd_set *) 0, &timeout);
    //check which ones are here and print message
    //  within each check, when the message is received 
    //  generate timestamp and add it to message
    
    //output to file
    printf("Parent: Read '%s' from the pipe.\n", read_msg);
  }
  else if (pid == 0){
    close(fd1[READ_END]);
    // loop while not finished
    // generate a random value for sleeping
    // generate write message
    // get timestamp
    write(fd[WRITE_END], write_msg, strlen(write_msg)+1);
  }

  // join all threads to parent.
  // kill(pid_t pid, SIGKILL)

  

  return 0;
}
