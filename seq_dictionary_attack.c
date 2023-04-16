#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

char* encrypt(char *real)
{
	char *pass = (char*)malloc(sizeof(char)*strlen(real));
	for (int i = 0; i < strlen(real); i++)
	{
		pass[i] = real[i] ^ 4;
	}
	return pass;
}
void main(int argc, char *argv[]) 
{
	if(argc!=2)
	{
		printf("Must be two arguments!!\n");
	}
	else {
		clock_t start, end;
		float time_exec;

		FILE *fptr, *fptw;

		// encrypting and writing the encrypted passwords (of passwords.txt) into hashed_pass.txt
		fptr = fopen("passwords2.txt", "r"); // reading all passwords
		fptw = fopen("hashed_pass.txt","a");
		while(fptr != NULL)
		{
			char *p = (char*)malloc(sizeof(char)*20);
			char *encrypted_p = (char*)malloc(sizeof(char)*20); 

			fscanf(fptr, "%s", p);
			encrypted_p = encrypt(p);
			fputs(encrypted_p, fptw);
			fputs("\n", fptw);
		}	

		//looking for the password
		start = clock();
		char *my_pass = argv[1];
		char *my_encrypted = encrypt(my_pass);
		fptr = fopen("hashed_pass.txt","r");
		while(fptr!=NULL)
		{
			char *thispass = (char*)malloc(sizeof(char)*20);
		
			fscanf(fptr, "%s", thispass);
			if(my_encrypted == thispass)
			{
				printf("The cracked password is %s\n", thispass);
				break;
			}
			if(fptr == NULL)
			{
				printf("Not Found!!\n");
			}
		}
		end=clock();
		time_exec = ((float)(end-start))/CLOCKS_PER_SEC;
		printf("Time of execution: %.5f\n", time_exec);
	}
}
