/** @file shell.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "log.h"

#define _BUFFER_SIZE_ 1024
typedef int bool;
#define false 0
#define true 1
pid_t process_ID;
char * change_working_directory;
char * line_buffer;
size_t line_size;
ssize_t read_size;
log_t * my_log;

char ** tokenize(const char * input,int len)
{
	char * str = strdup(input);
	len = 0;
	int cap = 5;
	char ** words = malloc(cap * sizeof(char *));
	char * token = strsep(&str,"\n");
	char * p = strsep(&token," ");
	words[0] = p;
	len = len + 1;
	while(token != NULL)
	{
		p = strsep(&token," ");
		if(len >= cap)
			words = realloc(words, (cap = 2 * cap) * sizeof(char*));
		words[len] = p;
		len = len+1;
	}

	if(len >= cap)
		words = realloc(words,(cap++)*sizeof(char*));
	
	words[len] = NULL;
	len = len + 1;
	return words;

}

void insert(const char* buffer)
{
	char * temp = (char*) malloc(sizeof(char) * (strlen(buffer) + 5));
	strcpy(temp, buffer);
	log_push(my_log, temp);
}

bool unknown_command(char * query)
{
	insert(query);
	pid_t cpid;
	int status;

	if((cpid = fork()) < 0)
	{
		exit(0);
	}

	else if (cpid== 0)
	{
		int length = strlen(query);
		char ** words = tokenize(query, length);
		if(execvp(query, words) < 0)
		{
			printf("%s: not found\n", query);
			exit(0);
		}	
	}

	else
		while(wait(&status) != cpid);

	if(cpid == -1)
	{
		return false;
	}

	else
		return true;
}

void destroy()
{
	if(line_buffer != NULL)
		free(line_buffer);

	if(change_working_directory != NULL)
		free(change_working_directory);

	log_destroy(my_log);
}


/**
 * Starting point for shell.
 */
int main() {
	my_log = malloc(sizeof(log_t));
	log_init(my_log);

	process_ID = getpid();
	char buffer[_BUFFER_SIZE_];
	char * m_ptr = NULL;
	char * q_ptr = NULL;
	change_working_directory = getcwd(buffer, _BUFFER_SIZE_);

//ask for input
	printf("(pid = %d)%s$ ", process_ID, change_working_directory);
	line_buffer = NULL;
	line_size = 0;

//parsing input
	while(m_ptr || (read_size = getline(&line_buffer, &line_size, stdin))!= -1)
	{
		if(m_ptr) //if match pointer points to something
		{
			q_ptr = m_ptr; //set it to query
			m_ptr = NULL; //NULL match pointer
		}

		else //otherwise if match pointer is NULL
		{
			q_ptr = line_buffer; //have it point to the line buffer
			if(q_ptr[read_size - 1] == '\n') //if there is a new line
			{
				q_ptr[read_size - 1] = '\0'; //change it to a terminator
				read_size--; // lower the size
			}
		}


//!Query
		if(q_ptr[0] == '!' && q_ptr[1] != '#')
		{
			if(strlen(q_ptr) > 1)
			{
				if((m_ptr = (char *) log_search(my_log->next, q_ptr + 1)) != NULL)
				{
					printf("%s matches %s\n", q_ptr + 1, m_ptr);
					q_ptr = q_ptr - 1;
					continue;
				}

				else
				{
					printf("No Match\n");
					printf("(pid=%d%s$", process_ID, change_working_directory);
					continue;
				}
			}

			else
			{
				printf("(pid=%d)%s$", process_ID, change_working_directory);
				continue;
			}
		}



//!#
		if(strncmp(q_ptr, "!#", 2) == 0)
		{
			if(strcmp(q_ptr, "!#") == 0)
			{
				if(my_log->next == NULL)
				{
					printf("\n");
					printf("(pid=%d)%s$", process_ID, change_working_directory);
					continue;
				}
				print_log(my_log->next);
			}
				
			printf("(pid=%d)%s$", process_ID, change_working_directory);
			continue;
		}

	//change directory

		if(strncmp(q_ptr, "cd", strlen("cd")) == 0 && strlen(q_ptr) > 3)
		{
			insert(q_ptr); //add this input to the log
			if(chdir(q_ptr + 3) == -1)
			{
				if(errno == ENOENT)
					printf("%s: No such file or directory\n", q_ptr + 3);
				else
				{
					free(q_ptr);
					log_destroy(my_log);
					free(my_log);
					exit(1);
				}
			}

			else
				change_working_directory = getcwd(buffer, _BUFFER_SIZE_);
			printf("(pid=%d)%s$", process_ID, change_working_directory); //prompt again			
			continue;
		}

		

		//terminate command
		if(strcmp(q_ptr, "exit") == 0)
		{
			free(q_ptr);
			break;
		}
	
		if(unknown_command(q_ptr))
		{
			printf("(pid=%d)%s$", process_ID, change_working_directory);
			continue;
		}
	
		else
		{
			free(q_ptr);
			log_destroy(my_log);
			free(my_log);
			exit(1);
		}
	}

	if(read_size == -1)
	{
		free(q_ptr);
		log_destroy(my_log);
		free(my_log);
		exit(1);
	}
	
	log_destroy(my_log);
	free(my_log);
    return 0;
}
