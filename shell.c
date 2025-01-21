#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


#define MAX_SIZE 100
#define TOKEN_COUNT 10
#define INPUT_FD 0
#define OUTPUT_FD 1
int built_ins(char *command, const char *arg, char *infile, char *outfile) {
	if(command==NULL) 
		return -1;

	int saved_infd = dup(0);
	int saved_outfd = dup(1);
	int infileid;
	int outfileid;
	if(infile!=NULL || outfile!=NULL) {
		if(outfile != NULL) {
			if((outfileid= open(outfile, O_CREAT | O_RDWR, S_IRWXU))==-1) {
				perror("unable to open file");
				return -1;
			}
			dup2(outfileid, OUTPUT_FD);
		}
		if (infile != NULL) {
			if((infileid= open(infile, O_RDONLY | O_RDWR, S_IRWXU))==-1) {
				perror("unable to open file");
				return -1;
			}
			dup2(infileid, INPUT_FD);
		}
	}

	if (strcmp(command,"ls")==0) {
		DIR *dir;
		struct dirent *entry;
		dir = opendir(".");
		if(dir==NULL) {
			perror("Unable to open directory");
			return 1;
		}
		
		while((entry= readdir(dir)) != NULL) {
			printf("%s ", entry->d_name);
		}

		closedir(dir);
		printf("\n");
	}
	else if (strcmp(command,"cd")==0) {
		char new_dir[40];
		if(arg==NULL)
			strncpy(new_dir, "/home/rishabh", 40);
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

	if(infile!=NULL || outfile!=NULL){
		if(outfile!=NULL){
			dup2(saved_outfd, 1);
			close(outfileid);
		} 
		else if(infile!=NULL)  {
			dup2(saved_infd, 0);
			close(infileid);
		}
	}
	

	return 0;
}



int exec_user_program(char **args, size_t n, char *infile, char *outfile) {
	if(n>=TOKEN_COUNT){
		printf("no. of args exceeded\n");
		return -1;
	}

//	printf("n: %zu\n", n);
	args[n]= NULL;
	pid_t pid;
	if ((pid= vfork())<0){
		perror("vfork failed");
		return -1;
	}
	else if (pid==0) {
		if (infile!=NULL || outfile!=NULL){
			int infileid, outfileid;
			if(outfile!= NULL){
				if((outfileid = open(outfile, O_CREAT | O_RDWR, S_IRWXU))==-1){
					perror("failed to open file");
				}

				if(dup2(outfileid, 1)==-1){
					perror("Failed to duplicate the fd");
				}
				close(outfileid);
			}
			if(infile!=NULL){
				if((infileid = open(infile, O_CREAT | O_RDWR, S_IRWXU))==-1){
					perror("failed to open file");
				}
				
				if(dup2(infileid, 0)==-1){
					perror("Failed to duplicate the fd");
				}
				close(infileid);
			}
		}
		
//		for (int i =0; i<n; i++)
//			printf("%s ", args[i]);

		if(execv(args[0], args)==-1)
			perror("execv failed");
	}

	wait(NULL);
	return 0;
}

typedef struct{
	char **tokens;
	int size;
} token_pack;

token_pack parse(char *str) {
	char **tokens = (char **)malloc(TOKEN_COUNT*sizeof(char*));
	tokens[0]= strtok(str, " ");

	int n= 0;
	for (int i=1;; i++){
		if(tokens[i-1]==NULL)
			break;
		tokens[i] = strtok(NULL, " ");
		n++;
	}
	token_pack pack={tokens,n};
	return pack;

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

			char **args;
			token_pack unpack;
			unpack = parse(buffer);
			args= unpack.tokens;
			int n = unpack.size;


			char *infile= NULL;
			char *outfile= NULL;
			size_t newn= 1;
			int flag=0;	

			assert(n<TOKEN_COUNT);

			for (int i =0; i<n; i++){
				if(strcmp(args[i],">")==0){
					outfile= args[i+1];
					flag = 1;
				}
				else if(strcmp(args[i],"<")==0){
					infile = args[i+1];
					flag = 1;
				}

				if(flag==0)
					newn= i+1;
			}


			if (args[0]==NULL)
				continue;

			if(strcmp(args[0],"ls")==0 ||
			   strcmp(args[0],"cd")==0 ||
			   strcmp(args[0],"exit")==0 ||
			   strcmp(args[0],"help")==0) {
				int val= built_ins(args[0], args[1], infile, outfile);
				free(unpack.tokens);	
				if (val==2){
					break;
				}
			}
			else {
				int val= exec_user_program(args, newn, infile, outfile);
				free(unpack.tokens);	
				if(val==-1)
					printf("failed to execute the program\n");
			}
		}
		else {
			if(feof(stdin)){
				break;
			}
			printf("command not found\n");
		}

	}
}


int main() {
	repl();
}
