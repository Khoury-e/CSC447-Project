//OpenMP code for Brute Force Dictionary Attack 

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <string.h>

#define passwords 10000
#define filename "passwords2.txt"
#define reading "r"
#define appending "a"

char* encrypt(char *real)
{
    char *pass = (char*)malloc(sizeof(char)*strlen(real));
    for (int i = 0; i < strlen(real); i++)
    {
        pass[i] = real[i] ^ 4;
    }
    pass[strlen(real)]='\0';
    return pass;
}
void main()
{
	clock_t start, end;
	float time;

	FILE *pr, *pw;
	char *buff = (char*)malloc(sizeof(char*)*10000);
	char *buff2 = (char*)malloc(sizeof(char*)*10000);
	//setting the number of threads to be used (in this case only 10, each threads reads a block of 100 passwords)
	omp_set_num_threads(10);

	start = clock();	

	// reading and hashing
	pr = fopen(filename, reading);
	pw = fopen("hashed_omp.txt", appending);
	if(pr==NULL || pw==NULL)
	{
		exit(EXIT_FAILURE);
	}	

	int i;
	int j=1000;

	#pragma omp parallel private(i,j) shared(pr, pw)
	{
		#pragma omp for
		
			for(i=0;i<=j;i++)
			{
				while(fscanf(pr, "%s", &buff[i])!=EOF){
					char *encrypted_p = encrypt(&buff[i]);
        			fputs(encrypted_p, pw);
        			fputs("\n", pw);
        			i++;
        		}
        	}
		
	}

	pr = fopen("hashed_omp.txt", reading);
	if(pr==NULL)
	{
		exit(EXIT_FAILURE);
	}
	int k;
	int p=1000;
	char *my_pass = "password";
	int found = 0;
	#pragma omp parallel private(k, p) shared(my_pass, found)
	{
		#pragma omp for 
			for(k=0;k<=p;k++){
			char *thispass = (char*)malloc(sizeof(char)*20);
			if(strcmp(encrypt(my_pass), thispass) == 0)
			{
				found = 1;

			}
			
		}
	}
	if(found==0)
		{
			printf("Not Found!!\n");
		}
		else{
			printf("Found!!\n");
		}


	end= clock();
	time = ((float)(end-start))/CLOCKS_PER_SEC;
	printf("Time of execution: %.5f\n", time);
}