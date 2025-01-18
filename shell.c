#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX_SIZE 100
#define TOKEN_COUNT 10

int built_ins(char *command, const char *arg) {
	if(command==NULL) 
		return -1;

	if (strcmp(command,"ls")==0) {
		DIR *dir;
		struct dirent *entry;
		dir = opendir(".");
		if(dir==NULL) {
			perror("Unable to open directory");
			return 1;
		}
		
		while((entry= readdir(dir)) != NULL) {
			printf("%s\n", entry->d_name);
		}
	}
	else if (strcmp(command,"cd")==0) {
		char new_dir[40];
		if(arg==NULL)
			strncpy(new_dir, "home/rishabh", 40);
		else {
			strncpy(new_dir, arg, 40);
		}

		int status= chdir(new_dir);
		if(status!=0){
			perror("changing directory failed");
			return 1;
		}
		
	}
	else if (strcmp(command,"exit")==0) {
		return 2;
	}
	else if(strcmp(command,"help")==0) {
		printf("Steel Shell, version 0.1\n");
		printf("Following are the inbuilt commands of steelshell:\n");
		printf("1. ls: Lists files and directories in the current directory.\n");
		printf("2. cd arg: Switches current directory to arg\n");
		printf("3. exit: Exits the shell\n");
		printf("4. help: Prints out this message\n");
	}


	return 0;
}

char** parse(char *str) {
	char **tokens = (char **)malloc(TOKEN_COUNT*sizeof(char*));
	tokens[0]= strtok(str, " ");
	for (int i=1;; i++){
		if(tokens[i-1]==NULL)
			break;
		tokens[i] = strtok(NULL, " ");
	}

	return tokens;
}



void repl() {
	while (true) {
		printf("$ ");
		char buffer[MAX_SIZE];
		if(fgets(buffer, MAX_SIZE, stdin)!=NULL){
        		size_t len = strlen(buffer);
        		if (len > 0 && buffer[len - 1] == '\n') {
            			buffer[len - 1] = '\0';
        		}

			char **tokens;
			tokens = parse(buffer);
			if (tokens[0]==NULL)
				continue;

			if(strcmp(tokens[0],"ls")==0 ||
			   strcmp(tokens[0],"cd")==0 ||
			   strcmp(tokens[0],"exit")==0 ||
			   strcmp(tokens[0],"help")==0) {
				int val= built_ins(tokens[0], tokens[1]);
				if (val==2)
					break;
			}
		}
		else {
			if(feof(stdin))
				break;
			printf("command not found\n");
		}
	}
}


int main() {
	repl();
}
