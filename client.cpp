#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>

#define PORT "1337"

using namespace std;

void setHints(struct addrinfo* hints){
    bzero(hints, sizeof *hints);
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;

}

int connectServer(char* serveraddr){
    struct addrinfo hints, *serverinfo; 
    setHints(&hints);
    int rv = getaddrinfo(serveraddr, PORT, &hints, &serverinfo);
    int socket_fd = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);
    connect(socket_fd, serverinfo->ai_addr, serverinfo->ai_addrlen);
    cout << "Connected to server" << endl;
    return socket_fd;

}
char board[3][3];
void fillBoard(){
    char c='a';
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            board[i][j] = c+((i*3)+j);
        }
    }
}
void displayBoard(){
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            cout << board[i][j] << "\t";
        }
        cout << endl;
    }
}
void updateBoard(char playerMark, char turn){
    int move = turn - 'a';
    int I = move/3;
    int J = move%3;
    board[I][J] = playerMark;
}
int main(int argc, char** argv){
    char* serveraddr = argv[1];
    int socket_fd = connectServer(serveraddr);
    int numbytes;
    char buff[100];
    fillBoard();
    displayBoard();
    if(fork() == 0){
        //waiting for messages from server
        while(1){
            //Make move message
            numbytes = recv(socket_fd, buff, sizeof buff, 0);
            cout << endl << "Server: " << buff << endl;
                
            //Recieve own move
            char move_rcv;
            recv(socket_fd, &move_rcv, sizeof move_rcv, 0);

            char playerMark_rcv;
            recv(socket_fd, &playerMark_rcv, sizeof playerMark_rcv, 0);
            updateBoard(playerMark_rcv, move_rcv);
            if(move_rcv == 'D'){
                cout << "Game over :D" << endl;
                cout << playerMark_rcv << " won!" << endl;
                close(socket_fd);
                exit(0);
            }
            
            cout << playerMark_rcv << "'s move: " << endl;
            displayBoard();
            cout << endl << endl;
            
            //Recieve opponent's move
            recv(socket_fd, &move_rcv, sizeof move_rcv, 0);
            recv(socket_fd, &playerMark_rcv, sizeof playerMark_rcv, 0);
            updateBoard(playerMark_rcv, move_rcv);
            cout << playerMark_rcv << "'s move" << endl;
            displayBoard();
        }
       
    }
    //Sending messages to server 
    char move;
    while(1){
        cin >> move;
        send(socket_fd, &move, sizeof move, 0);
    } 

}
