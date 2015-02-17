#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <malloc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

#define MAXLINE 1000
#define DEBUG 0
#define SHMSZ   1000

int fd[2];
volatile sig_atomic_t usr_interrupt = 0;

size_t allocSize = sizeof(char) * MAXLINE;

/* Function declarations and globals */
int parent_pid;
int child_status;
pid_t* bg_child;
int bg_current_child;

char ** tokenize(char*) ;
char ** get_input() ;
int execute_command(char** tokens) ;
void freeAllocatedMemory(char** tokens);
void executeCommand(char** thearg);
void signal_handler(int signal_number);
int sleepFor(int mins, int hrs);
void cronJob(char** tokens);

/*This function does the signal handling*/
void signal_handler(int signal_number) {
	if ( getpid() == parent_pid ) {}
	else {
		kill(getpid(),SIGKILL);
	}
}

void sigchild_handler(int signal_number) {
	pid_t cid;
	while(1) {
		cid = waitpid(-1,&child_status,WNOHANG);
		if (cid != 0 && cid != -1) {
			printf("JASH : Background Process Done, PID : %d\n$ ", (int) cid);
			fflush(stdout);
			if(WEXITSTATUS(child_status) == EXIT_FAILURE)
				{
					fprintf(stderr, "JASH : EXIT FAILURE \n$");
					fflush(stdout);
				}
				else
				{
					fprintf(stderr, "JASH : EXIT SUCCESS \n$");
					fflush(stdout);
				}
		}
		else {
			break;
		}
	}
	fflush(stdout);
	//sleep(1);
}

int main(int argc, char** argv){
	parent_pid = getpid(); 
	bg_current_child = 0;

	//int diff = sleepFor(22,23);
	//printf("%d\n",diff);

	/* Set (and define) appropriate signal handlers */
	/* Exact signals and handlers will depend on your implementation */
	signal(SIGINT, signal_handler);
	signal(SIGCHLD,sigchild_handler);

	char input[MAXLINE];
	char** tokens;
	
	while(1) {
		printf("$ "); // The prompt
		fflush(stdin);

		char *in = fgets(input, MAXLINE, stdin); // Taking input one line at a time
		//Checking for EOF
		if (in == NULL){
			if (DEBUG) printf("JASH: EOF found. Program will exit.\n");
			break ;
		}

		// Calling the tokenizer function on the input line    
		tokens = tokenize(input);	
		// Executing the command parsed by the tokenizer
		execute_command(tokens) ; 
		
		// Freeing the allocated memory	
		int i ;
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
	}
	
	/* Kill all Children Processes and Quit Parent Process */
	printf("\nJASH : Exiting JASH\n");
	return 0 ;
}

int execute_command(char** tokens) 
{
	/*Set debug mode on to get more print statements*/
	if(DEBUG)
	{
		printf("JASH Command:\n");
		int i ;
		for(i=0;tokens[i]!=NULL;i++)
		{
			printf("%s\n",tokens[i]);
		}
	}

	/* 
	Takes the tokens from the parser and based on the type of command 
	and proceeds to perform the necessary actions. 
	Returns 0 on success, -1 on failure. 
	*/
	if (tokens == NULL) 
	{
		return -1 ; 				// Null Command
	} 
	else if (tokens[0] == NULL) 
	{
		return 0 ;					// Empty Command
	} 
	else if (!strcmp(tokens[0],"exit")) 
	{
		/* Quit the running process */
		if (DEBUG) printf("JASH: This is a exit command\n");
		if ( bg_current_child != 0 ) {
		printf("\n");
		int i = 0;
		for (i = 0; i < bg_current_child; i++ ) {
			kill ( bg_child[i],SIGKILL);
			printf("JASH: Killed: %d\n",(int) bg_child[i]);
			}
		}
		free(bg_child);
		printf("\nJASH : Exiting JASH\n");
		kill(parent_pid,SIGKILL);
		return 0 ;
	} 
	else if (!strcmp(tokens[0],"cd"))
	{
		/* Change Directory, or print error on failure */
		if (DEBUG) printf("JASH: This is a cd command\n");
		/*If no path is specified*/
		if (tokens[1] == NULL) 
		{
			fprintf(stderr, "JASH: No path specified! %d\n", errno);
			fflush(stdout);
		}
		/*If there is a path specified, cd has a second argument*/
		else {
			/*If no such dorectory exists*/
			if ( chdir(tokens[1]) != 0 ) 
			{
				fprintf(stderr,"JASH : cd error %d ",errno);
				if (errno == ENOENT || errno == ENOTDIR) {
					fprintf(stderr, " : Directory does not exist \n");
					perror("Error");
					fflush(stdout);
					return -1;
				}
			}
			/*Success!*/
			else 
			{
				printf("JASH : Changed to %s\n",tokens[1]);
				fflush(stdout);
			}
		}
		return 0 ;
	} 
	else if (!strcmp(tokens[0],"parallel")) 
	{
		if (DEBUG) printf("JASH: This is a parallel command\n");
		/* Analyze the command to get the jobs */
		/* Run jobs in parallel, or print error on failure */
		/*Get commands in the form of tokens*/
		char ** sub_tokens;
		int c = 1;
		int count_childs = 0;
		while(1)
		{
			sub_tokens = (char **) malloc(MAXLINE*sizeof(char*));
			int index;
			/*Since we already have space separated tokens, all we need is to separate meaninful commands from :::s*/
			for(index=0; tokens[c] != NULL && strcmp(tokens[c], ":::") != 0;c++, index++)
			{
				sub_tokens[index] = (char*)malloc(MAXLINE*sizeof(char));
				strcpy(sub_tokens[index], tokens[c]);
			}
			/*Set the last element in the array as null, so as to mark the end of commands*/
			sub_tokens[index] = NULL;
			count_childs++;
			/*Fork a new child process for a parallel command*/
			int pid = fork();
			if (pid == 0) 
			{
				printf("> ");
				int i ;
				for(i=0;sub_tokens[i]!=NULL;i++)
				{
					printf("%s ",sub_tokens[i]);
					fflush(stdout);
				}
				printf("\n");
		    	fflush(stdout);
		    	execute_command(sub_tokens);
		    	exit (0);
		    }
		    /*If the child process creation fails*/
		    else if (pid == -1) 
		    {
				fprintf(stderr, "JASH: Child process creation failed %d", errno);
				perror("Error");
				fflush(stdout);
				return -1;
			}
			/*Freeing the allocated memory*/
			int i ;
			for(i=0;sub_tokens[i]!=NULL;i++)
			{
				free(sub_tokens[i]);
			}

			free(sub_tokens);
			if(tokens[c] == NULL) break;
			c++;
		}
		/* Parent Process */
		/* Wait for child processes to complete */
		int count =0;
		while(count!=count_childs)
		{
			waitpid(-1,NULL,0);
			count++;
		}
		return 0 ;	
	} 
	else if (!strcmp(tokens[0],"sequential")) 
	{
		if(DEBUG) printf("JASH: This is a sequential command\n");
		/* Analyze the command to get the jobs */
		/* Run jobs sequentially, print error on failure */
		/* Stop on failure or if all jobs are completed */
		char ** sub_tokens;
		int c=1;
		while(1)
		{
			/*Since we already have space separated tokens, all we need is to separate meaninful commands from :::s*/
			sub_tokens = (char **) malloc(MAXLINE*sizeof(char*));
			int index;
			for(index=0; tokens[c] != NULL && strcmp(tokens[c], ":::") != 0;c++, index++)
			{
				sub_tokens[index] = (char*)malloc(MAXLINE*sizeof(char));
				strcpy(sub_tokens[index], tokens[c]);
			}

			sub_tokens[index] = NULL;
			printf("> ");
			int s ;
			for(s=0;sub_tokens[s]!=NULL;s++)
			{
				printf("%s ",sub_tokens[s]);
				fflush(stdout);
			}
			printf("\n");
	    	fflush(stdout);
			if(execute_command(sub_tokens) == -1) 
			{
				return -1;
			}
			/*Freeing the allocated memory*/
			int i ;
			for(i=0;sub_tokens[i]!=NULL;i++)
			{
				free(sub_tokens[i]);
			}

			free(sub_tokens);
			if(tokens[c] == NULL) break;
			c++;
		}
		return 0 ;					// Return value accordingly
	}
	else if (!strcmp(tokens[0],"sequential_or")) 
	{
		if(DEBUG) printf("JASH: This is a sequential command\n");
		/* Analyze the command to get the jobs */
		/* Run jobs sequentially, print error on failure */
		/* Stop on failure or if all jobs are completed */
		char ** sub_tokens;
		int c=1;
		while(1){
			sub_tokens = (char **) malloc(MAXLINE*sizeof(char*));
			int index;
			/*Since we already have space separated tokens, all we need is to separate meaninful commands from :::s*/
			for(index=0; tokens[c] != NULL && strcmp(tokens[c], ":::") != 0;c++, index++)
			{
				sub_tokens[index] = (char*)malloc(MAXLINE*sizeof(char));
				strcpy(sub_tokens[index], tokens[c]);
			}

			sub_tokens[index] = NULL;
			printf("> ");
			int s ;
			for(s=0;sub_tokens[s]!=NULL;s++)
			{
				printf("%s ",sub_tokens[s]);
				fflush(stdout);
			}
			printf("\n");
	    	fflush(stdout);
	    	/*If any of the commands execute, return*/
			if(execute_command(sub_tokens) == 0) 
			{
				return 0;
			}
			// Freeing the allocated memory	
			int i ;
			for(i=0;sub_tokens[i]!=NULL;i++)
			{
				free(sub_tokens[i]);
			}

			free(sub_tokens);
			if(tokens[c] == NULL) break;
			c++;
		}
		return -1;					// Return value accordingly
	}else if(!strcmp(tokens[0],"cron")){
		FILE * fp;
		char * line = NULL;
		size_t len = 0;
		ssize_t read;

		/*We'll now try to open the file as read-only*/
		fp = fopen(tokens[1], "r");

		/*If file read wasn't succssful*/
		if (fp == NULL) 
		{
				fprintf(stderr,"JASH : Read File Failure %d\n",errno);
				perror("Error");
				fflush(stdout);
				exit(EXIT_FAILURE);
		}
		/*If file read WAS successful, call execute_command recursively*/
		else
		{
			char ** sub_tokens;
		    while ((read = getline(&line, &len, fp)) != -1) 
		    {
		    	sub_tokens = tokenize(line);
		    	printf("> %s", line);
		    	fflush(stdout);
		    	//execute_command(sub_tokens);
		    	int pid = fork();
				if (pid == 0) {
					cronJob(sub_tokens);
					break;
				}/*If the child process itself wasn't created*/
				else if (pid == -1) 
				{
					fprintf(stderr, "JASH: Child process creation failed %d", errno);
					perror("Error");
					fflush(stdout);
					return -1;
				}else{
					bg_current_child++;
					bg_child = (pid_t*) realloc(bg_child, sizeof(pid_t) * bg_current_child);
					bg_child[bg_current_child - 1] = pid;
				}
		    	
		    }
		}

		fclose(fp);
		if (line) free(line);
	}
	else 
	{
		/* Either file is to be executed or batch file to be run */
		/* Child process creation (needed for both cases) */
		int background = 0;
		int b;
		for ( b = 0; tokens[b] != NULL; b++ ){
			if(!strcmp(tokens[b],"&")) {
				background = 1;
				if(tokens[b+1]!=NULL){
					printf("& should be at the end\n");
					fflush(stdout);
					return -1;
				}
			}
		}
		int pid = fork();
		if (pid == 0) {
			if (!strcmp(tokens[0],"run")) 
			{
				if (DEBUG) printf("JASH: This is a run command\n");
				/* Locate file and run commands */
				/* May require recursive call to execute_command */
				/* Exit child with or without error */
				/*Initialising file pointer*/	
				FILE * fp;
				char * line = NULL;
				size_t len = 0;
				ssize_t read;

				/*We'll now try to open the file as read-only*/
				fp = fopen(tokens[1], "r");

				/*If file read wasn't succssful*/
				if (fp == NULL) 
				{
						fprintf(stderr,"JASH : Read File Failure %d\n",errno);
						perror("Error");
						fflush(stdout);
						exit(EXIT_FAILURE);
				}
				/*If file read WAS successful, call execute_command recursively*/
				else
				{
					char ** sub_tokens;
				    while ((read = getline(&line, &len, fp)) != -1) 
				    {
				    	sub_tokens = tokenize(line);
				    	printf("> %s", line);
				    	fflush(stdout);
				    	execute_command(sub_tokens);
				    }
				}

				fclose(fp);
				if (line) free(line);
				//exit(EXIT_SUCCESS);
				exit (0);
			}
			else 
			{
				if (DEBUG) printf("JASH: This is a executable command\n");
				/* File Execution */
				/* Print error on failure, exit with error*/
				int i;
				int pipeline = 0;
				/*An array of arguments, including the executable itself*/
				char** thearg;
				thearg = (char**) malloc (500 * sizeof(char*));
				char** thearg2;
				thearg2 = (char**) malloc (500 * sizeof(char*));
				for ( i = 0; tokens[i] != NULL; i++ ) 
				{
					if(strstr(tokens[i],"|") != NULL){
						//dup2(fd[1],1);
						pipeline = 1;
						int j=0;
						i++;
						for(;tokens[i] != NULL; i++){
							thearg2[j] = (char*) malloc (strlen(tokens[i]) * sizeof(char));
							strcpy(thearg2[j],tokens[i]);
							//printf("thearg2: %s \n", thearg2[j]);
							if(strstr(tokens[i],"&") != NULL) break;
							j++;
						}
						thearg2[j] = (char*) malloc (sizeof(char*));
						thearg2[j] = NULL;
						break;
					}
					if(strstr(tokens[i],"&") != NULL) break;
					thearg[i] = (char*) malloc (strlen(tokens[i]) * sizeof(char));
					strcpy(thearg[i],tokens[i]);
					//printf("thearg: %s \n", thearg[i]);
				}
				thearg[i] = (char*) malloc (sizeof(char*));
				thearg[i] = NULL;

				if(pipeline == 1){
					pipe(fd);
					int child_id_1 = fork();
					if (child_id_1 == 0) {
						dup2(fd[1],1);
						close(fd[0]);
						close(fd[1]);
						executeCommand(thearg);
					}
					else {
						int child_id_2 = fork();
						if (child_id_2 == 0) {
							dup2(fd[0],0);
							close(fd[0]);
							close(fd[1]);
							executeCommand(thearg2);
						}
						else {
							close(fd[0]);
							close(fd[1]);
							waitpid(child_id_2,&child_status,0);
						}
					}
				}else{
					executeCommand(thearg);
				}
			}
		} 
		/*If the child process itself wasn't created*/
		else if (pid == -1) 
		{
			fprintf(stderr, "JASH: Child process creation failed %d", errno);
			perror("Error");
			fflush(stdout);
			return -1;
		}
		else 
		{
			/* Parent Process */
			/* Wait for child process to complete */
			if(background == 0){
				int statusPtr;
				waitpid(pid,&statusPtr,0);
				if(WEXITSTATUS(statusPtr) == EXIT_FAILURE)
				{
					//printf("child exited with failure\n");
					//fflush(stdout);
					return -1;
				}
				else
				{
					return 0;
				}
			}	else {
				bg_current_child++;
				bg_child = (pid_t*) realloc(bg_child, sizeof(pid_t) * bg_current_child);
				bg_child[bg_current_child - 1] = pid;
				return 1;
			}
			
		}
	}
	return 1 ;
}

void executeCommand(char** thearg){
		//printf("JASH: command execution!\n");
		int pos_input = -1;
		int pos_output = -1;
		int pos_append = -1;
		int valid = 1;
		char** exearg;
		exearg = (char**) malloc (500 * sizeof(char*));
		int p;
		int c;
		for(p=0, c=0;thearg[p]!=NULL;p++){
			if(strstr(thearg[p],">>") != NULL){
				if(pos_append != -1) {
					printf("JASH: Invalid command, multiple >>\n"); fflush(stdout); valid = 0; break;}
				else {pos_append = p; p++; continue;}
			}else if(strstr(thearg[p],">") != NULL){
				if(pos_output != -1) {
					printf("JASH: Invalid command, multiple >\n"); fflush(stdout); valid = 0; break;}
				else {pos_output = p; p++; continue;}
			}else if(strstr(thearg[p],"<") != NULL){
				if(pos_input != -1) {
					printf("JASH: Invalid command, multiple <\n"); fflush(stdout); valid = 0; break;}
				else {pos_input = p; p++; continue;}
			}
			//printf("JASH: cmd: %d \n",p);
			exearg[c] = (char*) malloc (strlen(thearg[p]) * sizeof(char));
			strcpy(exearg[c],thearg[p]);
			c++;
		}
		exearg[c] = (char*) malloc (sizeof(char*));
		exearg[c] = NULL;

		if(pos_output != -1 && pos_append != -1){
			printf("JASH: Invalid command,> and >> cannot exist together\n");
			fflush(stdout);
			valid = 0;
		}

		//printf("Positions: < %d > %d >> %d \n", pos_input, pos_output, pos_append);

		if(pos_input != -1 && valid == 1){

			int inpfd = open(thearg[pos_input+1],O_RDONLY);
			if (inpfd != -1 ) {
				dup2(inpfd,0);
			}
			else {
				fprintf(stderr,"JASH : Error input file %s not found\n",thearg[pos_input+1]);
				perror("Error");
				fflush(stdout);
				valid = 0;
			}
		}

		if(pos_output != -1 && valid == 1){

			int outfd = open(thearg[pos_output+1],O_CREAT|O_TRUNC|O_WRONLY, 0644);
			if (outfd != -1 ) {
				dup2(outfd,1);
			}
			else {
				fprintf(stderr,"JASH : Error Unable to create file %s\n",thearg[pos_output+1]);
				perror("Error");
				fflush(stdout);
				valid = 0;
			}
		}

		if(pos_append != -1 && valid == 1){

			int appfd = open(thearg[pos_append+1],O_WRONLY | O_APPEND | O_CREAT, 0644);
			if (appfd != -1 ) {
				dup2(appfd,1);
			}
			else {
				fprintf(stderr,"JASH : Error Unable to append to file %s\n",thearg[pos_append+1]);
				perror("Error");
				fflush(stdout);
				valid = 0;
			}
		}

		//printf("executing command\n");
		/*Try executing the command*/
		if(valid == 0 || execvp(exearg[0], exearg) != 0)
		{
			fprintf(stderr, "JASH: Error executing command %s %d\n", thearg[0], errno);
			if(valid == 1) perror("Error");
			fflush(stdout);
			freeAllocatedMemory(thearg);
			freeAllocatedMemory(exearg);
			exit(EXIT_FAILURE);
		}
		//if()
		freeAllocatedMemory(thearg);
		freeAllocatedMemory(exearg);
		exit(0);
		return;	
}

void cronJob(char** tokens){
	//time_t start = time(NULL);
	//struct tm *tm_p;
    //tm_p = localtime( &start );
    //printf("The time is %.2d:%.2d:%.2d\n", tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec );
	//printf("CronJob %s\n", tokens[2]);
	int mins, hrs;

	if(!strcmp(tokens[0],"*")) mins = -1;
	else{
		mins = atoi(tokens[0]);
	}

	if(!strcmp(tokens[1],"*")) hrs = -1;
	else{
		hrs = atoi(tokens[1]);
	}
	int i;
	char** thearg;
	thearg = (char**) malloc (500 * sizeof(char*));
	for (i = 2; tokens[i] != NULL; i++ ) 
	{
		thearg[i-2] = (char*) malloc (strlen(tokens[i]) * sizeof(char));
		strcpy(thearg[i-2],tokens[i]);
		//printf("thearg: %s \n", thearg[i-2]);
	}
	thearg[i-2] = (char*) malloc (sizeof(char*));
	thearg[i-2] = NULL;
	//execute_command(thearg);
	
	while(1){
		int sleepfor = sleepFor(mins, hrs);
		fflush(stdout);
		printf("CronJob %s is scheduled afer %d seconds\n", thearg[0],sleepfor);
		fflush(stdout);
		printf("$ ");
		fflush(stdout);
		sleep(sleepfor);
		execute_command(thearg);
	}
	
}

int sleepFor(int mins, int hrs){
	time_t start = time(NULL);
	struct tm *tm_p;
    tm_p = localtime( &start );
    //printf("The time is %.2d:%.2d:%.2d\n", tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec );

	if(mins == -1 && hrs == -1){
		//every minute of every hour
		return (60-tm_p->tm_sec);
	}
	else if(mins == -1){
		//every minute of a particular hour
		if(hrs == tm_p->tm_hour && tm_p->tm_min != 59){
			return (60-tm_p->tm_sec);
		}else if(hrs == tm_p->tm_hour && tm_p->tm_min == 59){
			return ((23)*60*60+60-tm_p->tm_sec);
		}
		else if(hrs > tm_p->tm_hour){
			return ((hrs-tm_p->tm_hour)*60*60-tm_p->tm_sec);
		}else{
			return ((24+hrs-tm_p->tm_hour)*60*60-tm_p->tm_sec);
		}
	}else if(hrs == -1){
		//execute every hour
		if(mins == tm_p->tm_min){
			return (60*60-tm_p->tm_sec);
		}else if(mins > tm_p->tm_min){
			return ((mins-tm_p->tm_min)*60-tm_p->tm_sec);
		}else{
			return ((60+mins-tm_p->tm_min)*60-tm_p->tm_sec);
		}
	}else{
		//execute every hour
		if(hrs == tm_p->tm_hour){
			if(mins == tm_p->tm_min){
				return (24*60*60-tm_p->tm_sec);
			}else if(mins > tm_p->tm_min){
				return ((mins-tm_p->tm_min)*60-tm_p->tm_sec);
			}else{
				return ((24*60+mins-tm_p->tm_min)*60-tm_p->tm_sec);
			}
		}else if(hrs > tm_p->tm_hour){
			if(mins == tm_p->tm_min){
				return ((hrs-tm_p->tm_hour)*60*60-tm_p->tm_sec);
			}else if(mins > tm_p->tm_min){
				return ((hrs-tm_p->tm_hour)*60*60+(mins-tm_p->tm_min)*60-tm_p->tm_sec);
			}else{
				return ((hrs-tm_p->tm_hour)*60*60+(mins-tm_p->tm_min)*60-tm_p->tm_sec);
			}
		}else{
			if(mins == tm_p->tm_min){
				return ((24+hrs-tm_p->tm_hour)*60*60-tm_p->tm_sec);
			}else if(mins > tm_p->tm_min){
				return ((24+hrs-tm_p->tm_hour)*60*60+(mins-tm_p->tm_min)*60-tm_p->tm_sec);
			}else{
				return ((24+hrs-tm_p->tm_hour)*60*60+(mins-tm_p->tm_min)*60-tm_p->tm_sec);
			}
		}
		
	}
}

void freeAllocatedMemory(char** tokens){
	// Freeing the allocated memory	
	int i ;
	for(i=0; tokens[i]!=NULL;i++)
	{
		free(tokens[i]);
	}

	free(tokens);
}

/*The tokenizer function takes a string of chars and forms tokens out of it*/
char ** tokenize(char* input){
	int i, doubleQuotes = 0, tokenIndex = 0, tokenNo = 0 ;
	char *token = (char *)malloc(MAXLINE*sizeof(char));
	char **tokens;

	tokens = (char **) malloc(MAXLINE*sizeof(char*));

	for(i =0; i < strlen(input); i++){
		char readChar = input[i];

		if (readChar == '"'){
			doubleQuotes = (doubleQuotes + 1) % 2;
			if (doubleQuotes == 0){
				token[tokenIndex] = '\0';
				if (tokenIndex != 0){
					tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
					strcpy(tokens[tokenNo++], token);
					tokenIndex = 0; 
				}
			}
		} else if (doubleQuotes == 1){
			token[tokenIndex++] = readChar;
		} else if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0; 
			}
		} else {
			token[tokenIndex++] = readChar;
		}
	}

	if (doubleQuotes == 1){
		token[tokenIndex] = '\0';
		if (tokenIndex != 0){
			tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
			strcpy(tokens[tokenNo++], token);
		}
	}

	tokens[tokenNo] = NULL ;
	return tokens;
}