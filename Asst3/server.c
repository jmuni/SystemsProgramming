#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//for hashing
#include <errno.h>
#include <limits.h>
#include <openssl/sha.h>


#include <pthread.h>
//#define BUFF_SIZE 1024
// for mutex tracking
#define maxsize 50
#define BUFF_SIZE 1024
#define MAX_PROJS 50


char **nameArray;
int *dirVer;

static pthread_mutex_t mutexes[MAX_PROJS];

int *allSocks;
pthread_t *thread;



pthread_mutex_t Mlock = PTHREAD_MUTEX_INITIALIZER;
//end for mutex tracking

typedef struct manifest
{
	char *path;
	char *version;
	char *hash;
	struct manifest *next;
} manifest;

int manifestSize(char *input)//RETURNS SIZE OF MANIFEST FOR EASY READING
{
int i;
int bsize =0;
for(i = 0; input[i]!= '\0';i++)
	{
		if(input[i] == '\n')
		{
			bsize++;
		}
	}
	return bsize;
}

/////////////FUNCTION DECLARATIONS
void *readManifest(char *input, int size, manifest* readMF);
int fileExist (char* filename);
void readFile(char* filename);
 
void emptyManifest(char* dir_name);
static void newManifest(const char * dir_name, char* mpath); 
void currentversion(int sock);

void error(char *msg);
void * sendFile(int sock, char *fileName);
void makeTar(char* fileName);
void tarAndSend(char* path, int socket);

void checkout(int sock);
void createDirectory(int sock);
void destroyDirectory(int sock);
int remove_directory(const char *path);
void removeFile(char* filename); 
char* fileToString(char* inputpath); 
void writeFile(const char* path, char* input);
void * dostuff2(void *arg);

int findMutex(char *name);
void newMutex(char *name);
void removeMutex(char *name);

void *recieveFile(int sock, char *fileName);
void *recieveTar(int sock, char *fileName);


///END FUNCTION DECLARATIONS


char cInput[2000];
char buffer[BUFF_SIZE];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


/////////////////////MUTEX FUNCTIONS
int findMutex(char *name)//RETURNS INTEGER VALUE OF MUTEX WITH GIVEN NAME FOR LOOKUP
{
	int i = 0;
	for(i = 0;i<MAX_PROJS;i++)
	{
		if(strcmp(name, nameArray[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}
int verNum(char *name)
{
	int i = 0;
	for(i = 0;i<MAX_PROJS;i++)
	{
		if(strcmp(name, nameArray[i]) == 0)
		{
			return dirVer[i];
		}
	}
	return -1;
}
void verPlus(char *name)

{
	int i = 0;
	for(i = 0;i<MAX_PROJS;i++)
	{
		if(strcmp(name, nameArray[i]) == 0)
		{
			dirVer[i]++;
		}
	}
	return;
}
void saveVer(char* name)
{
	char path[50];
	strcpy(path, name);
	strcat(path, "/.ver.txt");
	int file = open(path, O_CREAT | O_RDWR);
	write(file, "I", 2);
}
int readVerNumber(char *name)
{
	char path[50];
	char currentChar;
	strcpy(path, name);
	strcat(path, "/.ver.txt");
	FILE *fptr;
	if(fptr == NULL)
	{
		return 0;
	}
	int numCharacters;
	fptr = fopen(path, "r+");
	while(currentChar!=EOF)
	{
		numCharacters++;
		currentChar = getc(fptr);
	}
	return numCharacters-1;
}
int AnotherGetVersionNumber(char *name)
{
	int i;
	for(i = 0; i<MAX_PROJS;i++)
	{
		if(strcmp(name, nameArray[i]) == 0)
		{
			return dirVer[i];
		}

	}
}
void newMutex(char *name)//CREATES MUTEX WITH GIVEN NAME
{
	printf("Creating mutex");
	int i = 0;
	for(i = 0;i<MAX_PROJS;i++)
	{
		if(strcmp(nameArray[i], "")==0)
		{
		//	nameArray[i] = calloc(maxsize,  sizeof(char*));
			memcpy(nameArray[i], name, maxsize);
			pthread_mutex_init(&mutexes[i], NULL);
			saveVer(name);
			printf("Mutex with name %s Created at spot %i\n",nameArray[i],i);
			break;
		}
	}
	return;
}
void removeMutex(char *name)//KILLS MUTEX WITH SPECIFIC NAME
{
	printf("removing mutex");
	int i = 0;
	for(i = 0;i<MAX_PROJS;i++)
	{
		if(strcmp(nameArray[i], name)==0)
		{
			strcpy(nameArray[i], "");
			pthread_mutex_lock(&mutexes[i]);
			pthread_mutex_unlock(&mutexes[i]);
			sleep(3);
			if(pthread_mutex_destroy(&mutexes[i]) == 0)
			{
			printf("Mutex number %i Deleted\n", i);}
			break;
		}
	}
	return;
}




////////////////////END MUTEX FUNCTIONS



void makeMutexes()//MAKES MUTEXES FOR EVERY FOLDER IN THE MAIN DIRECTORY
{
	struct dirent *duh;
	DIR *direc = opendir(".");
//	struct stat st;
	if(direc == NULL)
	{
		printf("Can't open local directory, please edit permissions\n");
	}
	while((duh = readdir(direc)) !=NULL)
	{
		if(duh->d_type == DT_DIR)
		{
			if((strcmp(duh->d_name, ".")!=0) &&(strcmp(duh->d_name, "..")!=0))
			{
				newMutex(duh->d_name);
			}
		}
	}
	closedir(direc);
	return;
}





void *readManifest(char *input, int size, manifest* readMF)//reads a .manifest file into a manifest struct array, which is more easily parsed

{
	printf("reading");
	int i = 0;
	int bsize = 0;
	int curr = 0;
	char read = 'p';
	int pos = 0;
	int firstRead = 1;
	char temp[50];
	memset(temp,'\0',50);	
	printf("\n");
	for(i = 0; input[i]!='\0';i++)
	{
		if(input[i] == '\n')
		{
			printf("\nTemp is:%s", temp);
			readMF[curr].hash = malloc(sizeof(temp));
			memcpy(readMF[curr].hash, temp, sizeof(temp));
			printf("Moving to write code: Word is:%s()\n", readMF[curr].hash);
			memset(temp,'\0',sizeof(temp));
		//	printf("moving to write code\n");
			curr++;
			read = 'p';
			pos = 0;
			
		}
		else if(input[i] == '\t' && read == 'v')
		{	
			printf("\nTemp is:%s", temp);
			readMF[curr].version = malloc(sizeof(temp));
			memcpy(readMF[curr].version, temp, sizeof(temp));
		//	printf("Moving to write code: Word is:%s()\n", readMF[curr].version);
			memset(temp,'\0',sizeof(temp));
		//	printf("moving to write code\n");
			read = 'h';
			pos = 0;
		}
		else if(input[i] == '\t' && read == 'p')
		{	
			printf("\nTemp is:%s", temp);
			readMF[curr].path = malloc(sizeof(temp));
			memcpy(readMF[curr].path, temp, sizeof(temp));
		//	printf("Moving to write code: Word is:%s()\n", readMF[curr].path);
			memset(temp,'\0',sizeof(temp));
		//	printf("moving to write code\n");
			read = 'v';
			pos = 0;
		}

	/*	else if(input[i] == '\\')
		{
			printf("starting read");
		}*/
		else
		{	
		//	printf("%i\n", pos);
			temp[pos] = input[i];			
			pos++;
		}
	}
	printf("\n\nFinished");
//	free(temp);
	return;	
}

int fileExist (char* filename) {//checks if a file exists

   
	int file; 

	// Open file 
	file = open(filename, O_RDONLY); 
	if(file >= 0)
	{ 
        	return 0;
	}  
	else
	{
	close(file); 
	return 1;
	}
}

void readFile(char* filename) { // ./WTFClient read <filename>  reads the contents of a file into the terminal
	
    FILE *fptr; 
  
    char c; 
   
    // Open file 
    fptr = fopen(filename, "r"); 
    if (fptr == NULL) 
    { 
        printf("Cannot open file: %s \n", filename); 
        exit(0); 
        //return 0;
    } 
  
    // Read contents from file 
    c = fgetc(fptr); 
    while (c != EOF) 
    { 
        printf ("%c", c); 
        c = fgetc(fptr); 
    } 
  
    fclose(fptr); 
    //return 1;
 } 



void emptyManifest(char* dir_name) {//creates an empty manifest file

	
	DIR * d;
    	d = opendir (dir_name);

    	if (! d) {
        	fprintf (stderr, "Cannot open directory '%s': %s\n", dir_name, strerror (errno));
        	exit (EXIT_FAILURE);
    	}
	
	char mpath[50];
	strcpy(mpath, dir_name);
	strcat(mpath, "/.Manifest");
	writeFile(mpath, "");
	printf("Empty Manifest created");

	//test to see inside empty manifest
	readFile(mpath);

	return;
}

static void newManifest(const char * dir_name, char* mpath) {//creates a new manifest for a given directory

    
	DIR * d;
    	d = opendir (dir_name);

    	if (! d) {
        	fprintf (stderr, "Cannot open directory '%s': %s\n", dir_name, strerror (errno));
        	exit (EXIT_FAILURE);
    	}
	    
	writeFile(mpath, ".Manifest");
	writeFile(mpath, "\t");
		
	writeFile(mpath, "1");
	writeFile(mpath, "\t");

	writeFile(mpath, "hash");
	writeFile(mpath, "\n");

	while (1) {
        	struct dirent * entry;
        	const char * d_name;
	//	struct stat sb;

        	entry = readdir (d);
        	if (! entry) {
           		break;
        	}

		if(entry->d_name[0] == '.')// && (strcmp(entry->d_name,".Manifest")==0)) { //if it starts with . DONT PUT INTO MANIFEST
		{	printf("%s is not added to manifest \n", entry->d_name);
		} else if(strcmp(entry->d_name,"Makefile") == 0) {
			printf("%s was found, not added to .Manifest \n", entry->d_name);	
	//	} else if(stat(d_name, &sb) == 0 && sb.st_mode & S_IXUSR) {
	//		printf("%s is an executable, not added to .Manifest");
		} else {

        	d_name = entry->d_name;
        
		if(entry->d_type & DT_DIR) {
		
		} else {	
		printf("%s/%s\n", dir_name, d_name);
		char fullpath[100];
		strcpy(fullpath, (char*)dir_name);
		strcat(fullpath, "/");
		strcat(fullpath, (char*)d_name);

		//THIS WORKS
		printf("FULL PATH: %s\n", fullpath); 
		
		/*
		writeFile(mpath, (char*)dir_name);
		writeFile(mpath, "/");
		writeFile(mpath, (char*)d_name);
		*/
		writeFile(mpath, fullpath);
		writeFile(mpath, "\t");
		
		writeFile(mpath, "1");
		writeFile(mpath, "\t");

		
		char* filestring = fileToString(fullpath);
			
		//SHA1 ATTEMPT TO HASH
		size_t length = strlen(filestring);
		unsigned char hash[SHA_DIGEST_LENGTH];
		SHA1(filestring, length, hash); 

		char hashcode[41];
		//char* hashpart;
		//hashcode = hash;
		int i;
		for(i = 0; i<20;i++){
			sprintf(&hashcode[i*2], "%02x",hash[i]);
		}
		hashcode[40]=0;
		//printf("\n");
		printf("Hashcode is %s \n", hashcode);
			
		writeFile(mpath, hashcode);
		writeFile(mpath, "\n"); 
		
		}

        	if (entry->d_type & DT_DIR) {

            	/* Check that the directory is not "d" or d's parent. */
            
            	if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0) {
                int path_length;
                char path[PATH_MAX];
 
                path_length = snprintf (path, PATH_MAX,"%s/%s", dir_name, d_name);
		//This prints out the directory
                //printf ("%s [path print]\n", path);
                //printf("That was a dir \n");
                if (path_length >= PATH_MAX) {
                    fprintf (stderr, "Path length has got too long.\n");
                    exit (EXIT_FAILURE);
                }
                /* Recursively call "newManifest" with the new path. */
                newManifest (path, mpath);
		}
            }
	}
    }
    /* After going through all the entries, close the directory. */
    if (closedir (d)) {
        fprintf (stderr, "Could not close '%s': %s\n",dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
}

void currentversion(int sock) {


	int socket = sock;
	memset(buffer, 0, 1024);
	read(socket, buffer, 1024);
	char directory[50];
	memcpy(directory, buffer,50);
	struct stat st;
	printf("%s\n", directory);
	if(stat(directory, &st)==-1)
	{
        	printf("project not found!");
		write(sock, "ERROR: PROJECT NOT FOUND ON SERVER", 1024);
	}
	else
	{
        	printf("Directory Found!\n");
		if(pthread_mutex_lock(&mutexes[findMutex(directory)]) == 0)
		{
			printf("Locking Directory %s with Directory Specific Mutex %i\n", directory, findMutex(directory));
		}
		else
		{
			printf("Mutex Lock Error");
		}
		//printf("hello");

       	 	//tarAndSend(directory, socket);
       	 	char mpath[50];
		memcpy(mpath, directory,50);
		strcat(mpath, "/.Manifest");
		char cvpath[50];
		strcpy(cvpath, directory);
		strcat(cvpath, ".CV");
		printf("Up to if / else");

		if(fileExist(mpath) != 1) {
			newManifest(directory, mpath);
		}
		if(fileExist(mpath)==1) {
			char* manifeststring = fileToString(mpath); 
			//printf("THIS IS THE MANIFEST STRING: %s", manifeststring); 
			int size = manifestSize(manifeststring);
			printf("\n\n%i\n\n", size);
			manifest* MFFromFile;
			MFFromFile = malloc(size*sizeof(manifest));
			readManifest(manifeststring, size, MFFromFile);

			
			int i;
			printf("\n");
			//prints out all the file paths, versions, and hashes
			for(i = 0; i < size; i++) {
				//printf("%s\n", MFFromFile[i].path);
				writeFile(cvpath, MFFromFile[i].path);
				writeFile(cvpath, "\t");
				//printf("%s\n", MFFromFile[i].version);
				writeFile(cvpath, MFFromFile[i].version);
				writeFile(cvpath, "\n");

				//printf("%s\n", MFFromFile[i].hash);
				//printf("\n");
			}
			//frees the struct
			for(i = 0; i<size; i++)
			{
				free(MFFromFile[i].path);
				free(MFFromFile[i].version);
				free(MFFromFile[i].hash);
			}
			free(MFFromFile);
		} else {
			
				writeFile(cvpath, "NO FILES UNDER DIRECTORY");
		}
		tarAndSend(cvpath,socket);
		//sendFile(socket, cvpath);
		//removeFile(cvpath);
		strcat(cvpath,".tar.gz");
		removeFile(cvpath);
		if(pthread_mutex_unlock(&mutexes[findMutex(directory)]) == 0)
		{
			printf("Unlocking %s Mutex %i\n", directory, findMutex(directory));
		}
		else
		{
			printf("Mutex Unlock Error");
		}
		
    	}
	return;
	    
}


void error(char *msg)
{
    perror(msg);
    exit(1);
}

void *recieveFile(int sock, char *fileName)//generic function for recieving a file, reading it in chunks of 1024 bytes from a socket and writing it to a file with a name given by the client

{
	printf("Recieving File %s...\n");
	int file;
	int fileSize;
	int remaining;
	int chunk;
	int bytes;
	bytes = read(sock, buffer, 1024); //reads 1024 bytes from the socket, stores this information as the size of the file it is set to recieve
	printf("\nBytes Read: %i\n", bytes);
	printf("File Size is: %s\n", buffer);
	fileSize = atoi(buffer);
	remaining = fileSize;
	file = open(fileName, O_RDWR | O_CREAT, 0644); //opens/creates a new file with the desired filename and gives a file descriptor
	while((remaining > 0) && ((chunk = read(sock, buffer, 1024))>0))//loops until the ammount of bytes to be recieved is zero
	{
	//	printf("reading chunk");
		printf("Chunk Size: %i\n", chunk);
		write(file, buffer, chunk); // writes the bytes to the file descriptor
		remaining-=chunk;//deincriments the 'remaining' value so the loops knows when to end
	}
	return;
}

void newDir(char *name)
{
	char output[70];
	char num[2];
//	strcpy(output, "/");
	strcat(output, name);
	strcat(output, "/ver");
	int ver = verNum(name);
	sprintf(num, "%i", ver);
	strcat(output, num);
	printf("MEW DIRECTORY IS: %s", output);
	mkdir(output, 0700);
}

void *recieveTar(int sock, char *fileName)//a version of recieveFile meant for tar files, which untars them into a directory of the same name

{
	printf("Recieving File %s...\n");
	int file;
	int fileSize;
	int remaining;
	int chunk;
	int bytes;
	char output[50];
//	char buffer[1024];
	//int newSock = *((int *)arg);
	bytes = read(sock, buffer, 1024);
	if(strcmp(buffer, "ERROR: PROJECT NOT FOUND ON SERVER")==0)
	{
		printf(buffer);
		return;
	}
	printf("\nBytes Read: %i\n", bytes);
	printf("File Size is: %s\n", buffer);
	fileSize = atoi(buffer);
	remaining = fileSize;
	char regName[50];
	int i;
	for(i = 0; fileName[i]!='\0';i++)
	{
	}
	i = i-7;
	int j;
	for(j = 0; j<i;j++)
	{
		regName[j] = fileName[j];
	}
	regName[j] = '\0';
	
	file = open(fileName, O_RDWR | O_CREAT, 0644);
	while((remaining > 0) && ((chunk = read(sock, buffer, 1024))>0))
	{
	//	printf("reading chunk");
		printf("Chunk Size: %i\n", chunk);
		write(file, buffer, chunk);
		remaining-=chunk;
	}
	newDir(regName);
	strcpy(output, "tar xvzf ");
	strcat(output, fileName);
	strcat(output, " -C ");
	char num[2];
//	strcpy(output, "/");
	strcat(output, regName);
	strcat(output, "/ver");
	int ver = verNum(regName);
	sprintf(num, "%i", ver);
	strcat(output, num);
	printf("OUTPUT: %s\n",output);
	system(output);
	verPlus(regName);
	strcat(regName, ".tar.gz");
	removeFile(regName);
	return;
}



void * sendFile(int sock, char *fileName)
{
//	char buffer[1024] = {0};
//	memset(buffer, 0, 1024);
	printf("Sending File %s...\n", fileName);
	int loop = 1;
	int n;
	int file;
	int length;
	size_t size;
	file = open(fileName , O_RDWR, 0);//creates a file descriptor for the file that is meant to be sent
	if(file == -1)
	{
		printf("ERROR: PROJECT NOT FOUND ON SERVER");
		write(sock, "ERROR: PROJECT NOT FOUND ON SERVER", 1024);
		return;
	}
	printf("File %d", file);
	struct stat fSize;
	if(stat(fileName, &fSize)==0)
	{
		size = fSize.st_size;//the size of the file
	}
	printf("\n\nFile Size Is:%lu\n\n", size);
	n = sprintf(buffer, "%lu\0", size);
	printf("Buffer Size: %i\n", sizeof(buffer));
	printf("buffer: %s\n", buffer);
	if((length = write(sock, buffer, 1024)) <= 0)//writes the file size to the socket for the other application to read
	{
		printf("Error sending file size");
	}
	else
	{
		printf("File Size Sent! %i\n", length);
	}
	//write(file, buffer, 1024);
	sleep(3);//may comment out later
	int readLength;
	while(loop = 1)
	{
		readLength = read(file, buffer, 1024); //the ammount of byes reaed from the buffer
		printf("Read Length: %i\n", readLength);
		if(readLength == 0)
		{
			printf("Finished Reading\n");
			break;
		}
		if(readLength == -1)
		{
			printf("Error, Readlength below zero");
			break;
		}
		if((length = write(sock, buffer, readLength)) == -1)//writes the bytes from the buffer to the file
		{
			printf("Error writing to socket");
			break;
		}
		printf("Length: %i\n", length);
	}
	close(file);
	return;
}
void makeTar(char* fileName)
{
	char output[100];
	strcpy(output, "tar -czvf ");
	strcat(output, fileName);
	strcat(output, ".tar.gz ");
	strcat(output, fileName);
	system(output);
	return;
}

void tarAndSend(char* path, int socket)
{
	makeTar(path);
	char tarN[100];
	strcpy(tarN, path);
	strcat(tarN, ".tar.gz");
	sendFile(socket, tarN);
	return;
}

void checkout(int sock)
{
	int socket = sock;
	memset(buffer, 0, 1024);
	read(socket, buffer, 1024);
	char directory[50];
	strcpy(directory, buffer);
	
	struct stat st;
	if(stat(directory, &st)==-1)
	{
        	printf("project not found!");
		write(sock, "ERROR: PROJECT NOT FOUND ON SERVER", 1024);
	}
	else
	{
        	printf("Directory Found!\n");

		//check if .Manifest is made, else make one
		char mpath[50];
		strcpy(mpath, directory);
		strcat(mpath,"/.Manifest");
		
		if(fileExist(mpath) == 1) {
			
		} else {
			newManifest(directory, mpath);
		}
		
       	 	tarAndSend(directory, socket);
    	}
	strcat(directory, ".tar.gz");
	//printf("%s ATTEMPT TO REMOVE",directory);
	removeFile(directory);
	return;
}

void update(int sock) {
	int socket = sock;
	memset(buffer, 0, 1024);
	read(socket, buffer, 1024);
	char directory[50];
	strcpy(directory, buffer);
	struct stat st;
	if(stat(directory, &st)==-1)
	{
        	printf("project not found!");
		write(sock, "ERROR: PROJECT NOT FOUND ON SERVER", 1024);
	}
	else
	{
        	printf("Directory Found!\n");
		if(pthread_mutex_lock(&mutexes[findMutex(directory)]) == 0)
		{
			printf("Locking Directory %s with Directory Specific Mutex %i\n", directory, findMutex(directory));
		}
		else
		{
			printf("Mutex Lock Error");
		}
		//check if .Manifest is made, else make one
		char mpath[50];
		strcpy(mpath, directory);
		strcat(mpath,"/.Manifest");
		
		if(fileExist(mpath) == 1) {
			
		} else {
			newManifest(directory, mpath);
		}
		
       	 	sendFile(socket, mpath);
		
		if(pthread_mutex_unlock(&mutexes[findMutex(directory)]) == 0)
		{
			printf("Unlocking %s Mutex %i\n", directory, findMutex(directory));
		}
		else
		{
			printf("Mutex Unlock Error");
		}
		
    	}
	return;
}
	

void createDirectory(int sock)
{
	printf("Starting create\n");
	int socket = sock;
	memset(buffer, 0, 1024);
	read(sock, buffer, 1024);
	char directory[50];
	strcpy(directory, buffer);
	struct stat st;
	if(stat(directory, &st)==-1)
	{
		pthread_mutex_lock(&Mlock);///////////////////METALOCK
		newMutex(directory);//////////////////MANIPULATES METAFILES
		mkdir(directory, 0700);
		//initialize .Manifest inside directory
		emptyManifest(directory);
		memset(buffer,0,1024);
		strcpy(buffer, "Creation of Directory Successful!\n");
		write(sock, buffer, 1024);
		pthread_mutex_unlock(&Mlock);////////////////METAUNLOCK
	}
	else
	{	
		memset(buffer,0,1024);
		strcpy(buffer, "Creation of Directory FAILED! ALREADY EXISTS\n");
		write(sock, buffer, 1024);
	}
	return;
}

void destroyDirectory(int sock)
{
	printf("Starting destroy");
	int socket = sock;
	memset(buffer, 0, 1024);
	read(sock, buffer, 1024);
	char directory[50];
	strcpy(directory, buffer);
	struct stat st;
	if(stat(directory, &st) != -1)
	{
		rmdir(directory); //THIS ONLY WORKS FOR EMPTY DIRECTORY'
		pthread_mutex_lock(&Mlock);//////////////////METALOCK
		if(pthread_mutex_lock(&mutexes[findMutex(directory)]) == 0)
		{
			printf("Locking Directory %s: Preparing for deletion\n", directory);
		}
		else
		{
			printf("Mutex Lock Error");
		}
		if(pthread_mutex_unlock(&mutexes[findMutex(directory)]) == 0)
		{
			printf("All Concurrent operations finished, Unlocking %s Mutex %i\n", directory, findMutex(directory));
		}
		else
		{
			printf("Mutex Unlock Error");
		}
		
		removeMutex(directory);/////////////////////WATCH THIS CLOSELY
		printf("%s",directory);
		remove_directory(directory); //THIS RECUSIVELY DELETES ALL FILES AND SUBDIRECTORIES. 
		rmdir(directory); //THIS ONLY WORKS FOR EMPTY DIRECTORY'

		memset(buffer,0,1024);
		strcpy(buffer, "Deletion of Directory Successful!\n");
		write(sock, buffer, 1024);
		pthread_mutex_unlock(&Mlock);//////////////////METAUNLOCK
	
	}
	else
	{	
		memset(buffer,0,1024);
		strcpy(buffer, "Deletion of Directory FAILED! DIRECTORY NOT FOUND\n");
		write(sock, buffer, 1024);
	}
	return;
}

void push(int sock) 
{
	char projectname[50];
	int socket = sock;
	//int socket = socketConnect();
	memset(buffer, 0, 1024);
	read(socket, buffer, 1024);
	strcpy(projectname, buffer);
	//memset(buffer, 0, 1024);
	strcat(projectname, ".tar.gz");
	recieveTar(socket, projectname);
	///at this point, client recieves tarred project file, and it upzips
		
			
	//delete the tar
	if(fileExist(projectname) == 1) 
		removeFile(projectname); 
	
	return;
}


int remove_directory(const char *path)
{
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d)
   {
      struct dirent *p;

      r = 0;

      while (!r && (p=readdir(d)))
      {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
          {
             continue;
          }

          len = path_len + strlen(p->d_name) + 2; 
          buf = malloc(len);

          if (buf)
          {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);

             if (!stat(buf, &statbuf))
             {
                if (S_ISDIR(statbuf.st_mode))
                {
                   r2 = remove_directory(buf);
                }
                else
                {
                   r2 = unlink(buf);
                }
             }

             free(buf);
          }

          r = r2;
      }

      closedir(d);
   }

   if (!r)
   {
      r = rmdir(path);
   }

   return r;
}

void removeFile(char* filename) { //to remove tar.gz files on server side
	
	if(remove(filename) == 0) {
		printf("%s was removed successfully \n", filename);
	} else {
		printf("Unable to remove this file: %s Check for proper file path\n", filename);
	}
}

char* fileToString(char* inputpath) {

	//opens the file using the input path, and writes it to a string. 
	int fd = open(inputpath, O_RDONLY);
	int len = lseek(fd, 0, SEEK_END);
	if(len == 0) {
		return;
	}
	void *buffer = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);

	//char* input = (char*)malloc(sizeof(char)*1000); 
	//strcat(input, buffer);
	return buffer;
}

void writeFile(const char* path, char* input) {
	
	int fd = open(path, O_CREAT | O_RDWR | O_APPEND, S_IWUSR | S_IRUSR);
	write(fd, input, strlen(input));
}




void * dostuff2(void *arg)
{
	int newSock = *((int *)arg);
	memset(buffer, 0, 1024);
	recv(newSock, buffer, 1024, 0);
	pthread_mutex_lock(&lock);
	char command[20];
	strcpy(command, buffer);
	printf("Command: %s\n", command);

	if(strcmp(command, "checkout") == 0) //3.1
	{
		checkout(newSock); 	
	}
	if(strcmp(command, "update")==0) //3.6
	{
		update(newSock);
	}

	if(strcmp(command, "create")==0) //3.6
	{
		createDirectory(newSock);
	}
	if(strcmp(command, "destroy")==0) //3.7
	{
		destroyDirectory(newSock);
		//createDirectory(newSock);
	}
	if(strcmp(command, "push")==0) 
	{
		push(newSock);
	}
	if(strcmp(command, "currentversion") == 0) //3.10
	{
		//do currentversionstuff
		currentversion(newSock);
	}
	
	//strcat(output, "\n");
	//strcpy(buffer, output);
	//free(output);
	pthread_mutex_unlock(&lock);
	//mutex here
	sleep(1);
	//send(newSock,buffer,256,0);
	//sendFile(newSock, "./test.c");
	//recieveFile(newSock, "./manifestTest.c");
	printf("Exit socketThread\n");
	close(newSock);//closes the socket
	pthread_exit(NULL);//kills the thread
}
/**void * dostuff(int sock)
{
	char message[2000];
	char buffer[BUFF_SIZE];
	read(sock, message, 2000);
	printf("%s", message);
	char *output = malloc(sizeof(message));
	strcpy(output, "Hello Client: ");
	strcat(output, message);
	if(message[0] == 't')
	{
		b(message);
	}
	if(message[0] == 'm')
	{
		m(message);
	}
	strcat(message, "\n");
	strcpy(buffer, output);
	free(output);
	//mutex here
	sleep(1);
	write(sock,buffer,256);
	printf("Exit socketThread\n");
}*/
void killAll()
{
	printf("Hes dead jim");
	int i;
	for(i = 0; i<50; i++)
	{
	//	write(allSocks[i], "SERVER HAS CLOSED, SORRY", 1024);
	//	close(allSocks[i]);
		free(nameArray[i]);
	}
	free(nameArray);
	exit(1);
}
int main(int argc, char *argv[])//Its main. you know what main is. Okay fine, main esablishes the sockets, allocates space for the mutex array names, handles SIGINT, and runs a loop to accept new connections, putting all of them into threads
{
	int sSock;
	int newSock;
	int port;
	struct sockaddr_in serverAddress;
	struct sockaddr_storage serverStorage;
	socklen_t addressSize;
	signal(SIGINT, killAll);
	//mutex functions being added
	int k;
	if(argc!=2)
	{
		printf("Syntax: ./WTFserver <port>");
		exit(1);
	}
	nameArray = malloc(MAX_PROJS * sizeof(char*));//////////////////update:add this line
	dirVer = malloc(MAX_PROJS * sizeof(int));
	for(k=0;k<MAX_PROJS;k++)////////////////////UPDATE:ADD THIS ENTIRE LOOP
	{
		nameArray[k] = malloc(maxsize *sizeof(char));
		memset(nameArray[k],0,maxsize);	
	}

	//makeMutexes();

	pthread_t thread;
	port = atoi(argv[1]);
	sSock = socket(AF_INET, SOCK_STREAM,0);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	memset(serverAddress.sin_zero, '\0', sizeof(serverAddress.sin_zero));
	bind(sSock, (struct sockaddr *) &serverAddress, sizeof(serverAddress));//bind call
	
	if(listen(sSock, 50) == 0)
	{
		printf("Listening\n");
	}
	int pid;
	int i = 0;
	addressSize = sizeof(serverStorage);
	while(1)
	{
	newSock = accept(sSock, (struct sockaddr *) &serverStorage, &addressSize);//accepts new connection
	
	printf("\nAccepted\n");
		if(pthread_create(&thread, NULL, dostuff2, &newSock) != 0)//creates a new thread for that connection, sends it to a function to handle the rest
		{
			printf("Thread creation failed\n");
		}
		
		/**pid = fork();
		if(pid == 0)
		{
			close(sSock);
			printf("%i\n", pid);
			dostuff(newSock);
			exit(0);		
		}
		else
		{
			close(newSock);
		}*/
	}
	return 0; 
}
