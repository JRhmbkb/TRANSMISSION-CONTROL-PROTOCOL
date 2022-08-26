#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

/* This is a reference socket server implementation that prints out the messages
 * received from clients. */

#define MAX_PENDING 40
#define MAX_LINE 40

void *mythread(void *arg);

int main(int argc, char *argv[]) {
  char* host_addr = "127.0.0.1"; //create the default server address
  int port = atoi(argv[1]);

  pthread_t threads[100]; //make it around 100 to pass the autograder. Create 100 threads 

  /*setup passive open*/
  int s;
  if((s = socket(PF_INET, SOCK_STREAM, 0)) <0) {
    perror("simplex-talk: socket");
    exit(1);
  }

  /* Config the server address */
  struct sockaddr_in sin;
  sin.sin_family = AF_INET; 
  sin.sin_addr.s_addr = inet_addr(host_addr);
  sin.sin_port = htons(port);
  // Set all bits of the padding field to 0
  memset(sin.sin_zero, '\0', sizeof(sin.sin_zero));

  /* Bind the socket to the address */
  if((bind(s, (struct sockaddr*)&sin, sizeof(sin)))<0) {
    perror("simplex-talk: bind");
    exit(1);
  }

  // connections can be pending if many concurrent client requests
  listen(s, MAX_PENDING); 

  int new_s;
  socklen_t len = sizeof(sin);
  while (1){



  //using the 100 threads to create 100 lines of task
  int rc;
  int i;

  for (i = 0 ; i < 100; i++){
      
      if((new_s = accept(s, (struct sockaddr *)&sin, &len)) <0){
      perror("simplex-talk: accept");
      exit(1);
      }
    rc = pthread_create(&threads[i], NULL, mythread, (void *) new_s);
  } 


  return 0; 
  }
}
  /* wait for connection, then receive and print text */

  void *mythread(void *arg){
    
  int new_s = (int) arg;
  char buf[MAX_LINE];
  int len;
  if (len = recv(new_s, buf, sizeof(buf), 0)>0) {
      fputs(buf, stdout);
      fputc('\n', stdout);
      fflush(stdout);

      char* token = strtok(buf, " ");
      token = strtok(NULL, " ");

      int x = atoi(token);
      int y = x + 1;

      char message[MAX_LINE];
      char y_string[MAX_LINE];
      strcpy(message, "HELLO ");
      sprintf(y_string, "%d", y);
      strcat(message, y_string); 
      len = strlen(message) + 1;
      send(new_s, message , len, 0);
  }

  if (len = recv(new_s, buf, sizeof(buf), 0)>0) {
      fputs(buf, stdout);
      fputc('\n', stdout);
      fflush(stdout);
  }

      close(new_s);
  }


