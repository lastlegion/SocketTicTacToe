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


int players[100];
char games[50][3][3];
char moves[50][9];
int game=0;
int numPlayers=0;
int MAX=100;


void setHints(struct addrinfo *hints)
{
    bzero(hints, sizeof *hints);
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
}

int setUpServer(){
    struct addrinfo hints, *serverinfo;
    setHints(&hints);
    int addrinfo = getaddrinfo(NULL, PORT, &hints, &serverinfo);
    int socket_fd;
    if((socket_fd = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol)) == -1){
        perror("Socket: ");
        exit(0);
    }
    int yes=1;
    if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        perror("setsockopt ");
    }
    if(bind(socket_fd, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1){
        perror("Bind error ");
        exit(1);
    }

    listen(socket_fd, 5);
    cout << "Server listening at port " << PORT << endl;
    return socket_fd;
}

void fillBoard(int gameid){
//    cout << "Filling board" << endl;
    char c='a';
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            games[gameid][i][j] = c+((i*3)+j);
//            cout << games[gameid][i][j] << "\t";
        }
//        cout << endl;
    }
}

void displayBoard(int gameid){
//    cout << "Filling board" << endl;
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
           cout << games[gameid][i][j] << "\t";
        }
        cout << endl;
    }
}


void initializeGames(){
    for(int i=0; i<100; i++){
        players[i] = -1;
    }
    for(int i=0; i<50; i++){
        for(int j=0; j<9; j++){
            moves[i][j] = '0';
        }
    }
}
void initialize(int gameid){
    fillBoard(gameid);
}
int checkIfDone(int id, char turn, char playerMark){
    int move = turn - 'a';
    int I = move/3;
    int J = move%3;
    int gameid = id/2;
//    cout <<"checking if done" << endl;
    games[gameid][I][J] = playerMark;    
//    cout << "displaying board in checkIfDone" << endl;
    for(int i=0; i<3; i++){
        if(games[gameid][i][0] == games[gameid][i][1] && games[gameid][i][2] == games[gameid][i][1] )
        {
//            cout << games[gameid][i][0] << " " << games[gameid][i][1] << " " << games[gameid][i][2] << endl;
//            cout << "row matched" << endl; 
            return true;
        }
    }
    //check cols
    for(int i=0; i<3; i++){
        if(games[gameid][0][i] == games[gameid][1][i] && games[gameid][1][i] == games[gameid][2][i])
            return true;
    }
    //check diagonals
    if(games[gameid][0][0] == games[gameid][1][1] && games[gameid][1][1] == games[gameid][2][2])
        return true;
    if(games[gameid][2][0] == games[gameid][1][1] && games[gameid][1][1] == games[gameid][0][0])
        return true;
    //if(games[gameid][I][0] == )
    return false;
}
int checkTurn(int id, char turn, char playerMark){
    int move = turn -'a';
    int I = move/3;
    int J = move%3;
    int gameid = id/2;
    cout << "checkTurn()" << endl;
    if(games[gameid][I][J] == turn){
        //return true;
    } else {
        //return false;
    }
//    cout << "checking turn checkTurn()" << endl;
    if(checkIfDone(id, turn, playerMark) == true){
        char move_rcvd = 'D';
        //char player_mark = '';
        send(gameid*2, &move_rcvd, sizeof move_rcvd, 0);
        send(gameid*2, &playerMark, sizeof playerMark, 0);
        send((gameid*2)+1, &move_rcvd, sizeof move_rcvd, 0);
        send((gameid*2)+1, &playerMark, sizeof playerMark, 0);

    }
}
//this function is redundant checkIfDone() already makes the move
void makeMove(int id, char playermark, char turn){
    int move = turn -'a';
    int I = move/3;
    int J = move%3;
    int gameid = id/2;
    games[gameid][I][J] = playermark;
}
void processMove(int id, char move_rcv, char playerMark){
    int move = move_rcv - 'a';
    int I = move/3;
    int J = move%3;
    int gameid =id/2;
    games[gameid][I][J]= playerMark;
    cout << "moves\t";
    for(int i=0; i<9; i++){
        if(moves[gameid][i] == '0'){
            moves[gameid][i] = move_rcv;
            break;
        }
        cout << moves[gameid][i] << "\t";
    }
    cout << endl;
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            int flag=false;
            for(int k=0; k<9; k++){
                if(moves[gameid][k] == '0')
                    break;
                if(('a'+(i*3)+j) == moves[gameid][k])
                    flag = true;
            }
            if(flag == true){
                games[gameid][i][j] = games[gameid][i][j];
            } else {
                games[gameid][i][j] = ('a'+(i*3)+j);
            }
        }
    }
    if(checkIfDone(id, move_rcv, playerMark) == true){
        cout << "Game over!" << endl;
        char endMark = 'D';
        //char player_mark = '';
        send(gameid*2, &endMark, sizeof endMark, 0);
        send(gameid*2, &move_rcv, sizeof move_rcv, 0);
        send(gameid*2, &playerMark, sizeof playerMark, 0);
//        string mmsg = "game over(server)";
//        send((gameid*2)+1, mmsg.c_str(), mmsg.length(), 0);
//        send((gameid*2)+1, mmsg.c_str(), mmsg.length(), 0);
        send((gameid*2)+1, &endMark, sizeof endMark, 0);
        send((gameid*2)+1, &move_rcv, sizeof move_rcv, 0);
        send((gameid*2)+1, &playerMark, sizeof playerMark, 0);

   
    }
    //make move
    //games[gameid][I][J] = playerMark;

    //displayBoard(gameid);

    //check if game finished

}
void startGame(int id1, int id2, int gameid){
    string msg;
    int numTurns=0;
    bool done=false;
    int turn=0;
    initialize(gameid);
    displayBoard(gameid);    
    for(int i=0; i<100; i++){
        if(players[i] == -1){
            break;
        }
        cout << i << "\t" << players[i] << endl;
    }

    while(numTurns!=9 || done!=true){
        int id;
        msg = "Make your Turn ";
        char playerMark;
        if(turn ==0){
            playerMark = 'O';
            id = id1;
        } else {
            playerMark = 'X';
            id = id2;
        }       
        if(numTurns == 0){
          msg = msg+ playerMark;
          send(id, msg.c_str(), MAX, 0);
        }
        char move_rcvd;
        recv(id, &move_rcvd, sizeof move_rcvd, 0);
        cout << "Recieved " << move_rcvd << " from " << id << endl;
        processMove(id, move_rcvd, playerMark); 
        /*
        if(checkTurn(id, move_rcvd, playerMark )){
            makeMove(id,playerMark, move_rcvd);     
        }
        */
        send(id, &move_rcvd, sizeof move_rcvd, 0);
        send(id, &playerMark, sizeof playerMark, 0);
        msg = "Make your turn ";
        if(playerMark == 'X'){
          msg+= 'O';
        } else {
          msg+='X';
        }
        if(id != id2){
          send(id2, msg.c_str(), msg.length(), 0);
          send(id2, &move_rcvd, sizeof move_rcvd, 0);
          send(id2, &playerMark, sizeof playerMark, 0);
        } else {
          send(id1, msg.c_str(), msg.length(), 0);
          send(id1, &move_rcvd, sizeof move_rcvd, 0);
          send(id1, &playerMark, sizeof playerMark, 0);

        }
        turn=(turn+1)%2;
        
    }
    msg = "Make your turn";
    send(id1,msg.c_str(), MAX,0);
    char msg_rcvd[100];
    recv(id1, msg_rcvd, 100,0);
    cout << "Recieved " << msg_rcvd << " from " << id1 << endl;
    int a; 
    cin >> a;
}

void clientHandler(int client_fd){
    string msg;
    bool waiting=true;
    if(numPlayers%2 == 1){
        msg = "Waiting for another player";
    } else {
        waiting=false;
        msg = "Joining another player" + client_fd;
        //game=game+1;
    }
    send(client_fd, msg.c_str(),MAX, 0);
    int gameid;
    if(waiting == true){
        while(numPlayers%2 !=0){
            //do nothing
        }
    } else {
        //startGame(client_fd, players[numPlayers-1]);
    }

    gameid=game;
    game++;
    cout << numPlayers << endl;
    cout << "Starting game with " << client_fd << " and " << players[numPlayers-2] << endl;
    startGame(client_fd, players[numPlayers-2], gameid);
    //player 0,1 have gameid 0
}

int main(int argc, char** argv)
{
    int socket_fd = setUpServer();
    initializeGames();
    struct sockaddr_storage client_addr;
    int new_fd;
    while(1){
        socklen_t sin_size = sizeof client_addr;
        new_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &sin_size);
        cout << "New client connected to server" << new_fd <<  endl;
        players[numPlayers] = new_fd;
        numPlayers++;
        if(fork() == 0){
            close(socket_fd);
            clientHandler(new_fd);
            close(new_fd);
            cout << "Player " << new_fd << " disconnected "<<endl;
            numPlayers--;
            exit(0);
        }
        //close(new_fd);
    }
}
