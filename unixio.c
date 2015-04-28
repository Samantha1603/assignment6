#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 256
#define DURATION 30

int timeup = 0;
struct timeval startTime;
struct timeval selectMaxDuration;
struct timeval stdinTimeout;


pid_t pid1, pid2,pid3,pid4, pid5; // create process ids for the forks
FILE *outputFile;

/*
  create 4 pipe child processes
  create 1 stdin child process
  each process may write to its pipe              ###### COMPLETED
  each message will have its own timestamp        ###### COMPLETED
  sleep for random 0-2 seconds                    ###### COMPLETED
  process ends after 30 seconds                   ###### COMPLETED
  this means we want to use a timer handler       ###### COMPLETED

  parent will use select to read                  ###### COMPLETED
  timestamp when it reads                         ###### COMPLETED
  write out the line to output.txt.               ###### COMPLETED

  terminate parent after all child processes are completed.
*/

// Remove signal handler and do a time comparison in each process' loop ###### COMPLETED
// change random generator and seed it with the pid ###### COMPLETED
// have each parent wait on its child to end gracefully ###### COMPLETED




double getElapsedTime(){
  struct timeval now;
  gettimeofday(&now, NULL);
  double secondsElapsed = (now.tv_sec-startTime.tv_sec)*1000 + (now.tv_usec-startTime.tv_usec)/1000; // final result will be in milliseconds
  secondsElapsed = secondsElapsed/1000;
  return secondsElapsed;
}

char *getElapsedTimeString() {
  double secondsElapsed = getElapsedTime();
  char *secElapsed = (char *)calloc(6, sizeof(char));;
  sprintf(secElapsed, "%5.3f", secondsElapsed);
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
  free(timeStamp);
  return newMessage;
}

int main(int argc, char *argv[]){
  outputFile = fopen("output.txt", "w");
  int status;


  int fd1[2], fd2[2],fd3[2],fd4[2],fd5[2];
  int result, nread, resultStdin;
  fd_set inputs, inputfds;  // sets of file descriptors


  char *write_msg1=(char *)calloc(BUFFER_SIZE, sizeof(char));
  char *write_msg2=(char *)calloc(BUFFER_SIZE, sizeof(char));
  char *write_msg3=(char *)calloc(BUFFER_SIZE, sizeof(char));
  char *write_msg4=(char *)calloc(BUFFER_SIZE, sizeof(char));
  char *write_msg5=(char *)calloc(BUFFER_SIZE, sizeof(char));
  char *read_msg=(char *)calloc(BUFFER_SIZE, sizeof(char));
  char *read_msg_stdin=(char *)calloc(BUFFER_SIZE, sizeof(char));
  char *temp_msg_buffer=(char *)calloc(BUFFER_SIZE, sizeof(char));


  if (pipe(fd1) == -1) {
    fprintf(stderr,"pipe() failed\n");
    return 1;
  }
  if (pipe(fd2) == -1) {
    fprintf(stderr,"pipe() failed\n");
    return 1;
  }

  if (pipe(fd3) == -1) {
    fprintf(stderr,"pipe() failed\n");
    return 1;
  }
  if (pipe(fd4) == -1) {
    fprintf(stderr,"pipe() failed\n");
    return 1;
  }
  if (pipe(fd5) == -1){
    fprintf(stderr,"pipe() failed\n");
    return 1;
  }
  FD_ZERO(&inputs);
  FD_SET(fd1[READ_END], &inputs);
  FD_SET(fd2[READ_END], &inputs);
  FD_SET(fd3[READ_END], &inputs);
  FD_SET(fd4[READ_END], &inputs);
  FD_SET(fd5[READ_END], &inputs);
  FD_SET(0, &inputs);

  gettimeofday(&startTime, NULL);
 
  selectMaxDuration.tv_sec = DURATION;

  pid1 = fork();

  if (pid1 > 0) {  // this is the parent
    close(fd1[WRITE_END]); // Close the unused READ end of the pipe.

    while(getElapsedTime() <= DURATION){ // loop while not finished
      inputfds = inputs;      
      result = select(FD_SETSIZE, &inputfds, 
		      NULL, NULL, &selectMaxDuration);
      switch(result){
      case 0: 
	printf("timed out\n");
	break;
      case -1:
	perror("select\n");
	exit(1);
	break;
      default:
	if(FD_ISSET(fd1[READ_END], &inputfds)){ // file descriptor 1
	  ioctl(fd1[READ_END], FIONREAD, &nread);
	  if(nread==0){
	    printf("Nothing to read\n");
	    exit(0);
	  }
	  nread = read(fd1[READ_END], read_msg, nread);
	  read_msg = (char *) insertTimestamp(read_msg);
	  fprintf(outputFile, "Parent: Read '%s' from the pipe.\n", read_msg);
	}
	if(FD_ISSET(fd2[READ_END], &inputfds)){ // file descriptor 2
	  ioctl(fd2[READ_END], FIONREAD, &nread);
	  if(nread==0){
	    printf("Nothing to read\n");
	    exit(0);
	  }
	  nread = read(fd2[READ_END], read_msg, nread);
	  read_msg = (char *) insertTimestamp(read_msg);  
	  fprintf(outputFile, "Parent: Read '%s' from the pipe.\n", read_msg);
	}
	if(FD_ISSET(fd3[READ_END], &inputfds)){ // file descriptor 3
	  ioctl(fd3[READ_END], FIONREAD, &nread);
	  if(nread==0){
	    printf("Nothing to read\n");
	    exit(0);
	  }
	  nread = read(fd3[READ_END], read_msg, nread);
	  read_msg = (char *) insertTimestamp(read_msg);
	  fprintf(outputFile, "Parent: Read '%s' from the pipe.\n", read_msg);
	}
	if(FD_ISSET(fd4[READ_END], &inputfds)){ // file descriptor 4
	  ioctl(fd4[READ_END], FIONREAD, &nread);
	  if(nread==0){
	    printf("Nothing to read\n");
	    exit(0);
	  }
	  nread = read(fd4[READ_END], read_msg, nread);
	  read_msg = (char *) insertTimestamp(read_msg);
	  fprintf(outputFile, "Parent: Read '%s' from the pipe.\n", read_msg);
	}

	if(FD_ISSET(fd5[READ_END], &inputfds)){ // file descriptor stdin
	  ioctl(fd5[READ_END], FIONREAD, &nread);
	  if(nread==0){
	    printf("Nothing to read\n");
	    exit(0);
	  }
	  nread = read(fd5[READ_END], read_msg, nread);
	  read_msg = (char *) insertTimestamp(read_msg);  
	  fprintf(outputFile, "Parent: Read '%s' from the pipe.\n", read_msg);
	}

      }// end switch-case stmt
      //output to file
    }
    wait(&status);
    fclose(outputFile);
  }
  else{ // child
    // fork again
    pid2 = fork();
    srand(pid2); 
    if(pid2>0){ // child - parent
      while(getElapsedTime() <= DURATION){ // loop while not finished
	close(fd1[READ_END]);
	write_msg1 = "Message from child 1";
	write_msg1 = (char *) insertTimestamp(write_msg1);
	sleep(rand() % 3);
	write(fd1[WRITE_END], write_msg1, strlen(write_msg1)+1);
      }
      wait(&status);
      exit(0);
    }
    else{ // child - child
      pid3 = fork();
      srand(pid3); 
      if(pid3>0){ // child - child - parent
	while(getElapsedTime() <= DURATION){ // loop while not finished
	  close(fd2[READ_END]);
	  write_msg2 = "Message from child 2";
	  write_msg2 = (char *) insertTimestamp(write_msg2);
	  sleep(rand() % 3);
	  write(fd2[WRITE_END], write_msg2, strlen(write_msg2)+1);
	}
	wait(&status);
	exit(0);
      }
      else{ // child - child - child
        pid4 = fork();
        srand(pid4); 
        if(pid4>0){ // child - child - child - parent
	  while(getElapsedTime() <= DURATION){ // loop while not finished
	    close(fd3[READ_END]);
	    write_msg3 = "Message from child 3";
	    write_msg3 = (char *) insertTimestamp(write_msg3);
	    sleep(rand() % 3);
	    write(fd3[WRITE_END], write_msg3, strlen(write_msg3)+1);
	  }
	  wait(&status);
	  exit(0);
	}
	else{ // child - child - child - child
	  pid5 = fork();
	  srand(pid5);   
	  if(pid5>0){
	    while(getElapsedTime() <= DURATION){ // loop while not finished
	      close(fd4[READ_END]);
	      write_msg4 = "Message from child 4";
	      write_msg4 = (char *) insertTimestamp(write_msg4);
	      sleep(rand() % 3);
	      write(fd4[WRITE_END], write_msg4, strlen(write_msg4)+1);
	    }
	    wait(&status);
	    exit(0);
	  }
	  else{ // child - child - child - child - child
	    srand((unsigned) time(0));
	    while(getElapsedTime() <= DURATION){ // loop while not finished
	      inputfds = inputs;
	      stdinTimeout.tv_sec = rand() % 3;
	      resultStdin = select(FD_SETSIZE, &inputfds,
				   NULL, NULL, &stdinTimeout);
	      switch(resultStdin) {
	      case 0: {
		printf("Please type your message\n");
		fflush(stdout);
		break;
	      }
	      case -1: {
		printf("ERRORED OUT");
		perror("select");
		exit(1);
	      }
	      default: {
		if (FD_ISSET(0, &inputfds)) {
		  ioctl(0,FIONREAD,&nread);
                    
		  if(nread > 0){
		    memset(read_msg_stdin, 0, BUFFER_SIZE);
		    nread = read(0,read_msg_stdin,nread);
		    memset(temp_msg_buffer, 0, BUFFER_SIZE);
		    write_msg5 = "Message from stdin:";
		    int write_msg5_len = strlen(write_msg5);
		    strncat(temp_msg_buffer, write_msg5, strlen(write_msg5)+1);
		    int read_msg_stdin_len = strlen(read_msg_stdin);
		    strncat(temp_msg_buffer, read_msg_stdin, strlen(read_msg_stdin)+1);
		    temp_msg_buffer[write_msg5_len + read_msg_stdin_len-1] = '\0';
		    write_msg5 = (char *) insertTimestamp(temp_msg_buffer);
		    write(fd5[WRITE_END], write_msg5, strlen(write_msg5)+1);
		  }
		}
	      }
	      }
	    }
	    exit(0);
	  }
	}
      }
    }
  }
  printf("#############################\nProgram completed\n");
  return 0;
}
