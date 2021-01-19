#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <pwd.h>

#define MAX_LIMIT 200

int sh_cd(char *arg) {
	char s[100];
	if(arg == NULL || strcmp(arg, "~") == 0) {
		struct passwd *p = getpwuid(getuid());
		const char *homedir = p->pw_dir;
		if(chdir(homedir)){
			printf("Can't change to home directory\n");
		}
	}
	else {
		if(chdir(arg)){
			printf("Can't change to this directory.\n");
		}
	}
	printf("CWD: %s\n", getcwd(s, 100));
	return 1;
}

int sh_exit(char *arg) {
	if(arg == NULL) {
		return 0;
	}
	else if(strcmp(arg, "--help") == 0){
		printf("exit: exit [n]\n");
     	printf("Exit the shell.\n\n");
    	printf("Exits the shell with a status of N.  If N is omitted, the exit status\n");
    	printf("is that of the last command executed.\n");
    	return 1;
	}
	return atoi(arg);
}

int sh_pwd(char *arg) {
	char s[100];
	if(arg == NULL || strcmp(arg, "-P")) {
		printf("%s", getcwd(s, 100));
	}
	printf("\n");
	return 1;
}
void print_input(char *str) {
	char *token = strtok(str, " \"\'");
	while(token != NULL){
		printf("%s ", token);
		token = strtok(NULL, " \"\'");
	}
}

int countChars( char* s, char c)
{
	if(*s == '\0') {
		return 0;
	}
	return countChars(s + 1, c) + (*s == c);
}


int sh_echo(char *arg, char *arg2) {
	if(arg == NULL){
		printf("\n");
		return 1;
	}
	int i = countChars(arg, '"');
	int j = countChars(arg, '\'');
	if(i % 2 == 0 && j % 2 == 0) {
		if(arg2 == NULL || strcmp(arg2, "-E") == 0){
			print_input(arg);
			printf("\n");
		}
		else{
			print_input(arg);
		}
	}
	else{
		printf("Input mismatch.\n");
	}
	return 1;
}

void write_to_history(char *line) {
	FILE *file = fopen("history.txt", "a+");
	if(file == NULL) {
		printf("Couldn't open file.\n");
	}
	else{
		fputs(line, file);
		fclose(file);
	}
}

int sh_get_history(char *arg){
	FILE *file;
	char line[MAX_LIMIT];
	file = fopen("history.txt", "r");
	if(file == NULL){
		printf("File not present.\n");
	}
	if(arg == NULL){
		int i = 1;
		while(fscanf(file, "%[^\n]\n", line) != EOF) {
			printf("%d. %s\n", i, line);
			i++;
		}
	}
	else if(strcmp(arg, "!!") == 0){
		int i = 1;
		while(fscanf(file, "%[^\n]\n", line) != EOF) {
			i++;
		}
		i-=2;
		fclose(file);
		file = fopen("history.txt", "r");
		int j = 1;
		while(fscanf(file, "%[^\n]\n", line) != EOF) {
			if(i == j){
				printf("%d. %s\n", i, line);
			}
			j++;
		}
	}
	else{
		int num = atoi(arg);
		int i = 1;
		while(fscanf(file, "%[^\n]\n", line) != EOF) {
			if(i == num){
				printf("%d. %s\n", i, line);
			}
			i++;
		}
		
	}
	fclose(file);
	return 1;;
}

int main(int argc, char *argv[]) {
	pid_t pid;
	int return_value;
	int call_internal = 1;

	char s[MAX_LIMIT];
	printf("%s-> ", getcwd(s, MAX_LIMIT));
	char line[MAX_LIMIT];
	fgets(line, MAX_LIMIT, stdin);
	write_to_history(line);

	char *token = strtok(line, " \n");
	char *command = token;

	if(command == NULL){
		return_value = 1;
	}
	else{
		token = strtok(NULL, " \n");
		char *arg = token;

		char *arg2;
		if(token != NULL) {
			token = strtok(NULL, " \n");
			arg2 = token;
		}

		if(strcmp(command, "cd") == 0) {
			return_value = sh_cd(arg);
		}
		else if(strcmp(command, "exit") == 0) {
			return_value = sh_exit(arg);
		}
		else if(strcmp(command, "pwd") == 0) {
			return_value = sh_pwd(arg);
		}
		else if(strcmp(command, "echo") == 0) {
			return_value = sh_echo(arg, arg2);
		}
		else if(strcmp(command, "history") == 0) {
			return_value = sh_get_history(arg);
		}
		else {
			call_internal = 0;
			pid = fork();
			if(pid < 0) {
				perror("fork failed.\n");
			}
			else if(pid == 0) {
				//child process execs
				execl("./commands", "./commands", command, arg, arg2, NULL);
			}
		}
	}
	//parent process
	int status;
	pid_t childpid;
	int childReturnValue;
	if(call_internal == 0) {
		childpid = wait(&status);
		childReturnValue = WEXITSTATUS(status);
	}

	if((call_internal == 1 && return_value != 0) || (call_internal ==0 && childReturnValue != 0)){
		execvp(argv[0], argv);
	}
	return 0;
}

