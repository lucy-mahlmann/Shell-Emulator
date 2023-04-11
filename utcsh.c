/*
  utcsh - The UTCS Shell

  Lucy Mahlmann - lam6744
  Reyva Chawla - rc49264
*/

/* Read the additional functions from util.h. They may be beneficial to you
in the future */
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

/* Global variables */
/* The array for holding shell paths. Can be edited by the functions in util.c*/
char shell_paths[MAX_ENTRIES_IN_SHELLPATH][MAX_CHARS_PER_CMDLINE];
static char prompt[] = "utcsh> "; /* Command line prompt */
static char *default_shell_path[2] = {"/bin", NULL};
/* End Global Variables */

/* Convenience struct for describing a command. Modify this struct as you see
 * fit--add extra members to help you write your code. */
struct Command {
  char *name;       // command name
  char **args;      /* Argument array for the command, 
                    includes all input that was recieved from the command line
                    args[0] is the command name of the first command call 
                    */
  char *outputFile; /* Redirect target for file (NULL means no redirect, 
                    "ERROR" means invalid redirection) 
                    */
  int num_args;     // the number of arguments in the command.
};


char **tokenize_command_line(char *cmdline);
struct Command parse_command(char **tokens);
void eval(struct Command *cmd);
int try_exec_builtin(struct Command *cmd);
void exec_external_cmd(struct Command *cmd);

/* Main REPL: read, evaluate, and print. This function should remain relatively
   short: if it grows beyond 60 lines, you're doing too much in main() and
   should try to move some of that work into other functions. */
int main(int argc, char **argv) {
  // Lucy was driving
  set_shell_path(default_shell_path);
  bool is_open = false; // true if file is already open
  // main while loop to run the RELP
  while (1) {
    char *cmdline = NULL;
    size_t len = 0; 

    /* Read */
    if(argc == 2) {
      // entered script mode 
      const char *file_name = argv[1];
      // get the size of the file
      struct stat size_struct;
      stat(file_name, &size_struct);
      long size = size_struct.st_size;
      if(size == 0){
        // error and exits if file has a size of zero
        printf("13\n");
        display_errors();
        exit(1);
      }
      // Lucy stopped
      // Reyva was driving
      FILE *open_file;
      if (!is_open) {
        // only open the file one time
        open_file = fopen(file_name, "r");
        if (open_file == NULL) {
          // errors and exits if file_name is an invalid file
          printf("12\n");
          display_errors();
          exit(1);
        } 
      }
      // file has been opened
      is_open = true; 
      if (feof(open_file)) {
        // exit if reached the end of the file 
        // without returning an error
        exit(0);
      }
      size_t ch = getline(&cmdline, &len, open_file);

    } else if (argc > 2) {
        // errors and exits if there are too many arguments 
        printf("11\n");
        display_errors();
        exit(1);

    } else {
      // enter terminal mode, read in from standard input
      printf("%s", prompt);
      size_t ch = getline(&cmdline, &len, stdin);    
    }
    // Reyva stopped
    // Lucy was driving
    if(cmdline == NULL || len == 0){
      // errors and exits if cmdline is NULL
      printf("10\n");
      display_errors();
      exit(0);
    }

    // a pointer to an array of strings that holds all the commands 
    // from the commandline and their arguments
    char ***token_array_ptr = tokenize_command_line(cmdline);
    if (token_array_ptr[0] != NULL) {
      // makes sure that the command in not empty

      /* Evaluate */
      int num_of_cmds = 0;
      // create a ptr to an array of struct Commands that hold all
      // of the commands inputed
      struct Command *struct_array = malloc(MAX_CHARS_PER_CMDLINE 
        * sizeof(struct Command));
      // fill in the array of struct Commands
      while(token_array_ptr[num_of_cmds] != NULL) {
        struct_array[num_of_cmds] = parse_command(token_array_ptr[num_of_cmds]);
        num_of_cmds++;
      }

      // Lucy stopped
      // Reyva was driving
      int status;
      // loop through all the commands and evaluate if is a valid command
      for (int i = 0; i < num_of_cmds; i++) {
        struct Command curr_cmd_struct = struct_array[i];
        if (curr_cmd_struct.outputFile == NULL 
        || strcmp(curr_cmd_struct.outputFile, "ERROR") != 0) {
          if (curr_cmd_struct.name != NULL) {
            // is valid therefore evaluate
            eval(&curr_cmd_struct);
          }
        }  
      }

      // while(wait(&status) != -1){
      //   // parent waits for ALL child processes to complete
      // }
    }
  }
  return 0;
  // Reyva stopped
}

/** Tokenizing a command based on whitespace with strtok
 *
 * Turns a command str into an array of arguments and returns the array
 */
char **make_command_str(char *curr_cmd) {
  // Lucy was driving  
  char *token;
  char **token_array = malloc(MAX_WORDS_PER_CMDLINE * sizeof(char*));

  int count = 0;
  token = strtok(curr_cmd, " \n\t");
  // loop through the elements in the current command 
  // until it reaches NULL terminator
  while(token != NULL) {
    // set char * token to the next element in the token_array
    // will be an array of all the arguments including the 
    // command name when finished tokenizing that is NULL terminated
    token_array[count] = token;
    count++;
    token = strtok(NULL, " \n\t");
  }
  // Lucy stopped
  return token_array;
}

/** Turn a command line into tokens with strtok
 *
 * This function turns a command line into an array of arguments, making it
 * much easier to process. First, you should figure out how many arguments you
 * have, then allocate a char** of sufficient size and fill it using strtok()
 */
char **tokenize_command_line(char *cmdline) {
  // Reyva was driving  
  int num_cmds = 0;
  char *command;
  char **command_array = malloc(MAX_WORDS_PER_CMDLINE * sizeof(char*));

  // tokenize cmdline based on & to split the line into commands
  command = strtok(cmdline, "&");
  // loop through the elements in the cmdline until it reaches NULL terminator
  while(command != NULL) {
    // set char * token to the next element in the command_array
    // will be an array of all the commands and is NULL terminated
    command_array[num_cmds] = command;
    num_cmds++;
    command = strtok(NULL, "&");
  }

  // creates a pointer to an array of strings which holds all the commands
  // and their arguments
  char ***cmds_array_ptr = malloc(MAX_WORDS_PER_CMDLINE * sizeof(char**));
  // loop through each command and tokenize based on whitespace
  for (int i = 0; i < num_cmds; i++) {
    char *curr_cmd = command_array[i];
    cmds_array_ptr[i] = make_command_str(curr_cmd);
  }

  return cmds_array_ptr;
  // Reyva stopped
}


/** Turn tokens into a command.
 *
 * The `struct Command` represents a command to execute. This is the preferred
 * format for storing information about a command, though you are free to change
 * it. This function takes a sequence of tokens and turns them into a struct
 * Command.
 */
struct Command parse_command(char **tokens) {
  // Lucy was driving
  char *command_name = tokens[0];
  int count = 0;
  // loop through elements in tokens, counting the
  // number of elements in the array
  while(tokens[count] != NULL) {
    count++;
  }
  // create the struct Command for the current command
  struct Command command_exec = {
    .name = command_name, 
    .args = tokens, 
    .outputFile = NULL, 
    .num_args = count - 1 // the number of elements in tokens minus the cmd name
  };
  
  if (command_exec.num_args > MAX_WORDS_PER_CMDLINE) {
    // exit if the command is too long 
    exit(0);
  } 
  // Lucy stopped
  // Reyva was driving

  // loop through the arguments and check if redirection occurs
  for (int i = 0; i <= command_exec.num_args; i++) {
    if (command_exec.args[i] != NULL 
      && strcmp(command_exec.args[i], ">") == 0) {

      if (i == 0) {
        // error if there is no command name before
        // the redirect
        display_errors();
        command_exec.outputFile = "ERROR";
        printf("8\n");
        break;
      }

      // found a ">" could be a possible redirect
      if (command_exec.outputFile == NULL) {
        // this is the first redirect
        if (command_exec.args[i + 1] == NULL) {
          // errors if there is no file listed after redirect
          display_errors();
          printf("7\n");
          command_exec.outputFile = "ERROR";
          break;
        } 
        // set outputFile to the arg after the redirect
        command_exec.outputFile = command_exec.args[i + 1];
        if (command_exec.args[i + 2] != NULL 
          && strncmp(command_exec.args[i + 2], "/", 1) == 0) {
          // errors when the arg after the output file is also a file 
          command_exec.outputFile = "ERROR";
          printf("6\n");
          display_errors();
          break;
        }
        // Reyva stopped
        // Lucy was driving

        // removes the redirect symbol and the file from the commands
        // argument array
        command_exec.args[i] = command_exec.args[i + 2];
        i--;
        command_exec.num_args = command_exec.num_args - 2;
      } else {
        // errors if there was already a redirect 
        command_exec.outputFile = "ERROR";
        printf("5\n");
        display_errors();
        break;
      }
    }
  } 
  return command_exec;
  // Lucy stopped
} 

/* Display an error
 * prints an error message to stderr whenever an error occurs in the shell
 */
void display_errors(){
  // Lucy was driving 
  char emsg[30] = "An error has occurred\n";
  int nbytes_written = fprintf(stderr, emsg, 30);
  return;
  // Lucy stopped
}

/** Evaluate a single command
 *
 * Both built-ins and external commands can be passed to this function--it
 * should work out what the correct type is and take the appropriate action.
 */
void eval(struct Command *cmd) {
  // Lucy was driving
  pid_t pid;
  int is_built_in = try_exec_builtin(cmd);
  if (!is_built_in) {
    // is an external command
    pid = fork();
    if (pid == 0) {
      exec_external_cmd(cmd);
    }
    // parent waits for child to finish
    int status;
    if (waitpid(pid, &status, 0) < 0) {
      printf("1\n");
      display_errors();
      exit(0);
    }
    
  }
  return;
  // Lucy stopped
}

/** Execute built-in commands
 *
 * If the command is a built-in command, execute it and return 1 if appropriate
 * If the command is not a built-in command, do nothing and return 0
 */
int try_exec_builtin(struct Command *cmd) {
  // Lucy was driving
  if (strcmp(cmd->name, "cd") == 0) {
    if (cmd->num_args != 1) {
      // errors if more than one or no arguments are provided
      display_errors();
      return 1;
    }
    // executes built-in cd system call
    int res = chdir(cmd->args[1]);
    if (res != 0) {
      // errors if chdir failed
      display_errors();
    }
    return 1;
  }
  
  if (strcmp(cmd->name, "path") == 0) {
    // executes built-in path system call
    // checks if the first argument in an absolute path
    if (!is_absolute_path(cmd->args[1])) {
      // find an absolute path
      int result = set_shell_path(&(cmd->args[1]));
      if(result == 0) {
        // errors if setting shell path fails
        printf("2\n");
        display_errors();
      } 
    }
    return 1;
  }

  if (strcmp(cmd->name, "exit") == 0) {
    if (cmd->num_args != 0) {
      // errors if any arguments are provided
      display_errors();
    }
    // execute built-in exit system call
    exit(0);
    return 1;
  } 
  
  return 0;
  // Lucy stopped
}

/* Helper function that gets the absolute path
 *
 * Takes in a Struct Command and returns a char* of the absolute path
 * for the path is stored in the Struct Command in the args variable
 * Returns null if no absolute path was found 
 */
char *get_abs_path(struct Command *cmd) {
  // Reyva was driving
  char *abs_path;
  // loop through each element in the shells_paths array
  // returns the first absolute path that is found
  for (int i= 0; i < MAX_ENTRIES_IN_SHELLPATH; i++) {
    abs_path = exe_exists_in_dir(shell_paths[i], cmd->args[0], false);
    if (abs_path != NULL) {
    // found first possible absolute path
    return abs_path;
    }
  }

  // no absolute path was found
  return NULL;
  // Reyva stopped 
}

/** Execute an external command
 *
 * Execute an external command by fork-and-exec. Should also take care of
 * output redirection, if any is requested
 */
void exec_external_cmd(struct Command *cmd) {
  // Reyva was driving
  // int status;
  // pid_t child = fork();
  // if (child == -1) {
  //   // errors if fork fails
  //   display_errors();
  //   return;
  // } else if (child == 0) {
    // this is the child process

    char *abs_path;
    // check if the the cmd name is an absolute path
    if(!is_absolute_path(cmd->args[0])) {
      // need to find an absolute path
      abs_path = get_abs_path(cmd);
    } else {  
      // is already an absolute path
      abs_path = cmd->name;
    }

    if (cmd->outputFile != NULL) {
      // there is a valid redirect so open the outputFile and write
      // output/errors to it
      int fd = open(cmd->outputFile, O_TRUNC | O_CREAT | O_WRONLY, 0666);
      dup2(fd, STDOUT_FILENO);
      dup2(fd, STDERR_FILENO);
    }
    
    if (execv(abs_path, cmd->args) == -1) {
      // display error if execv fails
      printf("3\n");
      //printf("%s\n", abs_path);
      display_errors();
      exit(0);
    }

   // return;
  //} 
  return;
  // Reyva stopped 
}
