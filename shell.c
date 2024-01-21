/*
    Code written by:
    Stefania Douliaka, 00974
    Panagiotis Karoutsos, 02034
    Olga Vasileiou, 01691
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>                                               
#include <unistd.h>                                                  
#include <string.h>
#include <fcntl.h>                                                  
                                                     
#define TOKEN 64                                                    
#define BUFFERSIZE 1024                                                


// Function declarations
void line_execution(int argc, char **args);
char **break_buffer(char *buffer);
char *get_input(void);
int execute_args(char **args, int exec_value);         
int change_dir(char **args, int check_value);
int spawn_process(char **args, int exec_value);
void append_found(char *name, char *buffer);
void overwrite_found(char* name, char *buffer);
void pipe_tokeniser(char *buffer);
void pipe_found(char **buffer, char **pipe_buffer);

int main(int argc, char **argv)                                     
                                                                    
{
    char *buffer, **arr_args;
    int status = 1;

    if (argc > 1)                               // If the program receives more than its name
    {
        line_execution(argc, argv);
        return EXIT_SUCCESS;                                       
    }

    while (status)
    {
        printf("my_shell$ ");
        buffer = get_input();                   // buffer = "ls -la"
                                                                    
        arr_args = break_buffer(buffer);                            
        
        status = execute_args(arr_args, 0);     // arr[][] = {"ls","-la",NULL}

        free(buffer);
        buffer = NULL;

        free(arr_args);
        arr_args = NULL;
    }

    return EXIT_SUCCESS;
}


void line_execution(int argc, char **argv)
{
    int bufsize = TOKEN, i;
    char **buffer = malloc(bufsize * sizeof(char *));
    for (i = 1; i < argc; i++)
    {
        buffer[i - 1] = argv[i];                // Parse the argv to the 2D buff array
    }
    buffer[i++] = NULL;                         // Setting the final token as NULL
    execute_args(buffer, 0);
}


char **break_buffer(char *buffer)
{

    int bufsize = TOKEN;
    int position = 0;

    int i = 0;
    int j = 0;

    char *name = calloc(sizeof(char), strlen(buffer));          // Initialize each block with 0 and has 2 parameters
                                                                           
    char **tokens = calloc(sizeof(char *), TOKEN);

    char *token;

    if (!tokens)                                                // If we have no tokens
    {
        fprintf(stderr, "Allocation Error\n");                  // When we need to output an error message: Error 404 not found
        exit(EXIT_FAILURE);
    }
    if (!name)                                                  // If we have no name
    {
        fprintf(stderr, "Allocation Error\n");                              
        exit(EXIT_FAILURE);
    }

    // Scan the buffer for overwrite(>), append(>>) or pipe(|)
    for (i = 0; i < strlen(buffer); i++)                  
    {
        if (buffer[i] == '>' && buffer[i + 1] == '>')
        {
            while (i + 3 < strlen(buffer))
            {
                name[j] = buffer[i + 3];                        // Find the specific command written
                j++;
                i++;
            }

            buffer[strlen(buffer) - j - 4] = '\0';              // From: buffer = "ls -la >> txt\0" , name = "txt\0"
                                                                // To: buffer = "ls -la\0", name = "txt\0"

            append_found(name, buffer);
            tokens[0] = NULL;
            return tokens;
        }

        if (buffer[i] == '>' && buffer[i + 1] == ' ')           // Overwrite check: >
        {
            while (i + 2 < strlen(buffer))
            {
                name[j] = buffer[i + 2];                        // Find the specific command written
                j++;
                i++;
            }
            buffer[strlen(buffer) - j - 4] = '\0';              // From: buffer = "ls -la >> txt\0" , name = "out.txt\0"
                                                                // To: buffer = "ls -la\0", name = "out.txt\0"

            overwrite_found(name, buffer);
            tokens[0] = NULL;                                   // The first cell = 0
            return tokens;
        }

        if (buffer[i] == '|')                                   // Pipe check: |
        {
            pipe_tokeniser(buffer);
            tokens[0] = NULL;
            return tokens;
        }
    }

    // Tokenize one command buffer
    token = strtok(buffer, " ");                                // Read until the delimiter is found
    
    while (token != NULL)                                       // If there are tokens
    {
        tokens[position] = token;
        position++;
        token = strtok(NULL, " ");                              // Read until the delimiter (if we exceed the allocated size)
    }
    tokens[position] = NULL;
    return tokens;
}


char *get_input(void)
{
    int bufsize = BUFFERSIZE;                                                   
    int position = 0;                                                       
    char *buffer = malloc(sizeof(char)*bufsize);                // Dynamic matrix: buffer
    int c;

    if (!buffer)                                                // If there is no buffer found
    {
        fprintf(stderr, "Allocation Error\n");                              
        exit(EXIT_FAILURE);                                                 
    }
    while (1)
    {
        // Read a character
        c = getchar();                                          // Return int

        if (c == EOF || c == '\n')                              // EOF stands for End of the File
        {
            buffer[position] = '\0';                                        
            return buffer;
        }
        else
        {
            buffer[position] = c;                                           
        }
        position++;

        if (position >= bufsize)                                // If we exceed realloc
        {
            bufsize += BUFFERSIZE;                                          
            buffer = realloc(buffer, bufsize);                  // Realloc because of the new size
            if (!buffer)
            {
                fprintf(stderr, "Allocation Error\n");                      

                exit(EXIT_FAILURE);
            }
        }
    }
}


int execute_args(char **args, int exec_value)
{
    if (args[exec_value] == NULL)
    {
        return 1;
    }

    if (strcmp(args[exec_value], "cd") == 0)                    // Compare 2 strings
                                                                // If "cd" is written
    {
        return change_dir(args, 1);
    }

    if (strcmp(args[exec_value], "exit") == 0)                  // If "exit" is written
    {
        exit(EXIT_SUCCESS);                                     // Return 0
    }

    return spawn_process(args, exec_value);
}


int change_dir(char **args, int check_value)
{
    if (args[check_value] == NULL) 
    {
        fprintf(stderr, "Expected Argument to \"cd\"\n");           
    }
    else
    {
        if (chdir(args[check_value]) != 0)                      // Change the process's working directory to PATH
        {
            perror("Error");                                    // Print a message describing the meaning of the value of errno
                                                                // errno: set by system calls and some library functions in the event of an error to indicate what went wrong
        }
    }
    return 1;
}


int spawn_process(char **args, int exec_value)
{
    pid_t pid;                                                  // (int) data type that stands for process identification
    pid = fork();                                               // The fork system call creates a new process

    if (!pid)                                                   // Child Process 
    {
        if (execvp(args[exec_value], args) == -1)               // Duplicate (copy) the actions of the shell while searching for an executable file
        {
            perror("WARNING!");                                             
            return 0;
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("Fork Failed.\n");
        return 0;
    }
    else                                                        // Parent Process
    {
        waitpid(pid, NULL, 0);                                  // Wait for a process to finish
    }

    return 1;
}


void append_found(char *name, char *buffer)
{
    char **args;
    int status;
    int save_out;
    int outfd = open(name, O_RDWR | O_CREAT | O_APPEND, 0600);  // Open the file specified by pathname

    if (outfd < 0)
    {
        fprintf(stderr, "Failed opening %s", name);                         
        return;
    }

    save_out = dup(STDOUT_FILENO);                              // The dup() system call creates a copy of a file descriptor.

    if (dup2(outfd, STDOUT_FILENO) < 0)                         // The dup2() function duplicates an open file descriptor.
    {
        fprintf(stderr, "Failed redirecting stdout.");                      

        return;
    }

    args = break_buffer(buffer);                                // buffer = ls -la";
                                                                // args = {"ls","-la",NULL};

    status = execute_args(args, 0);

    fflush(stdout);                                             // Clean the stdout
    close(outfd);

    dup2(save_out, STDOUT_FILENO);
    close(save_out);
}


void overwrite_found(char *name, char *buffer)
{
    char **args;
    int status;
    int save_out;
    int outfd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0600);   // Open the file specified by pathname

    if (outfd < 0)
    {
        fprintf(stderr, "Failed opening %s", name);
        return;
    }

    save_out = dup(STDOUT_FILENO);

    if (dup2(outfd, STDOUT_FILENO) < 0)                                     
    {
        fprintf(stderr, "Failed redirecting stdout.");                     

        return;
    }

    args = break_buffer(buffer);                                // buffer = ls -la";
                                                                // args = {"ls","-la",NULL};

    status = execute_args(args, 0);

    fflush(stdout);                                                         
    close(outfd);

    dup2(save_out, STDOUT_FILENO);

    close(save_out);
}


void pipe_tokeniser(char *buffer)
{
    char *token;
    int bufsize = TOKEN;
    char **tokens = malloc(bufsize * sizeof(char *));
    char **pipe_tokens = malloc(bufsize * sizeof(char *));
    int position = 0, pipe_position = 0, break_flag = 0;

    token = strtok(buffer, " ");                                // ls -la | wc -c
                                                                // Read until the delimiter is found
    while (token != NULL)                                       // While we have no tokens
    {
        if (strcmp(token, "|") != 0)
        {
            if (break_flag == 0)
            {
                tokens[position] = token;
                position++;
            }
            else
            {
                pipe_tokens[pipe_position] = token;
                pipe_position++;
            }
        }
        else
        {
            break_flag = 1;
        }

        token = strtok(NULL, " ");                                          
    }

    tokens[position] = NULL;                                    // "ls" "-la" NULL
    pipe_tokens[pipe_position] = NULL;                          // "wc" "-c" NULL

    pipe_found(tokens, pipe_tokens);
}


void pipe_found(char **buffer, char **pipe_buffer)
{
    int pipefd[2];                                              // Array of two file descriptors
    pid_t p1;                                                  
    int save_out;

    if (pipe(pipefd) < 0)                                       // If pipe fails
    {
        printf("Pipe Failed.\n");
        return;
    }

    p1 = fork();

    if (!p1)
    {
        save_out = dup(STDOUT_FILENO);                          // Redirection (could use stdout)
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);

        if (spawn_process(buffer, 0) == 0)
        {
            perror("Error.");
            return;
        }
        close(pipefd[1]);
        dup2(save_out, STDOUT_FILENO);                          // Restore
        close(save_out);
    }
    else if (p1 < 0)
    {
        printf("Fork Failed.\n");
        return;
    }
    else
    {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);

        if (spawn_process(pipe_buffer, 0) == 0)
        {
            fprintf(stderr, "Failed execution of the second command.\n");
            return;
        }
        waitpid(p1, NULL, 0);
    }
}
