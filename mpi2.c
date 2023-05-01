/*
 * Authors: Yaakoub Hamad, Elias khoury, Sema shehade 
 To make: mpicc -o mpi2 mpi2.c
 To Run: mpiexec  mpi2
 */
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

char *xorit(char *real, char *pass){
	for (int i = 0; i < strlen(real); i++){
		pass[i] = real[i] ^ 4;
	}
	return pass;
}

int FindPassword(MPI_File *input, char *pass, int length, const int rank, const int size, const int olap){
	MPI_Offset starting_offset;
	int mysize;
	char *buff;

	MPI_Offset ending_offset;
	MPI_Offset file_size;

	MPI_File_get_size(*input, &file_size);

	file_size--;

	mysize = file_size / size;

	starting_offset = rank * mysize;

	ending_offset = starting_offset + mysize - 1;

	if (rank == size - 1) ending_offset = file_size - 1;
	if (rank != size - 1) ending_offset += olap;

	mysize = ending_offset - starting_offset + 1;
	buff = malloc((mysize + 1) * sizeof(char));
	MPI_File_read_at_all(*input, starting_offset, buff, mysize, MPI_CHAR, MPI_STATUS_IGNORE);
	buff[mysize] = '\0';

	int locstart = 0, locend = mysize - 1;
	if (rank != 0){
		while (buff[locstart] != '\n')
			locstart++;
		locstart++;
	}
	if (rank != size - 1){
		locend -= olap;
		while (buff[locend] != '\n')
			locend++;
	}

	int i = 0, j = 0;
	for (i = locstart; (i <= locend); i++){
		char c = buff[i];
		if (isspace(c)){
			j = 0;
			continue;
		}
		if ((j < length) && (c == pass[j])){
			j++;
		}

		if (j == length){
			free(buff);
			return 1;
		}
	} // close for
	free(buff);
	return 0;
}

int main(int argc, char **argv){

	char *real = "thegod";
	int length = strlen(real);
	int l = strlen(real);
	char pass[l];
	xorit(real, pass);
	// strncpy(subtext, &pass[l-11], 11);
	// printf("%s", pass);
	MPI_File input;

	int rank, size;
	int error_code;
	const int olap = 100;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	error_code = MPI_File_open(MPI_COMM_WORLD, "hashed.txt", MPI_MODE_RDONLY, MPI_INFO_NULL, &input);
	if (error_code){
		if (rank == 0)
			fprintf(stderr, "%s: Can't Open the Password File: %s\n", argv[0], argv[1]);
		MPI_Finalize();
		exit(2);
	}

	clock_t t = clock();
	int success = FindPassword(&input, pass, length, rank, size, olap);
	t = clock() - t;
	if (success == 1 && rank == 0){
		printf("Password Found!\n");
		printf("Time Taken: %f\n", (float)t / CLOCKS_PER_SEC);
	}
	else{
		if (rank == 0){
			printf("Password NOT found.\n"); // Fixed missing printf statement
		}
	}

	MPI_File_close(&input);
	MPI_Finalize();
	return 0;
}

