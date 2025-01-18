#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>



#define MAX_SIZE 100

int built_ins(char *command) {
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


	return 0;
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

			int val= built_ins(buffer);
			if (val==2)
				break;
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
