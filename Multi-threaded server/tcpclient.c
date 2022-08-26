#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_LINE 40

int main (int argc, char *argv[]) {
  char* host_addr = argv[1];
  int port = atoi(argv[2]);
  char* buf = argv[3];
  /* Open a socket */
  int s;
  if((s = socket(PF_INET, SOCK_STREAM, 0)) <0){
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

  /* Connect to the server */
  if(connect(s, (struct sockaddr *)&sin, sizeof(sin))<0){
    perror("simplex-talk: connect");
    close(s);
    exit(1);
  }

  /*main loop: get and send lines of text */
  char message [MAX_LINE];
  strcpy(message, "HELLO ");
  strcat(message, buf); 

  //get x value
  int x = atoi(buf);

  int len = strlen(message)+1;
  send(s, message, len, 0);

  //calculate the y value from x
  char y[MAX_LINE];


  //receive y from server

  recv(s, y, sizeof(y),0);
  fputs(y, stdout);
  fputc('\n', stdout);
  fflush(stdout);

  //send z to server

  char* token = strtok(y, " ");
  token = strtok(NULL, " ");

  int y_int = atoi(token);
  int z = y_int + 1;
  char z_string[MAX_LINE];
  strcpy(message, "HELLO ");
  sprintf(z_string, "%d", z);
  strcat(message, z_string); 
  len = strlen(message) + 1;
  send(s, message , len, 0);

}
