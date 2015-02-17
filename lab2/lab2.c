#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

// #include <malloc.h>
#define MAXLINE 1000
#define SHMSZ   1000

char domainChildName[50];
typedef struct{
	char userName[100];
} userList;

userList userNames[100];
int userNamesCount = 0;

int shmid;
key_t key;
char *shm, *s;

/* When a SIGUSR1 signal arrives, set this variable. */
volatile sig_atomic_t usr_interrupt = 0;
volatile sig_atomic_t process_interrupt = 0;
//declarations
char ** tokenize(char*);
char ** get_input() ;

/*A domain type*/
typedef struct
{
	char domain_name[50];
	int p_id;
} domain;

/*Array of domains*/
domain domainList[100];

/*Number of domains in the domain array*/
int domainNo = 0;

char* tokens;

int find_user(char* userName)
{
	int i;
	for (i = 0; i < userNamesCount; i++)
	{
		if(strcmp(userNames[i].userName, userName)==0)
		{
			return i;
		}
	}
	return -1;
}

/*Function to add a new domain*/
int add_user(char* userName)
{
	userList temp;
	strcpy(temp.userName,userName);
	userNames[userNamesCount] = temp;
	userNamesCount++;
}

int a2i(const char *s)
{
	//printf("%s\n", s);
   char * pEnd;
  long int l1;
  l1 = strtol (s,&pEnd,10);
  return l1;
}

void childProcessData(){
	/*
    for (s = shm; *s != NULL; s++){
        putchar(*s);
    }

    putchar('\n');
    */
    char* token1 = strtok(shm," ");
    char* token2 = strtok(NULL," ");
    char* token3 = strtok(NULL," ");
    if(strcmp(token1,"delete_domain") == 0){
    	process_interrupt = 1;
    	kill(getppid(), SIGUSR1);
    	exit(0);
    }
    else if(strcmp(token1,"add_email") == 0){
    	//printf("add child instruction!: %s %s %s\n",token1, token2, token3);
    	if(find_user(token2) == -1) {
    		add_user(token2);
	    	char finalString[500] = "";
		    strcat(finalString, "Child Process ");
		    strcat(finalString, token3);
		    strcat(finalString, " - Email address ");
		    strcat(finalString, token2);
		    strcat(finalString, "@");
		    strcat(finalString, token3);
		    strcat(finalString, " added successfully.\n");
		    sprintf(shm, "%s", finalString);
		}else{
			char finalString[500] = "";
		    strcat(finalString, "Child Process ");
		    strcat(finalString, token3);
		    strcat(finalString, " - Email address already exists\n");
		    sprintf(shm, "%s", finalString);
		}
    }else if(strcmp(token1,"search_email") == 0){
    	//printf("search child instruction!: %s %s %s\n",token1, token2, token3);
    	int f = find_user(token2);
    	if(f == -1) {
	    	char finalString[500] = "";
		    strcat(finalString, "Parent process - could not find the email address ");
		    strcat(finalString, token2);
		    strcat(finalString, "@");
		    strcat(finalString, token3);	
		    strcat(finalString, ".\n");
		    sprintf(shm, "%s", finalString);
		}else{
			char finalString[500] = "";
		    strcat(finalString, "Parent process - found the email address ");
		    strcat(finalString, token2);
		    strcat(finalString, "@");
		    strcat(finalString, token3);	
		    strcat(finalString, " at ");
		    char tempStr1[10];
			sprintf(tempStr1, "%d", f);
		    strcat(finalString, tempStr1);
		    strcat(finalString,"\n");
		    sprintf(shm, "%s", finalString);
		}
    }else if(strcmp(token1,"delete_email") == 0){
    	//printf("search child instruction!: %s %s %s\n",token1, token2, token3);
    	int f = find_user(token2);
    	if(f == -1) {
	    	char finalString[500] = "";
		    strcat(finalString, "Parent process - child ");
		    strcat(finalString, token3);
		    strcat(finalString, " could not find the email address ");
		    strcat(finalString, token2);
		    strcat(finalString, "@");
		    strcat(finalString, token3);	
		    strcat(finalString, ".\n");
		    sprintf(shm, "%s", finalString);
		}else{
			//deleting
			userList *temp = (userList*)malloc(sizeof(userList));
			userNames[f] = *temp;

			char finalString[500] = "";
		    strcat(finalString, "Child process - child ");
		    strcat(finalString, token3);
		    strcat(finalString, " deleted ");
		    strcat(finalString, token2);
		    strcat(finalString, "@");
		    strcat(finalString, token3);	
		    strcat(finalString, " from position ");
		    char tempStr1[10];
			sprintf(tempStr1, "%d", f);
		    strcat(finalString, tempStr1);
		    strcat(finalString,"\n");
		    sprintf(shm, "%s", finalString);
		}
    }else if(strcmp(token1,"check_user") == 0){
    	//printf("search child instruction!: %s %s %s\n",token1, token2, token3);
    	int f = find_user(token2);
    	if(f == -1) {
	    	char finalString[500] = "";
		    sprintf(shm, "%s", finalString);
		}else{
			char finalString[500] = "";
		    strcat(finalString, token2);
		    sprintf(shm, "%s", finalString);
		}
    }else if(strcmp(token1,"send_email") == 0){
    	char* token4 = strtok(NULL," ");
    	char* token5 = strtok(NULL," ");
    	printf("Child Process - Email sent from %s@%s to %s@%s\n",token2, domainChildName, token4, token5);
    	int receiver_pid = a2i( token3 );
    	
    	char finalString[500] = "";
	    strcat(finalString, "receive_email");
	    strcat(finalString, " ");
	    strcat(finalString, token2);
	    strcat(finalString, " ");
	    strcat(finalString, token4);
	    strcat(finalString, "\n");
	    //printf("%s\n",finalString);

    	sprintf(shm,"%s",finalString);
		//printf("%d\n",receiver_pid);
		kill(receiver_pid, SIGUSR2);
    }else if(strcmp(token1,"receive_email") == 0){
    	//printf("hello\n");
    	//printf("%s\n",shm);
    	printf("Child Process - Email received at process %d for user %s\n",getpid(), token3);
    	return;
    	//printf("%d\n",getppid());
    }

    kill(getppid(), SIGUSR1);
}

void childProcess(void) {
	
	//printf("new child1 ");
	/*
    for (s = shm; *s != NULL; s++){
        putchar(*s);
    }

    putchar('\n');
    */
    char* token1 = strtok(shm," ");
    char* token2 = strtok(NULL," ");
    char* token3 = strtok(NULL," ");

    add_user(token2);

    //printf("child instruction!: %s %s %s\n",token1, token2, token3);
    sprintf(domainChildName, "%s", token3);
    //printf("child domain!: %s\n",domainChildName);
    char finalString[500] = "";//="Child process "+token3+" - Email address "+token2+"@"+token3+" added successfully.";
    strcat(finalString, "Child Process ");
    strcat(finalString, token3);
    strcat(finalString, " - Email address ");
    strcat(finalString, token2);
    strcat(finalString, "@");
    strcat(finalString, token3);
    strcat(finalString, " added successfully.\n");
    sprintf(shm, "%s", finalString);
    //puts(shm);
	kill(getppid(), SIGUSR1);
	//printf("%d\n\n",getppid());
	while(1){
		if(process_interrupt == 1) break;
	};
	return;
}

void sig_usr(int signo){
	if(signo == SIGUSR1) usr_interrupt = 1;
	else if(signo == SIGUSR2) {
		childProcessData();
	}
}

int main(void) {
	//printf("parent process: %d\n",getpid());
	pid_t ppid;                    
    ppid = getpid();   

    char c;
    /*
    int shmid;
    key_t key;
    char *shm, *s;
	*/

    //We'll name our shared memory segment
    key = ppid;

    //Create the segment.
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    
    //Now we attach the segment to our data space.
    
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    s = shm;

    for (c = 'a'; c <= 'z'; c++)
        *s++ = c;
    *s = NULL;
    
	  struct sigaction usr_action;
	  sigset_t block_mask;
	  pid_t child_id;

	  /* Establish the signal handler. */
	  sigfillset (&block_mask);
	  usr_action.sa_handler = sig_usr;
	  usr_action.sa_mask = block_mask;
	  usr_action.sa_flags = 0;
	  sigaction (SIGUSR1, &usr_action, NULL);
	  sigaction (SIGUSR2, &usr_action, NULL);

    char ** tokens;

    while(1){
    	tokens = get_input();
    	if(tokens != NULL){
    		//printf("instruction: %s %s\n",tokens[0], tokens[1]);
    		if(strcmp(tokens[0],"send_email")==0){
    			int user1 = 0;
    			int user2 = 0;
    			int pid_sender, pid_receiver;
    			int d;
		    	for(d=0; d<domainNo;d++){
		    		if(strcmp(domainList[d].domain_name,tokens[2])==0){
		    			//domain exists
		    			sprintf(shm,"%s %s %s","check_user",tokens[1],tokens[2]);
		    			kill(domainList[d].p_id, SIGUSR2);
		    			while(!usr_interrupt);
						//printf("Signal processed OKay1\n");
						usr_interrupt = 0;
						char* token1 = strtok(shm," ");
						if(strcmp(token1,tokens[1])==0) user1 = 1;
						//printf("%s\n",shm);
						pid_sender = domainList[d].p_id;
		    		}
		    		if(strcmp(domainList[d].domain_name,tokens[4])==0){
		    			//domain exists
		    			sprintf(shm,"%s %s %s","check_user",tokens[3],tokens[4]);
		    			kill(domainList[d].p_id, SIGUSR2);
		    			while(!usr_interrupt);
						//printf("Signal processed OKay1\n");
						usr_interrupt = 0;
						char* token1 = strtok(shm," ");
						if(strcmp(token1,tokens[3])==0) user2 = 1;
						//printf("%s\n",shm);
						pid_receiver = domainList[d].p_id;
		    		}
		    		if(user1 == 1 && user2 ==1) break;
	    		}
	    		if(user1 != 1 || user2 !=1){
	    			printf("Parent process - The sender/receiver does not exist.\n");
	    			continue;
	    		}else{
	    			printf("Parent process - The sender/receiver both exist. %d %d\n",pid_sender, pid_receiver);
	    			sprintf(shm,"%s %s %d %s %s",tokens[0],tokens[1],pid_receiver,tokens[3],tokens[4], tokens[5]);
	    			kill(pid_sender, SIGUSR2);
	    			
					while(!usr_interrupt);
					//printf("Signal processed OKay1\n");
					usr_interrupt = 0;
					//printf("done\n");
	    		}
	    	}
    		if(strcmp(tokens[0],"delete_email")==0){
    			
    			int d;
		    	for(d=0; d<domainNo;d++){
		    		if(strcmp(domainList[d].domain_name,tokens[2])==0){
		    			//domain exists
		    			sprintf(shm,"%s %s %s",tokens[0],tokens[1],tokens[2]);
		    			kill(domainList[d].p_id, SIGUSR2);
		    			while(!usr_interrupt);
						//printf("Signal processed OKay1\n");
						usr_interrupt = 0;
						printf("%s\n",shm);
		    			break;
		    		}
	    		}
	    		if(d==domainNo){
	    			printf("Parent process - Domain does not exist.\n");
	    			continue;
	    		}
	    	}
    		else if(strcmp(tokens[0],"delete_domain")==0){
    			int d;
		    	for(d=0; d<domainNo;d++){
		    		if(strcmp(domainList[d].domain_name,tokens[1])==0){
		    			//domain exists
		    			sprintf(shm,"%s %s %s",tokens[0],"dummy",tokens[1]);
		    			kill(domainList[d].p_id, SIGUSR2);
		    			int count =0;
						while(count!=1){
							waitpid(-1,NULL,0);
							count++;
						}
		    			while(!usr_interrupt);
						//printf("Signal processed OKay1\n");
						usr_interrupt = 0;
						printf("Parent process - Domain %s with PID - %d deleted.\n",domainList[d].domain_name, domainList[d].p_id);
						domain *newdomain = (domain*)malloc(sizeof(domain));
						domainList[d] = *newdomain;
		    			break;
		    		}
	    		}
	    		if(d==domainNo){
	    			printf("Parent process - Domain does not exist.\n");
	    			continue;
	    		}
    		}
    		else if(strcmp(tokens[0],"search_email")==0){
    			
    			int d;
		    	for(d=0; d<domainNo;d++){
		    		if(strcmp(domainList[d].domain_name,tokens[2])==0){
		    			//domain exists
		    			sprintf(shm,"%s %s %s",tokens[0],tokens[1],tokens[2]);
		    			kill(domainList[d].p_id, SIGUSR2);
		    			while(!usr_interrupt);
						//printf("Signal processed OKay1\n");
						usr_interrupt = 0;
						printf("%s\n",shm);
		    			break;
		    		}
	    		}
	    		if(d==domainNo){
	    			printf("Parent process - Domain does not exist.\n");
	    			continue;
	    		}
    		}else if(strcmp(tokens[0],"add_email")==0){
    			int d;
	    	for(d=0; d<domainNo;d++){
	    		if(strcmp(domainList[d].domain_name,tokens[2])==0){
	    			//domain already exists
	    			//printf("domain already exist %d\n\n",domainList[d].p_id);
	    			sprintf(shm,"%s %s %s",tokens[0],tokens[1],tokens[2]);
	    			kill(domainList[d].p_id, SIGUSR2);
	    			while(!usr_interrupt);
					//printf("Signal processed OKay\n");
					usr_interrupt = 0;
					printf("%s\n",shm);
	    			break;
		    		}
		    	}

		    	if(d==domainNo){
		    		//domain doesn't exist
		    		//printf("domain doesn't exist\n\n");
		    		domain *newdomain = (domain*)malloc(sizeof(domain));
		    		strncpy(newdomain->domain_name, tokens[2], 25);
		    		domainList[domainNo] = *newdomain;
		    		int newpid = fork();
		    		domainList[domainNo].p_id = newpid;
		    		domainNo++;
		    		sprintf(shm,"%s %s %s",tokens[0],tokens[1],tokens[2]);
		    		if ( newpid == 0 ) {
						//printf("child1: %d\n",getpid());
						childProcess();
						//printf("exiting child1\n");
						exit(0);
					}	
					while(!usr_interrupt);
					//printf("Signal processed OKay\n");
					usr_interrupt = 0;
					printf("%s\n",shm);
		    	}
    		}	    	
			
	    	// Freeing the allocated memory	
			int i;
			for(i=0;tokens[i]!=NULL;i++){
				free(tokens[i]);
			}
			free(tokens);
    	}else{
    		// no more input
    		break;
    	}
    	
    }
	
	int count =0;
	while(count!=domainNo){
		waitpid(-1,NULL,0);
		count++;
	}

	return 0;
}

char ** tokenize(char* input){
	int i;
	int doubleQuotes = 0;
	
	char *token = (char *)malloc(1000*sizeof(char));
	int tokenIndex = 0;

	char **tokens;
	tokens = (char **) malloc(MAXLINE*sizeof(char*));

	int tokenNo = 0;
	
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
		}
		else if (doubleQuotes == 1){
			token[tokenIndex++] = readChar;
		}
		else if (readChar == ' ' || readChar == '\n' || readChar == '\t'|| readChar == '@'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0; 
			}
		}
		else{
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

char ** get_input() {
	FILE* stream = stdin;
	char input[MAXLINE];
	fflush(stdin) ;

	char *in = fgets(input, MAXLINE, stream); // taking input ;
	if (in == NULL){
		return NULL ;
	}
	// Calling the tokenizer function on the input line    
	char **tokens = tokenize(input);	
	
	// Comment to NOT print tokens
	/*
	int i ;
	for(i=0;tokens[i]!=NULL;i++){
		printf("%s\n", tokens[i]);
	}   
	*/
	return tokens ;
}