#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_SIZE 100

void repl() {
	while (true) {
		printf("$ ");
		char buffer[MAX_SIZE];
		if(fgets(buffer, MAX_SIZE, stdin)!=NULL){
			printf("%s", buffer);
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
