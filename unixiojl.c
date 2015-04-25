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
#define BUFFER_SIZE 128
#define DURATION 10

int timeup = 0;
struct itimerval sessionTimer;
struct timeval startTime;

pid_t pid1,pid2; // create process ids for the forks
FILE *outputFile;

/*
  create 5 child processes, start with 1 for now. ###### COMPLETED
  each process may write to its pipe              ###### COMPLETED
  each message will have its own timestamp        ###### COMPLETED
  sleep for random 0-2 seconds                    ###### COMPLETED
  process ends after 30 seconds                   ###### COMPLETED
  this means we want to use a timer handler       ###### COMPLETED

  parent will use select to read                  ###### COMPLETED
  timestamp when it reads                         ###### COMPLETED
  write out the line to output.txt.

  terminate parent after all child processes are completed.
*/

// Remove signal handler and do a time comparison in each process' loop
// change random generator and seed it with the pid
// have each parent wait on its child to end gracefully








char *getElapsedTimeString() {
  // TODO: Convert time into a timeval structure.
  struct timeval now;
  gettimeofday(&now, NULL);
  double secondsElapsed = (now.tv_sec-startTime.tv_sec)*1000 + (now.tv_usec-startTime.tv_usec)/1000;
  secondsElapsed = secondsElapsed/1000;
  
  char *secElapsed = (char *)calloc(6, sizeof(char));;
  int no_char = sprintf(secElapsed, "%5.3f", secondsElapsed);
  printf("number of characters copied: %d\n", no_char);
  return secElapsed;
}
char *insertTimestamp(char *message){
  if(message == NULL){
    return NULL;
  }
  int messageLength = strlen(message);
  char *timeStamp = getElapsedTimeString();
  int timeStampLength = strlen(timeStamp);
  char *separator = " | ";
  int totalLength = messageLength + timeStampLength + 3;
  char *newMessage = (char *)calloc(totalLength+1, sizeof(char));
  strncat(newMessage, timeStamp, timeStampLength);
  strncat(newMessage, separator, 3);
  strncat(newMessage, message, messageLength);
  return newMessage;
}

int main(int argc, char *argv[]){
  outputFile = fopen("io_output.txt", "w");


  int fd1[2], fd2[2];
  int result, nread;
  fd_set inputs, inputfds;  // sets of file descriptors

  char *write_msg1=(char *)calloc(BUFFER_SIZE, sizeof(char));
  char *write_msg2=(char *)calloc(BUFFER_SIZE, sizeof(char));
  char *read_msg=(char *)calloc(BUFFER_SIZE, sizeof(char));

  srand((unsigned) time(0));

  if (pipe(fd1) == -1) {
    fprintf(stderr,"pipe() failed\n");
    return 1;
  }
  if (pipe(fd2) == -1) {
    fprintf(stderr,"pipe() failed\n");
    return 1;
  }

  FD_ZERO(&inputs);
  FD_SET(fd1[READ_END], &inputs);
  FD_SET(fd2[READ_END], &inputs);
  signal(SIGALRM, SIGALRM_handler);
<<<<<<< HEAD
  //time(&startTime);
  gettimeofday(&startTime, NULL);
 
 sessionTimer.it_value.tv_sec = DURATION;
 setitimer(ITIMER_REAL, &sessionTimer, NULL);
=======
  time(&startTime);


  sessionTimer.it_value.tv_sec = DURATION;
  setitimer(ITIMER_REAL, &sessionTimer, NULL);
>>>>>>> 6df3d4a65ffbeb9872374f79865a6c62c6f1da5c
  printf("Forking now\n");
  pid1 = fork();
  if (pid1 > 0) {  // this is the parent
    close(fd1[WRITE_END]); // Close the unused READ end of the pipe.

    while(!timeup){ // loop while not finished
      printf("timeup is %d\n", timeup);
      inputfds = inputs;      
      printf("waiting for results\n");
      result = select(FD_SETSIZE, &inputfds, 
              NULL, NULL, NULL);
      //printf("number of results are %d\n", result);
      switch(result){
      case 0: 
    printf("timedout\n");
    break;

      case -1:
    perror("select\n");
    exit(1);
    break;

      default:
<<<<<<< HEAD
    printf("in default case\n");

    if(FD_ISSET(fd1[READ_END], &inputfds)){ // file descriptor 1
      ioctl(fd1[READ_END], FIONREAD, &nread);
      if(nread==0){
        printf("Nothing to read\n");
        exit(0);
      }
      nread = read(fd1[READ_END], read_msg, nread);
      read_msg = (char *) insertTimestamp(read_msg);
      printf("Parent: Read '%s' from the pipe.\n", read_msg);
    }
    if(FD_ISSET(fd2[READ_END], &inputfds)){ // file descriptor 2
      ioctl(fd2[READ_END], FIONREAD, &nread);
      if(nread==0){
        printf("Nothing to read\n");
        exit(0);
      }
      nread = read(fd2[READ_END], read_msg, nread);
      read_msg = (char *) insertTimestamp(read_msg);  
      printf("Parent: Read '%s' from the pipe.\n", read_msg);
    }
=======
	printf("in default case\n");
	if(FD_ISSET(fd1[READ_END], &inputfds)){ // file descriptor 1
	  ioctl(fd1[READ_END], FIONREAD, &nread);
	  if(nread==0){
	    printf("Nothing to read\n");
	    exit(0);
	  }
	  nread = read(fd1[READ_END], read_msg, nread);
	  read_msg = (char *) insertTimestamp(read_msg);
	  fprintf(outputFile, "Parent read: %s\n", read_msg);
	  fflush(outputFile);
	  printf("Parent: Read '%s' from the pipe.\n", read_msg);
	}
	if(FD_ISSET(fd2[READ_END], &inputfds)){ // file descriptor 2
	  ioctl(fd2[READ_END], FIONREAD, &nread);
	  if(nread==0){
	    printf("Nothing to read\n");
	    exit(0);
	  }
	  nread = read(fd2[READ_END], read_msg, nread);
	  read_msg = (char *) insertTimestamp(read_msg);
	  fprintf(outputFile, "Parent read: %s\n", read_msg);
	  fflush(outputFile);
	  printf("Parent: Read '%s' from the pipe.\n", read_msg);
	}
>>>>>>> 6df3d4a65ffbeb9872374f79865a6c62c6f1da5c
      }// end switch-case stmt
      //output to file
    }
    fclose(outputFile);
  }
  else{
    // fork again
    pid2 = fork();
    if(pid2>0){ // child - parent
      while(!timeup){
<<<<<<< HEAD
    printf("timeup is %d", timeup);
    printf("in child process %d\n", pid1);
    close(fd1[READ_END]);
    sleep(rand() % 3);
    write_msg1 = "Message from child 1";
    write_msg1 = (char *) insertTimestamp(write_msg1);
    int nwrote;
    nwrote = write(fd1[WRITE_END], write_msg1, strlen(write_msg1)+1);
    printf("sent my message with %d bytes\n", nwrote);
=======
	printf("timeup is %d\n", timeup);
	printf("in child process %d\n", pid1);
	close(fd1[READ_END]);
	sleep(rand() % 3);
	write_msg1 = "Message from child 1";
	write_msg1 = (char *) insertTimestamp(write_msg1);
	int nwrote;
	nwrote = write(fd1[WRITE_END], write_msg1, strlen(write_msg1)+1);
	printf("sent my message with %d bytes\n", nwrote);
>>>>>>> 6df3d4a65ffbeb9872374f79865a6c62c6f1da5c
      }

    }
    else{ // child - child
      
      while(!timeup){
<<<<<<< HEAD
    printf("timeup is %d", timeup);
    printf("in child process %d\n", pid2);
    close(fd1[READ_END]);
    sleep(rand() % 3);
    write_msg2 = "Message from child 2";
    write_msg2 = (char *) insertTimestamp(write_msg2);
    int nwrote;
    nwrote = write(fd2[WRITE_END], write_msg2, strlen(write_msg2)+1);
    printf("sent my message with %d bytes\n", nwrote);
=======
	printf("timeup is %d\n", timeup);
	printf("in child process %d\n", pid2);
	close(fd1[READ_END]);
	sleep(rand() % 3);
	write_msg2 = "Message from child 2";
	write_msg2 = (char *) insertTimestamp(write_msg2);
	int nwrote;
	nwrote = write(fd2[WRITE_END], write_msg2, strlen(write_msg2)+1);
	printf("sent my message with %d bytes\n", nwrote);
>>>>>>> 6df3d4a65ffbeb9872374f79865a6c62c6f1da5c
      }
    }
  }
  printf("\n\n\n\n\n\n\n\n\nProgram completed");
  return 0;
}
