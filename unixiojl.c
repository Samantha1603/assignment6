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

#define READ_END_OF_PIPE 0
#define WRITE_END_OF_PIPE 1

int main(int argc, char *argv[]){
  // create 5 pipes
  pid_t pid;
  int pipe_fd1[2];
  pid_t pid;
  int pipe_fd2[2];

  if(pipe(pipe_fd1) == -1){
    fprintf(stderr, "pipe() failed");
  }
  if(pipe(pipe_fd2) == -1){
    fprintf(stderr, "pipe() failed");
  }
  
  
  // create 5 child processes, start with 1 for now.
  // each process may write to its pipe
  // each message will have its own timestamp
  // sleep for random 0-2 seconds
  // process ends after 30 seconds
  // this means we want to use a timer handler

  // parent will use select to read
  // timestamp when it reads
  // write out the line to output.txt.

  //terminate parent after all child processes are completed.

  return 0;
}
