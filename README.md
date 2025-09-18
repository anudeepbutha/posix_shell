Question 1
Problem Statement: Display requirement
Approach:
1. Retrieve userdetails like username, home directory using getpwuid() function and store them in character buffers.
2. Retrieve host details like hostname using gethostname() function and store them in character buffers.
3. Retrieve current working directory using getcwd() function and store them in character buffers.
4. Display the three parameters in the form of username@hostname:current_directory>

Question 2
Problem Statement: Implement cd, echo, pwd shell commands
Approach:
1. Tokenize the inputs using string tokeniser and based on the fist argument call the related functions.
2. cd is implemented using changedir function arguments to chdir function changes according to second parameter
3. echo is implemented based on the parameters where the arguments are splitted based on the single space character and printed in the new line
4. pwd is implemented using getcwd() function and the returned value is stored in a character and the contents of the character buffer are displayed in a new line.

Question 3
Problem Statement: Implementation of ls(show list)
Approach:
1. Based on the second argument the input of the opendir() function call gets modified.
2. opendir function returns a dirstream pointer, this pointer contains the list files/directories that current directory contains.
3. readdir is used to iterate through each entity in the specified directory and based on the flags the structure of output changes
4. the return type of readdir is dirent structure pointer, from this pointer we print the filename when no flag is mentioned
5. By using filename we try to stat the file properties and print the detailed information about the contents of the specified directory when the flag is -l

Question 4
Problem Statement: Implement system commands as background/foreground process
Approach:
1. Using executevp and fork functions we process the input command.
2. If there is no ampersand parameter then using wait() function the parent process is suspended until execution of the child process.
3. If there is an ampersand parameter we will simply skip the wait() and print the process id of the child process

Question 5
Problem Statement: Print process related info using 'pinfo' command
Approach:
1. The process id of the current shell instance is fetched using getpid() method or pid is initialized to the parameter mentioned as argument
2. By accessing the /proc/<pid>/statm file we will retrieve the process info mentioned below.
3. Process status is present as the first argument of file and based on current pid and parent pid we can determine the foreground or background process to print '+' parameter.
4. Virtual Memory parameter in taken from line of the file and stored
5. The executable path is provided as symlink for the file /proc/<pid>/exe and displayed.

Question 6
Problem Statement: Search for a given file or folder under the current directory recursively.
Approach
1. The parameter for the search command is stored in a string so that it can be compared with files or directories.
2. Using opendir() function, returns a dirstream pointer, and this pointer contains the files/directories details.
3. Using readdir() function, the filename is retrieved from a dirent structure, and filename in this structure is compared with input parameter.
4. If the readdir() gives a directory then we are going the check the directory recursively using openddir() and readdir() functions.
5. The recursive function returns true once the filename is identified, but it iterates through every directory that is present in the parent directory if it is not found then returns false.

Question 7
Problem statement: Implement I/O redirection
Approach
1. Parse the input strings and tokenize the input line to individual strings
2. Check for redirection tokens (<, >, >>) and mark the respective flags to use them swapping file descriptors.
3. Create a duplicate file descriptor for the terminal, so that can be used as backup
4. Using dup2() function we will swap the file descriptors with terminal file descriptor.
5. Use Exec command to implement the write based on the remaining words
6. Again using dup2 function reverse the file descriptor with the backup file descriptor so that terminal is restored to normal state

Question 8
Problem Statement: Signal Handling for CTRL-Z, CTRL-C, CTRL-D
Approach:
1. Using signalhandling header file check for the input signal handling
2. Check for the child pid whenever there is a fork call and initialize to child pid
3. If there is a existing pid then kill the pid

Question 9
Problem Statement: Autocomplete for commands and all the files/directories under the current directory.
Approach:
1. Use rl_attempted_completion_function and readline to input the partial input and and searching for suggestions
2. Parse through the $PATH environment variable and try to check for the pattern matching with the given input
3. Do the similar approach for the contents in the current directory and store the available patterns in a string vector
4. Return the vector containing matched patterns to the gnu readline so that it will display in the new line.

Question 10
Problem statement: print the history of executed command in the shell
Approach:
1. Read the commands history using gnu readline add_history() function and read the command if there exists any from the history file stored in the home directory
2. Execute and store the commands that are given input from the user and when the user exits append the history data in the same file where the history is read
3. If buffer exceeds the limit then remove the oldest entry and print the elements
4. Do this for every command so that it is appended to the original file when exited from shell.