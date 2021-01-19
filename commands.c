#include <stdio.h>
#include <stdlib.h>	
#include <unistd.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

extern int alphasort();

int flag_ls_A(const struct direct *file) {
	if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
		return 0;
	}
	return 1;
}

int flag_ls(const struct direct *file) {
	return 1;
}

int flag_ls_a(const struct direct *file) {
	if(strcmp(file->d_name, "..") == 0) {
		return 0;
	}
	return 1;
}

int sh_ls(char *arg) {
	char path[200];
	struct direct **file;

	if(!getcwd(path, 200)) {
		perror("Error: couldn't get path\n");
		return 1;
	}
	int c;
	if(arg == NULL){
		c = scandir(path, &file, flag_ls, alphasort);
	}
	else if(strcmp(arg, "-a") == 0){
		c = scandir(path, &file, flag_ls_a, alphasort);
	}
	else{
		c = scandir(path, &file, flag_ls_A, alphasort);
	}
	if(c <= 0){
		perror("Files not found. \n");
		return 1;
	}

	for(int i=0; i<c; i++) {
		printf("%s   ", file[i]->d_name);
	}
	printf("\n");
	return 1;
}

int sh_cat(char *arg, char *arg2) {
	printf("%s\n", arg);
	FILE *file;
	char line[200];
	file = fopen(arg, "r");
	if(file == NULL){
		printf("File not present\n");
		return 1;
	}
	if(arg2 == NULL){
		while(fscanf(file, "%[^\n]\n", line) != EOF) {
			printf("%s\n", line);
		}
	}
	else if(strcmp(arg2, "-n") == 0) {
		int i = 1;
		while(fscanf(file, "%[^\n]\n", line) != EOF) {
			printf("%d. %s\n", i, line);
			i++;
		}
	}
	else{
		while(fscanf(file, "%[^\n]\n", line) != EOF) {
			printf("%s $\n", line);
		}
	}
	fclose(file);
	return 1;
}

int sh_date(char *arg) {
	if(arg != NULL) {
		if(strcmp(arg, "-u") == 0) {
			system("date -u");
		}
		else if(strcmp(arg, "-I") == 0) {
			system("date -I");
		}
	}
	else {
		system("date");
	}
	return 1;
}

int sh_rm(char *arg, char *arg2) {
	if(arg == NULL) {
		printf("Missing operand.\n");
	}
	else {
		if (arg2 == NULL){
			if(remove(arg) ){
				printf("rm: cannot remove %s: No such file or directory\n", arg);
			}
		}
		else if(strcmp(arg2, "-i") == 0){
			char ans;
			printf("rm: remove 'b.txt'? [Y/n] ");
			scanf("%c", &ans);
			if(ans == 'Y'){
				if(remove(arg) ){
					printf("rm: cannot remove %s: No such file or directory\n", arg);
				}
			}
		}
		else if(strcmp(arg2, "-f") == 0){
			remove(arg);
		}
	}
	return 1;

}

int sh_mkdir(char *arg, char *arg2) {
	if(arg == NULL) {
		printf("mkdir: missing operand\n");
	}
	else{
		if(strcmp(arg, "--help") == 0){
			printf("Usage: mkdir [OPTION]... DIRECTORY...\n");
			printf("Create the DIRECTORY(ies), if they do not already exist. \n\n");
			printf("Mandatory arguments to long options are mandatory for short options too.\n");
  			printf("-m, --mode=MODE   set file mode (as in chmod), not a=rwx - umask\n");
			printf("-p, --parents     no error if existing, make parent directories as needed\n");
			printf("-v, --verbose     print a message for each created directory\n");
			printf("-Z                   set SELinux security context of each created directory\n");
			printf(	"					 to the default type\n");
			printf( "  	--context[=CTX]  like -Z, or if CTX is specified then set the SELinux\n");
			printf( "                      or SMACK security context to CTX\n");
			printf( "  	--help     display this help and exit\n");
			printf( "  	--version  output version information and exit\n");		
		}
		else if(arg2 != NULL && strcmp(arg2, "-v") == 0) {
			if(mkdir(arg, (S_IRWXG | S_IRWXU)) == 0) {
				printf("mkdir: created directory %s\n", arg);
			}
		}
		else{
			if(mkdir(arg, (S_IRWXG | S_IRWXU))) {
				printf("mkdir: cannot make directory %s\n: File exists", arg);
			}
		}
	}
	return 1;
}


int main(int argc, char *argv[]) {
	int return_value;
	char *command = argv[1];
	char *arg = argv[2];
	char *arg2 = argv[3];

	if(strcmp(command, "ls") == 0) {
		return_value = sh_ls(arg);
	}
	else if(strcmp(command, "cat") == 0) {
		return_value = sh_cat(arg, arg2);
	}
	else if(strcmp(command, "date") == 0) {
		return_value = sh_date(arg);
	}
	else if(strcmp(command, "rm") == 0) {
		return_value = sh_rm(arg, arg2);
	}
	else if(strcmp(command, "mkdir") == 0) {
		return_value = sh_mkdir(arg, arg2);
	}
	else{
		return_value = 1;
	}


	return return_value;
}