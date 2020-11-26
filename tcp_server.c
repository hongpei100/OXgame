#if defined(_WIN32)
#error This program does not support Windows.
#endif

#include "web.h"
#include <ctype.h>
#include <stdlib.h>

struct user_space{
        char account[100];
        char password[100];
        int id;
	int login;
	int socket;
	char name_id[10];
}user[5];
//struct user_space user[5];
/*
strcpy(buf, "AAAA");
strcpy(user[0].account,"Ian"); strcpy(user[0].password,"1104"); user[0].id = 1;
strcpy(user[1].account,"LTN"); strcpy(user[1].password,"0917"); user[1].id = 2;
strcpy(user[2].account,"annie"); strcpy(user[2].password,"1209"); user[2].id = 3;
strcpy(user[3].account,"cindy"); strcpy(user[3].password,"0214"); user[3].id = 4;
strcpy(user[4].account,"leo"); strcpy(user[4].password,"0216"); user[4].id = 5;
*/
void check_login(int);
void list(int);
void challenge(int,char*);
void game_layout(int);
void game_n_layout(int,int*,char);
void game_play(int,int,char*,char);
int check_finish(int,int,int*,int,char);
void logout(int);
void unknown(int);
int find_id(int);

void check_login(int client){
	char buf[1000];
	char check_acc[1000];
	char check_pass[1000];
	int i;
	int fail;
	
	for(i = 0; i < 5; i++) user[i].login = 0;
	while(1){
		fail = 0;
		memset(buf, 0, sizeof(buf));
		strcpy(buf,"Account:");
		send(client, buf, strlen(buf), 0);
		
		memset(check_acc, 0, sizeof(check_acc));
		memset(check_pass, 0, sizeof(check_pass));
		recv(client, check_acc, sizeof(check_acc), 0);
		for(i = 0; i < 5; i++){
			printf("acc = %s...check_acc = %s...\n",user[i].account,check_acc);
			if(strcmp(user[i].account,check_acc) == 0 && user[i].login == 0){
				fail = 0;
				break;
			}
			else fail = 1;
		}
		if(fail == 1){
			memset(buf, 0, sizeof(buf));
                	strcpy(buf,"Account Not Registered or has already loginned\n");
                	send(client, buf, strlen(buf), 0);
			continue;
		}

		memset(buf, 0, sizeof(buf));
                strcpy(buf,"Password:");
                send(client, buf, strlen(buf), 0);

                recv(client, check_pass, sizeof(check_pass), 0);
		if(strcmp(user[i].password,check_pass) == 0){
			memset(buf, 0, sizeof(buf));
                	strcpy(buf,"Login Successfully\n You can use three comands below:\n(1)ls - list all users online\n(2)game - Play game!!\n(3)logout - logout from the web\n");
                	send(client, buf, strlen(buf), 0);
			user[i].login = 1;
			user[i].socket = client;
			break;
		}
		else{
			memset(buf, 0, sizeof(buf));
                	strcpy(buf,"Something wrong with your Password");
                	send(client, buf, strlen(buf), 0);
			continue;
		}
	}
}

void list(int client){
	char buf[1000];
	int i;
	send(client,"========Online user:=============\n",strlen("========Online user:=============\n"),0);
	send(client,"========    ID      =============\n",strlen("========    ID      =============\n"),0);
	
	for(i = 0; i < 5; i++){
		if(user[i].login == 1){
			memset(buf, 0, sizeof(buf));
			sprintf(buf,"%d  |  %s\n",user[i].id, user[i].account);
			send(client, buf, strlen(buf), 0);
		}
	}
	
}

void challenge(int client, char *read){
	
	char response[100];
	char buf[1000];
	char toy;
	int i;
	int client_id = find_id(client);

	list(client);
	send(client,"\nChoose one id of online user to play o/x game with\n",strlen("\nChoose one id of online user to play o/x game with\n"),0);
	
	memset(read, 0, sizeof(read));
	recv(client, read, 100, 0);
	int id = atoi(read);
	
	for(i = 0; i < 5; i++){
		if(user[i].id == id){
			send(client,"Wait for player's response\n",strlen("Wait for player's response\n"), 0);
			memset(buf, 0, sizeof(buf));
			sprintf(buf,"[%s] invites you to play o/x game. Join the game?\n",user[client_id].account);
			send(user[i].socket, buf, strlen(buf), 0);

			memset(read, 0, sizeof(read));
			recv(user[i].socket, read, 100, 0); //不能用strlen(read)
			if(strcmp(read,"yes") == 0){
				send(client,"Your rival Join the game......O/X game startle!!!!!!!\n",strlen("Your rivalry Join the game......O/X game startle!!!!!!!\n"), 0);
				send(client,"Choose O or X .......\n",strlen("Choose O or X .......\n"), 0);
				send(user[i].socket,"You Join the game now.......O/X game startle!!!!!!!\n",strlen("You Join the game now.......O/X game startle!!!!!!!\n"), 0);
				send(user[i].socket,"Waiting for another player chooseing O/X........\n",strlen("Waiting for another player chooseing O/X........\n"), 0);
				memset(buf, 0, sizeof(buf));
				recv(client, buf, sizeof(buf), 0);
				if(strcmp(buf,"O") == 0){
					toy == 'O';
					send(client,"You now playing O....\n",strlen("You now playing O....\n"), 0);
					send(user[i].socket,"You now playing X....\n",strlen("You now playing X....\n"), 0);
				}
				else if(strcmp(buf,"X") == 0){
					toy == 'X';
                                        send(client,"You now playing X....\n",strlen("You now playing X....\n"), 0);
                                        send(user[i].socket,"You now playing O....\n",strlen("You now playing O....\n"), 0);
                                }
				game_play(client,user[i].socket,read,toy);
				return;
			}
			else{
				send(client,"The player doesn't wanna join the game......\n",strlen("The player doesn't wanna join the game......\n"), 0);
				return;
			}
		}
	}
}

void game_layout(int client){
	send(client,"\n  0  |  1  |  2  \n",strlen("\n  0  |  1  |  2  \n"), 0);
	send(client,"\n --------------- \n",strlen("\n --------------- \n"), 0);
	send(client,"\n  3  |  4  |  5  \n",strlen("\n  3  |  4  |  5  \n"), 0);
	send(client,"\n --------------- \n",strlen("\n --------------- \n"), 0);
	send(client,"\n  6  |  7  |  8  \n",strlen("\n  6  |  7  |  8  \n"), 0);
}

void game_n_layout(int client, int record[9], char pick_one){
	char pick_two;
	char buf[1000];
	char tmp[9];
	int i;
	if(pick_one == 'O') pick_two = 'X';
	else pick_two = 'O';
	memset(buf, 0, sizeof(buf));
	memset(tmp, 0, sizeof(tmp));
	for(i = 0; i < 9; i++){
		if(record[i] == 1) tmp[i] = pick_one;
		else if(record[i] == 2) tmp[i] = pick_two;
	}
	sprintf(buf,"\n  %c  |  %c  |  %c  \n  %c  |  %c  |  %c  \n  %c  |  %c  |  %c  \n",tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8]);
	buf[strlen(buf)] = '\0';
	
	send(client, buf, strlen(buf), 0);
}

void game_play(int inviter, int opponent, char *read, char pick_one){
	
	int game_record[9];
	int choose, done = 0;
	memset(game_record, 0, sizeof(game_record));

	while( done == 0){
		game_layout(inviter);
		game_n_layout(inviter,game_record,pick_one);
		send(inviter,"Your turn: \n",strlen("Your turn: \n"), 0);
		
		game_n_layout(opponent,game_record,pick_one);
		send(opponent,"Waiting for other choosing.....\n",strlen("Waiting for other choosing.....\n"), 0);

		memset(read, 0, sizeof(read));
		recv(inviter,read ,1024, 0);
		choose = (int)read[0]%48;
		while( game_record[choose] != 0){
			send(inviter,"This is be choosing...\n",strlen("This is be choosing...\n"), 0);
			memset(read, 0, sizeof(read));
			recv(inviter, read, 1024, 0);
			choose = (int)read%48;
		}
		game_record[choose] = 1;

		done = check_finish(inviter,opponent, game_record, done, pick_one);
		if( done == 1) break;

                game_layout(opponent);
                game_n_layout(opponent,game_record,pick_one);
                send(opponent,"Your turn: \n",strlen("Your turn: \n"), 0);

                game_n_layout(inviter,game_record,pick_one);
                send(inviter,"Waiting for other choosing.....\n",strlen("Waiting for other choosing.....\n"), 0);

                memset(read, 0, sizeof(read));
                recv(opponent,read ,1024, 0);
                choose = (int)read[0]%48;
                while( game_record[choose] != 0){
                        send(opponent,"This is be choosing...\n",strlen("This is be choosing...\n"), 0);
                        memset(read, 0, sizeof(read));
                        recv(opponent, read, 1024, 0);
                        choose = (int)read%48;
                }
		game_record[choose] = 2;

		done = check_finish(inviter, opponent, game_record, done, pick_one);
	}
}

int check_finish(int client1, int client2, int record[9], int done, char pick){

	int flag = 0;
	if((record[0] == 1 && record[1] == 1 && record[2] == 1 )||
	(record[3] == 1 && record[4] == 1 && record[5] == 1) ||
	(record[6] == 1 && record[7] == 1 && record[8] == 1) ||
	(record[0] == 1 && record[3] == 1 && record[6] == 1) ||
	(record[1] == 1 && record[4] == 1 && record[7] == 1) ||
	(record[2] == 1 && record[5] == 1 && record[8] == 1) ||
	(record[0] == 1 && record[4] == 1 && record[8] == 1) ||
	(record[2] == 1 && record[4] == 1 && record[6] == 1) ) flag = 1;

        if((record[0] == 2 && record[1] == 2 && record[2] == 2 )||
        (record[3] == 2 && record[4] == 2 && record[5] == 2) ||
        (record[6] == 2 && record[7] == 2 && record[8] == 2) ||
        (record[0] == 2 && record[3] == 2 && record[6] == 2) ||
        (record[1] == 2 && record[4] == 2 && record[7] == 2) ||
        (record[2] == 2 && record[5] == 2 && record[8] == 2) ||
        (record[0] == 2 && record[4] == 2 && record[8] == 2) ||
        (record[2] == 2 && record[4] == 2 && record[6] == 2) ) flag = 2;

	if(flag == 0){
		send(client1,"---------Pacific Ocean~~~Peace!!!!!!!!---------\n",strlen("---------Pacific Ocean~~~Peace!!!!!!!!---------\n"), 0);
		send(client2,"---------Pacific Ocean~~~Peace!!!!!!!!---------\n",strlen("---------Pacific Ocean~~~Peace!!!!!!!!---------\n"), 0);
		return 1;
	}
	else if(flag == 1){
		send(client1,"----------You Win la-------------\n",strlen("----------You Win la-------------\n"), 0);
		send(client2,"----------You Lose la-------------\n",strlen("----------You Lose la-------------\n"), 0);
		return 1;
	}
	else if(flag == 2){
		send(client2,"----------You Win la-------------\n",strlen("----------You Win la-------------\n"), 0);
                send(client1,"----------You Lose la-------------\n",strlen("----------You Lose la-------------\n"), 0);
		return 1;
	}
	return 0;
}

void logout(int client){
	int client_id;
	send(client,"Logout....\n",strlen("Logout....\n"),0);
	client_id = find_id(client);
	user[client_id].login = 0;
}

int find_id(int client){
	int id;
	int i;
	for(i = 0; i < 5; i++){
		if(user[i].socket == client){
			id = i;
			break;
		}
	}
	return id ;
}

void unknown(int client){
	char buf[1000];
	
	memset(buf, 0, sizeof(buf));
	strcpy(buf,"Unknown Command... You should use below command instead.\n(1)ls - list all users online\n(2)game - Play game!!\n(3)logout - logout from the web\n");
	send(client,buf, sizeof(buf), 0);
}

int main(){

    //struct user_space user[5];
    strcpy(user[0].account,"Ian\0"); strcpy(user[0].password,"1104"); user[0].id = 0; user[0].login = 0;
    strcpy(user[1].account,"LTN\0"); strcpy(user[1].password,"0917"); user[1].id = 1; user[1].login = 0;
    strcpy(user[2].account,"annie\0"); strcpy(user[2].password,"1209"); user[2].id = 2; user[2].login = 0;
    strcpy(user[3].account,"cindy\0"); strcpy(user[3].password,"0214"); user[3].id = 3; user[3].login = 0;
    strcpy(user[4].account,"leo\0"); strcpy(user[4].password,"0216"); user[4].id = 4; user[4].login = 0;

    printf("Configuring local address...\n"); /*------創建local address的資料結構------*/
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address); /*--------用8080port去連結-------*/


    printf("Creating socket...\n"); /*-------創建socket 界面-------*/
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }


    printf("Binding socket to local address...\n"); /*-------將socket綁定local address-----------*/
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);


    printf("Listening...\n"); /*--------listen socket-------------*/
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Waiting for connections...\n"); /*------伺服器運作中--------*/

    while(1) { 

        struct sockaddr_storage client_address;
        socklen_t client_len = sizeof(client_address);
        SOCKET socket_client = accept(socket_listen, /*-----------等待新的client進來------------*/
                (struct sockaddr*) &client_address, &client_len);
        if (!ISVALIDSOCKET(socket_client)) {
            fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        char address_buffer[100];
        getnameinfo((struct sockaddr*)&client_address, /*---------取得client的資訊--------*/
                client_len, address_buffer, sizeof(address_buffer), 0, 0,
                NI_NUMERICHOST);
        printf("New connection from %s\n", address_buffer);

        int pid = fork();

        if (pid == 0) { //child process
            CLOSESOCKET(socket_listen);
	    check_login(socket_client);
            while(1) {
                char read[1024];
		memset(read,'\0', sizeof(read));
                int bytes_received = recv(socket_client, read, 1024, 0); /*-----處理接收訊息---------*/
                if (bytes_received < 1) {
                    CLOSESOCKET(socket_client);
                    exit(0);
                }

		if(strcmp(read,"ls") == 0) list(socket_client);
		else if(strcmp(read,"game") == 0) challenge(socket_client, read);
		else if(strcmp(read,"logout") == 0 ) logout(socket_client);
		else unknown(socket_client);
            }
        }

        CLOSESOCKET(socket_client);

    } //while(1)



    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

#if defined(_WIN32)
    WSACleanup();
#endif


    printf("Finished.\n");

    return 0;
}
