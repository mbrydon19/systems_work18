#include "pipe_networking.h"


/*=========================
server_handshake
args: int * to_client

Performs the server side pipe 3 way handshake.
Sets *to_client to the file descriptor to the downstream pipe.

returns the file descriptor for the upstream pipe.
=========================*/
int server_handshake(int *to_client) {
  // create and open well-known pipe
  char * wk_pipe = "wk_pipe";
  printf("creating well-known pipe\n");
  mkfifo(wk_pipe, 0666);
  int fd_wkp = open(wk_pipe, O_RDONLY);
  // read from well-known pipe
  char * pp = malloc(10);
  int bytes_read = read(fd_wkp, pp, 100);
  // print message recieved + return the message
  if(bytes_read){
    printf("from client: [%s]\n", pp);
    int fd_pp = open(pp, O_WRONLY);
    to_client = &fd_pp;
    write(fd_pp, pp, 10);
  }
  // remove well-known pipe
  printf("removing well-known pipe\n");
  remove(wk_pipe);
  // print confirmation message from client
  char * msg = malloc(10);
  int msg_read = read(fd_wkp, msg, 100);
  if(msg_read){
    printf("from client: [%s]\n", msg);
  }
  return fd_wkp;
}


/*=========================
client_handshake
args: int * to_server

Performs the client side pipe 3 way handshake.
Sets *to_server to the file descriptor for the upstream pipe.

returns the file descriptor for the downstream pipe.
=========================*/
int client_handshake(int *to_server) {
  // create and open private pipe
  char * p_pipe = malloc(10);
  sprintf(p_pipe, "%d", getpid());
  printf("creating private pipe\n");
  mkfifo(p_pipe, 0666);
  // open well-known pipe
  char * wk_pipe = "wk_pipe";
  int fd_wkp = open(wk_pipe, O_WRONLY);
  // write pid to well-known pipe
  int bytes_written = write(fd_wkp, p_pipe, 10);
  // open private pipe and print message recieved
  int fd_pp = open(p_pipe, O_RDONLY);
  char pp[10];
  int bytes_read = read(fd_pp, pp, 10);
  if(bytes_read){
    printf("from server: [%s]\n", pp);
    printf("removing private pipe\n");
    remove(p_pipe);
    write(fd_wkp, ACK, 10); // write confirmation message to server
  }
  to_server = &fd_wkp;
  return fd_pp;
}
