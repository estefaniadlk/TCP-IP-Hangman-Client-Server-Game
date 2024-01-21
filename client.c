/*
    Code written by:
    Stefania Douliaka, 00974
    Panagiotis Karoutsos, 02034
    Olga Vasileiou, 01691
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFERSIZE 1024
#define SIZE 256


void showHangman(int lives, int *spots, char letter);

int main(int argc, char **argv)
{
    int sockfd, servfd, i, spots[SIZE], lives, lettercount;
    char in_buffer[BUFFERSIZE], out_buffer[BUFFERSIZE], letter, *hangmanOutput, message[SIZE];

    struct sockaddr_in server, client;                          // These are the basic structures for all syscalls and functions that deal with internet addresses.
    
    if (argc != 3)
    {
        printf("Write %s IPNumber PortNumber\n", argv[0]);
        printf("IPNumber: 127.0.0.1 for localhost\n");
        exit(EXIT_FAILURE);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);                   // s = socket(domain, type, protocol)

    if (sockfd == -1)
    {
        fprintf(stderr, "Socket creation: Failure\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Socket creation: Success\n");

    bzero(&server, sizeof(struct sockaddr_in));                 // Used to set all the socket structures with null values

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));                     // Used to convert an IP port number in host byte order to the IP port number in network byte order.

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) != 0)
    {
        fprintf(stderr, "Client Connection to server: Failure\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Client connection to server: Success\n");

    bzero(in_buffer, BUFFERSIZE);
    
    recv(sockfd, in_buffer, BUFFERSIZE, 0);                     // Receive message (in_buffer) from the server's socket

    hangmanOutput = strtok(in_buffer, "\n");
    fprintf(stdout, "%s", strtok(NULL,"|"));
    fprintf(stdout, "The word that you need to find has %d letters in total.\n", (int)strlen(hangmanOutput));
    fprintf(stdout, "%c ", hangmanOutput[0]);


    for( i = 1; i < strlen(hangmanOutput)-1 ; i++)
    {
        fprintf(stdout, "%c ", hangmanOutput[i]);
    }

    fprintf(stdout, "%c\n", hangmanOutput[strlen(hangmanOutput)-1]);

    while(1){

        printf("Enter a letter: ");
        scanf("%c", &letter);

        while(letter  < 'a' || letter > 'z' && letter != '\n'){

            printf("Wrong Input! Try again: ");
            scanf("%c", &letter);
        }

        send(sockfd, &letter, 1, 0);
        read(sockfd, spots, SIZE);
        read(sockfd, &lives, 1);
        showHangman(lives, spots, letter);

        recv(sockfd, hangmanOutput, strlen(hangmanOutput), 0);                  // Receive the hangmanOutput from the server's socket

        fprintf(stdout, "%c ", hangmanOutput[0]);

        for( i = 1; i < strlen(hangmanOutput)-1 ; i++)
        {
            fprintf(stdout, "%c ", hangmanOutput[i]);
        }

        fprintf(stdout, "%c\n", hangmanOutput[strlen(hangmanOutput)-1]);

        if(strchr(hangmanOutput, '_') == NULL){

            fprintf(stdout, "\nYou won!\n\n");
            break;
        }

        if(lives == 0){

            fprintf(stdout, "\nYou lost!\n\n");
            break;
        }
    }

    close(sockfd);
}

void showHangman(int lives, int *spots, char letter)                        
{ // Show the hangman after each round
    
    int i;

    switch(lives)
    {
        // Case number = remaining lifes
        case 0:                                                                
            system("clear");
            printf("\n\t +------+ ");
            printf("\n\t |/     | ");
            printf("\n\t |     %cO/",'\\');
            printf("\n\t |      | ");
            printf("\n\t |     / %c",'\\');
            printf("\n\t/|%c       ",'\\');

            if(spots[0] == 0)

                printf("\tWrong guess. Try another one!");

            else{

                printf("\tLetter '%c' found in spot(s): ", letter);

                for(i = 0; i < SIZE; i++){

                    if(spots[i] == 0)
                        break;
                    printf("%d ",spots[i]);
                }
            }

            printf("\tLives: %d", lives); 
            printf("\n");
            break;

        case 1:                                                                                 
            system("clear");        
            printf("\n\t +------+ ");
            printf("\n\t |/     | ");
            printf("\n\t |     %cO/",'\\');
            printf("\n\t |      | ");
            printf("\n\t |       %c",'\\');
            printf("\n\t/|%c       ",'\\');

            if(spots[0] == 0)

                printf("\tWrong guess. Try another one!");

            else{

                printf("\tLetter '%c' found in spot(s): ", letter);

                for(i = 0; i < SIZE; i++){

                    if(spots[i] == 0)
                        break;
                    printf("%d ",spots[i]);
                }
            }

            printf("\tLives: %d", lives); 
            printf("\n");
            break;

        case 2:
            system("clear");
            printf("\n\t +------+ ");
            printf("\n\t |/     | ");
            printf("\n\t |     %cO/",'\\');
            printf("\n\t |      | ");
            printf("\n\t |        ");
            printf("\n\t/|%c      ",'\\');

            if(spots[0] == 0)

                printf("\tWrong guess. Try another one!");

            else{

                printf("\tLetter '%c' found in spot(s): ", letter);

                for(i = 0; i < SIZE; i++){

                    if(spots[i] == 0)
                        break;
                    printf("%d ",spots[i]);
                }
            }

            printf("\tLives: %d", lives); 
            printf("\n");
            break;

        case 3:
            system("clear");
            printf("\n\t +------+ ");
            printf("\n\t |/     | ");
            printf("\n\t |     %cO/",'\\');
            printf("\n\t |        ");
            printf("\n\t |        ");
            printf("\n\t/|%c      ",'\\');

           if(spots[0] == 0)

                printf("\tWrong guess. Try another one!");

            else{

                printf("\tLetter '%c' found in spot(s): ", letter);

                for(i = 0; i < SIZE; i++){

                    if(spots[i] == 0)
                        break;
                    printf("%d ",spots[i]);
                }
            }

            printf("\tLives: %d", lives); 
            printf("\n");
            break;

        case 4:
            system("clear");
            printf("\n\t +------+ ");
            printf("\n\t |/     | ");
            printf("\n\t |     %cO ",'\\');
            printf("\n\t |        ");
            printf("\n\t |        ");
            printf("\n\t/|%c      ",'\\');

           if(spots[0] == 0)

                printf("\tWrong guess. Try another one!");

            else{

                printf("\tLetter '%c' found in spot(s): ", letter);

                for(i = 0; i < SIZE; i++){

                    if(spots[i] == 0)
                        break;
                    printf("%d ",spots[i]);
                }
            }

            printf("\tLives: %d", lives); 
            printf("\n");
            break;

        case 5:
            system("clear");
            printf("\n\t +------+ ");
            printf("\n\t |/     | ");
            printf("\n\t |      O ");
            printf("\n\t |        ");
            printf("\n\t |        ");
            printf("\n\t/|%c      ",'\\');

            if(spots[0] == 0)

                printf("\tWrong guess. Try another one!");

            else{

                printf("\tLetter '%c' found in spot(s): ", letter);

                for(i = 0; i < SIZE; i++){

                    if(spots[i] == 0)
                        break;
                    printf("%d ",spots[i]);
                }
            }

            printf("\tLives: %d", lives); 
            printf("\n");
            break;
    }
    return;
}
