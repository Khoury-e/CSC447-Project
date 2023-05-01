/*
 * Authors: Yaakoub Hamad, Elias khoury, Sema shehade 
 To make: mpicc -o mpi1 mpi1.c
 To Run: mpiexec mpi1 passwords.txt hashed.txt
 */
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <string.h>
#define _XOPEN_SOURCE
#include <unistd.h>
#include <time.h>
void starttime(){

    time_t timer;
    char buffer[26];
    struct tm *tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Program started at %s\n", buffer);
}

void endtime(){

    time_t timer;
    char buffer[26];
    struct tm *tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Program ended at %s\n", buffer);
}
void parprocess(MPI_File *in, MPI_File *out, const int rank, const int size, const int overlap){
    MPI_Offset globalstart;
    int mysize;
    char *chunk;

    {
        MPI_Offset globalend;
        MPI_Offset filesize;
        MPI_File_get_size(*in, &filesize);
        filesize--; /* get rid of text file eof */
        mysize = filesize / size;
        globalstart = rank * mysize;
        globalend = globalstart + mysize - 1;
        
        if (rank == size - 1) globalend = filesize - 1;
        if (rank != size - 1) globalend += overlap;

        mysize = globalend - globalstart + 1;
        chunk = malloc((mysize + 1) * sizeof(char));

        MPI_File_read_at_all(*in, globalstart, chunk, mysize, MPI_CHAR, MPI_STATUS_IGNORE);
        chunk[mysize] = '\0';
    }

    int locstart = 0, locend = mysize - 1;
    if (rank != 0){
        while (chunk[locstart] != '\n')
            locstart++;
        locstart++;
    }
    if (rank != size - 1){
        locend -= overlap;
        while (chunk[locend] != '\n')
            locend++;
    }
    mysize = locend - locstart + 1;
    // char *hash_type_1 = "$1$";
    // char *hash_type_2 = "$6$"; // type 2 implies sha-512 (default value as in yr 2017, number of iteration is minimum 10,000 rounds )
    // char *salt_1 = "$";        // a simplified 0 length salt.
    // char *result;
    // char encyption_scheme[20];  // declare an array of size 20 to store the encryption type for md5 later
    // char encyption_scheme2[20]; // declare an array of size 20 to store the encryption type for sha-512 later
    // int count = 0;
    // char sizeofline[50];
    // strcpy(encyption_scheme, hash_type_1); // copy value of hashtype into encyptionscheme
    // strcpy(encyption_scheme2, hash_type_2);
    // strcat(encyption_scheme, salt_1); // add "$" into value of encryptionscheme
    // strcat(encyption_scheme2, salt_1);
    char *crypt(const char *key, const char *salt);
    // void encrypt(char *s)
    // {
    //     int i, l = strlen(s);
    //     for (i = 0; i < l; i++)
    //         s[i] -= 15;
    // }
    // encrypt(chunk);
    // printf("%s", chunk);
    for (int i = locstart; i <= locend; i++){
        char c = chunk[i];
        chunk[i] = (isspace(c) ? c : (c ^= 4));
    }
    MPI_File_write_at_all(*out, (MPI_Offset)(globalstart + (MPI_Offset)locstart), &(chunk[locstart]), mysize, MPI_CHAR, MPI_STATUS_IGNORE);
    return;
}

int main(int argc, char **argv){
    starttime();
    clock_t t = clock();

    MPI_File in,out;
    int rank, size;
    int ierr;
    const int overlap = 100;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3){
        if (rank == 0) fprintf(stderr, "Usage: %s infilename outfilename\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }
    ierr = MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &in);
    if (ierr){
        if (rank == 0) fprintf(stderr, "%s: Couldn't open file %s\n", argv[0], argv[1]);
        MPI_Finalize();
        exit(2);
    }

    ierr = MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &out);
    if (ierr){
        if (rank == 0) fprintf(stderr, "%s: Couldn't open output file %s\n", argv[0], argv[2]);
        MPI_Finalize();
        exit(3);
    }

    parprocess(&in, &out, rank, size, overlap);

    MPI_File_close(&in);
    MPI_File_close(&out);
    MPI_Finalize();
    
    t = clock() - t;
    printf("Time Taken: %f\n", (float)t / CLOCKS_PER_SEC);
    endtime();
    return 0;
}
