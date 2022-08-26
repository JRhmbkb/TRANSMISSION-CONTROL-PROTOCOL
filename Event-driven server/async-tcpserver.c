#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h> 

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

/* This is a reference socket server implementation that prints out the messages
 * received from clients. */

#define MAX_PENDING 40
#define MAX_LINE 40
#define MAX_CLIENT 100


//first handshake handler
void handle_first_shake(int new_s){
  int len;
  char buf[MAX_LINE];
  if (len = recv(new_s, buf, sizeof(buf), 0)>0){
      
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
}

void handle_second_shake (int new_s){
  int len;
  char buf[MAX_LINE];
  if (len = recv(new_s, buf, sizeof(buf), 0)>0){
  fputs(buf, stdout);
  fputc('\n', stdout);
  fflush(stdout);
  }
}


int main(int argc, char *argv[]) {
  char* host_addr = "127.0.0.1"; //create the default server address
  int port = atoi(argv[1]);


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

  //create and initialize the struct of client state
  struct ClientState {
    int fd ;
    int shake;
    int val;
  } client_state;
  
  //initialize the ClientState as all -1  
  struct ClientState ClientStateArray[MAX_CLIENT];

  //iterate through ClientStateArray to initialize all
  for (int i = 0; i < MAX_CLIENT; i++){
    struct ClientState current = {-1, -1, -1};
    ClientStateArray[i] = current;
  }

  //create the master fd set
  fd_set master_Set;
  fd_set ready_set;
  //set fd_max as s 
  int fd_max = s; 
  //fcntl(s, F_SETFL, O_NONBLOCK);
  fcntl(s, NULL, 0);

  FD_ZERO(&master_Set);
  FD_SET(s, &master_Set);


  while (1){
  
  //Initialize the ready_set
  FD_ZERO(&ready_set);

  //copy the master_set to ready_set  
  ready_set = master_Set;

  //get all the file discriptors that are ready to read
  select(fd_max + 1, &ready_set, NULL, NULL, NULL);

  for (int i = 0; i <= fd_max; i++){
    if (FD_ISSET(i, &ready_set)){
      
      if (i==s){
        int new_s = accept(s, (struct sockaddr *)&sin, &len);

        fcntl(new_s, NULL, 0);
        FD_SET(new_s, &master_Set);


        //loop through the whole ClientStateArray and find the first avaiable empty element
        for (int k = 0; k < MAX_CLIENT; k++){
          struct ClientState current = ClientStateArray[k];

          if (current.fd == -1){
            struct ClientState accepted_client = {.fd = new_s, .shake = 1, .val = s};
            ClientStateArray[k] = accepted_client;
            break;
          }
        }

        //if new_s > fd_max, update fd_max as new_s
        if (new_s > fd_max){
          fd_max = new_s;
        }


      }
      else{
        //if i != s, iterate through the whole ClientStateArray to find which one's fd == i 

        for (int k = 0; k < MAX_CLIENT; k++){
              
            if (ClientStateArray[k].shake == 1 && ClientStateArray[k].fd == i){
            //Handle_fist_hand shake
            handle_first_shake(i);

            //change the corresponding shake state to 2, 2nd handshake
            ClientStateArray[k].shake = 2;
            }

            else if (ClientStateArray[k].shake == 2 && ClientStateArray[k].fd == i){
              //Handle_second_hand shake
              handle_second_shake(i);
              //close
              close(i);
              //remove i from masterset 
              FD_CLR(i, &master_Set);
              //Clear/re-initialize struct entry for client_status_array[i]
              ClientStateArray[k].fd = -1;
              ClientStateArray[k].shake = -1;
              ClientStateArray[k].val = -1;
            }
     }
  }
}
}
}
}

