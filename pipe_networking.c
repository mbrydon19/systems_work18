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
  char * wk_pipe = "/tmp/wk_pipe";
  printf("creating well-known pipe\n");
  mkfifo(wk_pipe, 0666);
  int fd_wkp = open(wk_pipe, O_RDONLY);
  // read from well-known pipe
  char * pp = malloc(10);
  int bytes_read = read(fd_wkp, pp, 100);
  // connect to private pipe
  char p_pipe[20] = "/tmp";
  strcat(p_pipe, pp);
  int fd_pp = open(p_pipe, O_WRONLY);
  // print message recieved + send message
  if(bytes_read){
    printf("from client: [%s]\n", pp);
    if(strcmp(pp, ACK)){
      write(fd_pp, pp, 10);
    }
  }
  // close pipes
  close(fd_pp);
  close(fd_wkp);
  to_client = &fd_pp;
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
  // PID DOES NOT TRANSLATE PROPERLY FROM CHAR TO INT
  // FIX SO PROPER PID IS SENT TO SERVER
  char pid = getpid();
  char p_pipe[20] = "/tmp";
  strcat(p_pipe, &pid);
//  printf("%c\n", pid);
  printf("creating private pipe\n");
  mkfifo(p_pipe, 0666);
  int fd_pp = open(p_pipe, O_RDONLY);
  // open well-known pipe
  char * wk_pipe = "/tmp/wk_pipe";
  int fd_wkp = open(wk_pipe, O_WRONLY);
  // write pid to well-known pipe
  int bytes_written = write(fd_wkp, &pid, 10);
  char pp[10];
  int bytes_read = read(fd_pp, pp, 10);
  if(bytes_read){
    printf("from server: [%s]\n", pp);
    write(fd_wkp, ACK, 10);
  }
  // close pipes
  close(fd_pp);
  close(fd_wkp);
  to_server = &fd_wkp;
  return fd_pp;
}
