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

  int fd1[2];
  int result, nread;
  fd_set inputs, inputfds;  // sets of file descriptors

  FD_ZERO(&inputs);

  char *write_msg1;
  char read_msg1[BUFFER_SIZE];
  struct timeval timeout;
  timeout.tv_sec = 2;

  if (pipe(fd1) == -1) {
    fprintf(stderr,"pipe() failed\n");
    return 1;
  }
  FD_SET(fd1[READ_END], &inputs);
  inputfds = inputs;

  printf("Forking now\n");
  pid1 = fork();
  if (pid1 > 0) {  // this is the parent
    // Close the unused READ end of the pipe.
    close(fd1[WRITE_END]);
    // loop while not finished
    // Read from READ end of the pipe.
    printf("waiting for results\n");
    result = select(FD_SETSIZE, &inputfds, 
		    (fd_set *) 0, (fd_set *) 0, (struct timeval *) &timeout);
    printf("number of results are %d\n", result);
    //check which ones are here and print message
    //  within each check, when the message is received 
    //  generate timestamp and add it to message
    switch(result){
    case 0: 
      printf("timedout\n");
      break;

    case -1:
      perror("select\n");
      exit(1);
      break;

    default:
      printf("in default case\n");
      if(FD_ISSET(fd1[READ_END], &inputfds)){
	printf("FD_ISSET is true\n");
	ioctl(fd1[READ_END], FIONREAD, &nread);
	printf("found something in inputfds set\n");
	if(nread==0){
	  printf("Nothing to read\n");
	  exit(0);
	}
	
	nread = read(fd1[READ_END], read_msg1, nread);
	printf("number of bytes read %d\n", nread);
      }
    }
    //output to file
    printf("Parent: Read '%s' from the pipe.\n", read_msg1);
  }
  else{
    printf("in child process %d\n", pid1);
    close(fd1[READ_END]);
    // loop while not finished
    // generate a random value for sleeping
    // generate write message
    // get timestamp
    write_msg1 = "i will send this as a test\n";
    printf("wrote my message\n");
    int nwrote;
    nwrote =write(fd1[WRITE_END], write_msg1, strlen(write_msg1)+1);
    printf("sent my message with %d bytes\n", nwrote);
  }

  // join all threads to parent.
  // kill(pid_t pid, SIGKILL)

  

  return 0;
}
