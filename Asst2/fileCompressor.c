#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "tokenizer.c"

int checkFile(const char* path);
void writeFile(const char* path, char* input);
char* filetostring(char *inputpath);
char* recursive(char *basePath, const int root);


int main(int argc, char* argv[])
{
	if(argc < 4) {
		printf("Too few args \n");
		printf("Syntax: ./fileCompressor <flag> <path/file> <codebook> \n");
		return;
	}
	if(argc > 5) {
		printf("Too many args \n");
		printf("Syntax: ./fileCompressor <flag> <path/file> <codebook> \n");
		return;
	}
	
	char flag; 
	char rflag = 'a';
	char* fileorpath;
	char* codebook;
	char* input;
//	char* input2;
	char* output;
//	char* inputr;
//	char* outputr;
//	char* input = malloc(sizeof(char) * 10000);


if(argc == 4) {

	printf("Recursive mode off \n");

	if(strstr(argv[1], "d") != NULL) {
		flag = 'd';
		printf("Flag d found \n");
	} else if(strstr(argv[1], "b") != NULL) {
		flag = 'b';
		printf("Flag b found \n");
	} else if(strstr(argv[1], "c") != NULL) {
		flag = 'c';
		printf("Flag c found \n");
	} else {
		printf("Flag not found \n");
		printf("Syntax: ./fileCompressor <flag> <path/file> <codebook> \n");
		return;
	}

	fileorpath = argv[2];
    	codebook = argv[3];

} else {
	if(strstr(argv[1], "R") == NULL) {
		printf("Incorrect Syntax for Recursive Mode \n");
		printf("Syntax: ./fileCompressor -R <flag> <path/file> <codebook> \n");
		return;
	} else {
		printf("Recursive mode on \n");
		rflag = 'r';
	}
	if(strstr(argv[2], "d") != NULL) {
		flag = 'd';
		printf("Flag d found \n");
	} else if(strstr(argv[2], "b") != NULL) {
		flag = 'b';
		printf("Flag b found \n");
	} else if(strstr(argv[2], "c") != NULL) {
		flag = 'c';
		printf("Flag c found \n");
	} else {
		printf("Flag not found \n");
		return;
	}

	fileorpath = argv[3];
    	codebook = argv[4];
}		

	char originalfile[strlen(fileorpath) - 4];
	int di;


	if(rflag == 'r') {
		input =	recursive(fileorpath, 0);
//		printf("File contents: \n%sFile Size: %d \n", input, strlen(input));
//		char* inputr = malloc(sizeof(char) * 1000);
//		char* inputr;
//		input2 = strdup(input);
//		printf("Inputr contents: \n%s Size: %d \n", input, strlen(input));
	
//		output = stuff(input2, flag);	
//		writeFile(codebook, output);
//		free(inputr);
//		printf("Inputr is freeee");
//		writeFile(codebook, input);
		printf("\nCodebook has successfully been made and files have been recursively written into codebook: %s\n", codebook);
//		return;

	}
	else {	
	
	if(strstr(fileorpath, ".hcz") != NULL) {
		for(di = 0; di < strlen(fileorpath) - 4; di++) {
			originalfile[di] = fileorpath[di];
		}
		input = filetostring(originalfile);
	} else {
        	input = filetostring(fileorpath);
	}
	}
//	printf("File contents: \n%sFile Size: %d \n", input, strlen(input));
///////////////////
//	printf("Testing trim \n");
//	printf("Fileorpath is %s\n", fileorpath);
//	printf("Temp is %s\n", originalfile);

	char *input2;
/*
	if(strstr(fileorpath, ".hcz") != NULL) {
		for(di = 0; di < strlen(fileorpath) - 4; di++) {
			originalfile[di] = fileorpath[di];
		}
	//	input2 = strdup(originalfile);
	}
*/
//	else {
	input2 =strdup(input);
//	}
	printf("Input2: \n%sInput2 Size: %d \n", input2, strlen(input2));

	output = stuff(input2, flag);
//	stuff("Hello its me");

	if(flag == 'b') {
		writeFile(codebook,output);
		printf("\nCodebook has successfully been made and written into file: %s\n", codebook);
	}
	else if(flag == 'c') {
		strcat(fileorpath, ".hcz");
		writeFile(fileorpath,output);
		printf("\nCodebook was found, compression successful, writing into file: %s\n", fileorpath);
	}
	else if(flag == 'd') {

//	printf("Testing trim \n");
//	printf("Fileorpath is %s\n", fileorpath);
/*		char originalfile[strlen(fileorpath) - 4];
		int di;
		if(strstr(fileorpath, ".hcz") != NULL) {
			for(di = 0; di < strlen(fileorpath) - 4; di++) {
				originalfile[di] = fileorpath[di];
			}
			writeFile(originalfile,output);
			printf("\nCodebook was found, decompression successful, writing into original file: %s\n", originalfile);
			
		}
*/
//	printf("Temp is %s\n", originalfile);
//		else {
		writeFile(originalfile, output);
		printf("\nCodebook was found, decompression successful, writing into original file: %s\n", originalfile);
//		}
	}
//	free(input);
//	free(input2);
//	free(output);	
	
    return 0;
}


/*
 * Prints all files and sub-directories of a given 
 * directory in recursive structure. 
 * */

int checkFile(const char* path) {
	struct stat temp;
	    if(stat(path, &temp) < 0) 
		return -1;
	    if(S_ISDIR(temp.st_mode))
		return 0;
	    if(S_ISREG(temp.st_mode))
		return 1;
}
	
void writeFile(const char* path, char* input) {
	int fd = open(path, O_CREAT | O_RDWR | O_APPEND, S_IWUSR | S_IRUSR);
	write(fd, input, strlen(input));
//write(fd, binarycode, strlen(code)); 
//write(fd, "\t", 1);
//write(fd, token, strlen(token));
//write(fd, "\n", 1);

//strcopy(left, somecode)
//strcopy(right, somecode)	
//strcat(left, "0");
//strcat(right, "1");
}


char* filetostring(char* inputpath) {

	//opens the file using the input path, and writes it to a string. 
	int fd = open(inputpath, O_RDONLY);
	int len = lseek(fd, 0, SEEK_END);
	void *buffer = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);

	//char* input = (char*)malloc(sizeof(char)*1000); 
	//strcat(input, buffer);
	return buffer;
}

char* recursive(char *basePath, const int root)
{
    int i;
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    
//char array to store all the words in each text file to 
//	char* input;
	char* input = (char*)malloc(sizeof(char)*1000); 
//	char input[1000];
//	input[0] = {'\0'}
//initizializing variables for use in input
    int fd;
    int len;
    void* buffer;
//currently not able to read .txt files in subdirectories, will have to adjust path to fix this. 


//	struct stat checkFile;
	
    

    if (dir == NULL) {
//	printf("Directory not found");
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            for (i=0; i<root; i++) 
            {
                if (i%2 == 0 || i == 0)
                    printf("|");
                else
                    printf(" ");

            }
	    if(strstr(dp->d_name, ".txt") != NULL && strstr(dp->d_name, ".swp") == NULL && strstr(dp->d_name, ".hcz") == NULL && strstr(dp->d_name, "testplan") == NULL) {
		printf("Writing file to string ");
		
		fd = open(dp->d_name, O_RDONLY);
		len = lseek(fd, 0, SEEK_END);

		//mmap usage = mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset
		buffer = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
		strcat(input, buffer);
		
	

	      }
		if(checkFile(path) == -1)	   
			printf("??");
	    	if(checkFile(path) == 0)				
			printf("is DIR");
	    	if(checkFile(path) == 1)				
			printf("is FILE");

            printf("| | %s\n", dp->d_name);

            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
           recursive(path, root + 2);
        }
    }

    closedir(dir);
//printf("input is: \n%s , size of input: %d \n", input, strlen(input));
//free(input);
	



return input;
}

