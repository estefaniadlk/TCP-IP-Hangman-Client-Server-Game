/*
    Code written by:
    Stefania Douliaka, 00974
    Panagiotis Karoutsos, 02034
    Olga Vasileiou, 01691
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>                   
#include <sys/socket.h>                                                         
#include <sys/types.h>   
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define BUFFERSIZE 1024
#define BACKLOG 10
#define SIZE 256


void signal_handler(int);

int sockfd, clientfd;                   // Global variables

int main(int argc, char **argv)                                                
{

    int  len, lf, rows, i, j, random_choice, status, found, lives = 5, spots[SIZE], lettercount;
    char **dictionary, line[SIZE], *word, *hangmanOutput, in_buffer[BUFFERSIZE], out_buffer[BUFFERSIZE], attempt;
 
    pid_t pid;                                                  // pid_t: (int) data type that stands for process identification

    struct sockaddr_in client, server;                          // These are the basic structures for all syscalls and functions that deal with internet addresses. 

    if (argc != 3)                                              // 3 arguments (run./, port number, IP) needed in order for the code to run
    {
        printf("Write %s PortNumber TxtFile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT,signal_handler);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);                   // Create a new socket of type in domain, using protocol

    if (sockfd == -1)                                                           
    {
        fprintf(stderr, "Socket creation: Failure\n");          // When we need to output an error message: Error 404 not found.
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Socket creation: Success\n");

    bzero(&server,sizeof(struct sockaddr_in));                  // Erase the data in the n bytes of the memory starting at the location pointed to by s

    server.sin_family = AF_INET;                                // Match the socket() call
    server.sin_addr.s_addr = htonl(INADDR_ANY);                 // Bind to any local address
    server.sin_port = htons(atoi(argv[1]));                     // Specify port to listen on

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) != 0)      // Give the socket FD the local address ADDR (which is LEN bytes long)
    {
        fprintf(stderr, "Socket could not bind to address\n");                  
                                                                                
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Socked Binded to server address.\n");                      

    if (listen(sockfd, BACKLOG) != 0)                           // Define the backlog - max number of users who can connect that specific time
                                                                                
    {
        fprintf(stderr, "backlog queue could not be initialized\n");            

        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Initiliased backlog queue of length (%d)\n", BACKLOG);     

    FILE *fpointer;                             
    fpointer = fopen(argv[2],"r");                              // Read the .txt file 

    if (fpointer == NULL)                                       // Check if file is empty
    {
        fprintf(stderr,"No dictionary file detected\n");
        close(clientfd);
        close(sockfd);
        return 0;
    }
    
    fprintf(stdout,"Reading dictionary...\n");

    while(lf != -1)                                                             
    {
        lf = fscanf(fpointer,"%s",line);
        rows = rows + 1;
    }

    dictionary = (char **)malloc(rows * sizeof(char*));         // Malloc for the dictionary (2D matrix)

    fseek(fpointer,0,SEEK_SET);

    for(i = 0; i < rows; i++)                                   // Assign the values of every cell one by one
    {
        fscanf(fpointer,"%s",line);
        dictionary[i] = (char *)malloc(strlen(line) * sizeof(char));        // Malloc for the lines of the matrix
        strcpy(dictionary[i], line);
    }

    fclose(fpointer);                                                       // Set the pointer free
    
    fprintf(stdout, "Waiting for connections!\n");

    while (1)
    {
        clientfd = accept(sockfd, (struct sockaddr *)&client, &len);        // Stop the execution of the program, until a connection arrives and opens a new socket to communicate with it.

        if (!clientfd)
        {
            fprintf(stderr, "Error while trying to accept client. Exiting\n");
            exit(EXIT_FAILURE);
        }
        else printf("Client connected\n");

        // The hangman game
        if (!(pid = fork()))                                                // Same as: if (fork() == 0)
        {
            srand(time(NULL));
            random_choice = rand() % 100 + 1;

            word = (char *)malloc(strlen(dictionary[random_choice-1]) * sizeof(char));

            hangmanOutput = (char *)malloc(strlen(word) * sizeof(char));

            strcpy(word, dictionary[random_choice-1]);

            printf("%s\n",word);

            hangmanOutput[0] = word[0];                                 // For the first letter: we can see it

            for( i = 1; i < strlen(word)-1 ; i++)
            {
                hangmanOutput[i] = '_';                                 // For the middle letters: we can not see them (_)
            }

            hangmanOutput[strlen(word)-1] = word[strlen(word)-1];       // For the last letter: we can see it

            hangmanOutput[strlen(word)] = '\0';

            bzero(in_buffer, BUFFERSIZE);
            strcpy(in_buffer, hangmanOutput);

            strcat(in_buffer, "\n"); 
            strcat(in_buffer, "Welcome To The Hangman Games!\n");
                                 
            send(clientfd, in_buffer, BUFFERSIZE, 0);                   // Send buffer to the client through client socket

            while(1){

                recv(clientfd, &attempt, 1, 0);                         // Receive message (attempt) from client

                printf("Letter = %c\n", attempt);

                found = 0;

                bzero(spots, sizeof(spots));

                for(i = 1; i < strlen(word)-1; i++){

                    if(word[i] == attempt){

                        printf("%d\n",i);
                        found = 1;
                        hangmanOutput[i] = word[i];

                        for(j = 0; j < strlen(word)-2; j++)
                        {
                            if(spots[j] == 0)
                            {
                                spots[j] = i;
                                break;
                            }
                        }
                    }
                }

                if(found == 0)
                    lives--;

                write(clientfd, spots, SIZE);
                write(clientfd, &lives, 1);
                send(clientfd, hangmanOutput, strlen(word), 0);

                if(lives == 0){
                    close(clientfd);
                    break;
                }

                if(strchr(hangmanOutput, '_') == NULL){
                    close(clientfd);
                    break;
                }
            }
        }

        else
        {
        // Go up and wait for new connection
        }
        
    }

    close(clientfd);
    close(sockfd);
    return 0;
}

void signal_handler(int signnum){
// Handler for the signal SIGINT closes sockets and terminates program

    printf("\nServer shutting down.. \n");                  
    close(sockfd);
    close(clientfd);
    exit(0);
}
