#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <errno.h>
#include <limits.h>
#include <openssl/sha.h>


typedef struct manifest
{
	char *path;
	char *version;
	char *hash;
	struct manifest *next;
} manifest;

int manifestSize(char *input)
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
	}//manifest* readMF = malloc(bsize * sizeof(manifest)); should be called before reading the manifest

void *readManifest(char *input, int size, manifest* readMF);

void writeFile(const char* path, char* input); 
void configure(char* host, char* port);
static void newManifest(const char * dir_name, char* mpath); 
void deleteFromManifest(const char* dir_name, char* filename, char* mpath);

void readFile(char* filename);
static void listFiles(const char * dir_name); 
void listDirs(const char * dir_name);
void removeFile(char* filename); 
char* fileToString(char *inputpath);
int isdir(const char* path);
int fileExist (char* filename); 
int clientdir(const char* dir_name, char* projectname); 

void makeTar(char* fileName);
void tarAndSend(char* path, int socket);
void *recieveFile(int sock, char *fileName);
void *recieveTar(int sock, char *fileName);
void * sendFile(int sock, char *fileName);
int socketConnect();



//////////////////////////////////////////////////////////////////////////// START OF MANFIEST STRUCT


void *readManifest(char *input, int size, manifest* readMF)
{
	//printf("reading");
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
		//	printf("\nTemp is:%s", temp);
			readMF[curr].hash = malloc(sizeof(temp));
			memcpy(readMF[curr].hash, temp, sizeof(temp));
			//printf("Moving to write code: Word is:%s()\n", readMF[curr].hash);
			memset(temp,'\0',sizeof(temp));
		//	printf("moving to write code\n");
			curr++;
			read = 'p';
			pos = 0;
			
		}
		else if(input[i] == '\t' && read == 'v')
		{	
		//	printf("\nTemp is:%s", temp);
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
		//	printf("\nTemp is:%s", temp);
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
	//printf("\n\nFinished");
//	free(temp);
	return;	
}
//////////////////////////////////////////////////////////////////////// END MANIFEST STRUCT


////////////////////////////////////////START SOCKET FUNCTIONS
//
char buffer[1024];

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

void *recieveFile(int sock, char *fileName)
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
	file = open(fileName, O_RDWR | O_CREAT, 0644);
	while((remaining > 0) && ((chunk = read(sock, buffer, 1024))>0))
	{
	//	printf("reading chunk");
		printf("Chunk Size: %i\n", chunk);
		write(file, buffer, chunk);
		remaining-=chunk;
	}
	//strcpy(output, "tar xvzf ");
	//strcat(output, fileName);
	//system(output);
	return;
}

void *recieveTar(int sock, char *fileName)
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
	file = open(fileName, O_RDWR | O_CREAT, 0644);
	while((remaining > 0) && ((chunk = read(sock, buffer, 1024))>0))
	{
	//	printf("reading chunk");
		printf("Chunk Size: %i\n", chunk);
		write(file, buffer, chunk);
		remaining-=chunk;
	}
	strcpy(output, "tar xvzf ");
	strcat(output, fileName);
	system(output);
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
	file = open(fileName, O_RDWR, 0);
	printf("File %d", file);
	struct stat fSize;
	if(stat(fileName, &fSize)==0)
	{
		size = fSize.st_size;
	}
	printf("\n\nFile Size Is:%lu\n\n", size);
	n = sprintf(buffer, "%lu\0", size);
	printf("Buffer Size: %i\n", sizeof(buffer));
	printf("buffer: %s\n", buffer);
	if((length = write(sock, buffer, 1024)) <= 0)
	{
		printf("Error sending file size");
	}
	else
	{
		printf("File Size Sent! %i\n", length);
	}
	//write(file, buffer, 1024);
	sleep(3);
	int readLength;
	while(loop = 1)
	{
		readLength = read(file, buffer, 1024);
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
		if((length = write(sock, buffer, readLength)) == -1)
		{
			printf("Error writing to socket");
			break;
		}
		printf("Length: %i\n", length);
	}
	close(file);
	return;
}

int socketConnect()
{
	printf("Hello");
	int clientSocket;
	struct sockaddr_in serverAddress;
	struct hostent *server;
	size_t size;
	int i;
	int portn;
	struct stat st;
	stat("./.configure", &st);
	size = st.st_size;
	printf("\n\nFile Size is: %lu\n", size);
	int configF = open("./.configure", O_RDONLY, 0);
	char fileCont[size];
	char host[30] = {0};
	char port[20] = {0};
	int h = 0;
	int p = 0;
	char n = 'n';
	printf("Hello\n");
	read(configF, fileCont, size);
	for(i=0;i<size;i++)
	{
		if(fileCont[i] == '\n')
		{
			if(n == 'm')
			{
				break;
			}
			else
			{
				n = 'm';
			}
		}
		else if(n == 'n')
		{
			printf("%c", fileCont[i]);
			host[h] = fileCont[i];
			h++;
		}
		else if(n != 'n')
		{
			port[p] = fileCont[i];
			p++;
		}
	}
	printf("host is: %s\n", host);
	printf("port is: %s\n", port);
	portn = atoi(port);
	server = gethostbyname(host);
	if (server == NULL) 
	{
		printf("Can't find server");
		exit(1);
	}
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	memcpy(&serverAddress.sin_addr.s_addr, server->h_addr, server->h_length);
	serverAddress.sin_port = htons(portn);
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(clientSocket,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) < 0)
	{
		printf("Can't connect");
	}
	return clientSocket;
}

///////////////////////////////////////END SOCKET FUNCTIONS

void writeFile(const char* path, char* input) {
	
	int fd = open(path, O_CREAT | O_RDWR | O_APPEND, S_IWUSR | S_IRUSR);
	write(fd, input, strlen(input));
}

void configure (char* host, char* port) { // ./WTFClient configure <ip/host> <port> 

	printf("Attempting Configure \n");
	//char* host = argv[2];
	printf("Host is %s \n", host);
	//char* port = argv[3];
	printf("Port is %s \n", port);
	//char* hostport = host + port;
			
	//writes to configure file
	//currently CANT write to .configure file (dunno why? cant name files with a . in front?? 
	char* cpath = "./.configure";
	printf("Host and Port stored in %s \n", cpath);
	//write host and port to configure file
	writeFile(cpath,host);
	writeFile(cpath, "\n");
	writeFile(cpath,port);
	writeFile(cpath, "\n");
	//configureflag = 1;
	printf("Configure Success \n");

}


static void newManifest(const char * dir_name, char* mpath) {
    
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

		if(entry->d_name[0] == '.') { //if it starts with . DONT PUT INTO MANIFEST
			printf("%s is not added to manifest \n", entry->d_name);
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

void deleteFromManifest(const char* dir_name, char* filename, char* mpath) {

	DIR * d;
    	d = opendir (dir_name);

    	if (! d) {
        	fprintf (stderr, "Cannot open directory '%s': %s\n", dir_name, strerror (errno));
        	exit (EXIT_FAILURE);
    	}
    
	char* clientmanifeststring = fileToString(mpath); 
	//printf("THIS IS THE MANIFEST STRING: %s", manifeststring); 
	int clientsize = manifestSize(clientmanifeststring);
	//printf("\n\n%i\n\n", size);
	manifest* clientMF;
	clientMF = malloc(clientsize*sizeof(manifest));
	readManifest(clientmanifeststring, clientsize, clientMF);

	char tempmpath[50];
	strcpy(tempmpath, mpath);
	strcat(tempmpath, "temp");
	
	int i;
	char found = 'x';
	//printf("Filepath given: %s",filename);
 	for(i = 0; i<clientsize; i++) {
		if(strcmp(filename, clientMF[i].path) == 0) {//filename to delete found in manifest, DONT ADD TO NEW MANIFEST
			//printf("found\n");
			found = 'o';	
		} else {
		writeFile(tempmpath, clientMF[i].path);
		writeFile(tempmpath, "\t");
		writeFile(tempmpath, clientMF[i].version);
		writeFile(tempmpath, "\t");
		writeFile(tempmpath, clientMF[i].hash);
		writeFile(tempmpath, "\n");
		}
	}
	
	for(i = 0; i<clientsize; i++)
	{
		free(clientMF[i].path);
		free(clientMF[i].version);
		free(clientMF[i].hash);
	}
	free(clientMF);

	char* newManifest = fileToString(tempmpath);
	removeFile(tempmpath); 	//delete .Manifesttemp
	removeFile(mpath);	//delete .Manifest
	writeFile(mpath, newManifest); //create .Manifest w/o that deleted file
	
	if(found == 'x') {
		printf("%s was not found, manifest not changed\n",filename);
	} else {
		printf("%s was successfully removed\n",filename);
	}
	
	return;

}

	  


void readFile(char* filename) { // ./WTFClient read <filename>
	
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

static void listFiles(const char * dir_name) {
    
	DIR * d;

    	d = opendir (dir_name);

    	if (! d) {
        	fprintf (stderr, "Cannot open directory '%s': %s\n", dir_name, strerror (errno));
        	exit (EXIT_FAILURE);
    	}
    
	while (1) {
        	struct dirent * entry;
        	const char * d_name;

        	entry = readdir (d);
        	if (! entry) {
           		break;
        	}
        	d_name = entry->d_name;
        
		if(entry->d_type & DT_DIR) {
		
		} else {	
		printf("%s/%s\n", dir_name, d_name);
		//printf("%s\n", d_name);
		//char * filepath = (char*)dir_name;
		//strcat(filepath, "/");
		//strcat(filepath, d_name);
		//printf("This is the string: %s", filepath);
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
                /* Recursively call "listFiles" with the new path. */
                listFiles (path);
            }
	}
    }
    /* After going through all the entries, close the directory. */
    if (closedir (d)) {
        fprintf (stderr, "Could not close '%s': %s\n",dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
}

void listDirs(const char * dir_name) {
    
	DIR * d;

    	d = opendir (dir_name);

    	if (! d) {
        	fprintf (stderr, "Cannot open directory '%s': %s\n", dir_name, strerror (errno));
        	exit (EXIT_FAILURE);
    	}
    
	while (1) {
        	struct dirent * entry;
        	const char * d_name;

        	entry = readdir (d);
        	if (! entry) {
           		break;
        	}
        	d_name = entry->d_name;
        
		if (entry->d_type & DT_DIR) {

            		printf("DIR FOUND! %s",entry->d_type);
                }
                /* Recursively call "listFiles" with the new path. */
                     
	}
    
    if (closedir (d)) {
        fprintf (stderr, "Could not close '%s': %s\n",dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
}


void removeFile(char* filename) { // ./WTFClient remove <filename>
	
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
	void *buffer = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);

	//char* input = (char*)malloc(sizeof(char)*1000); 
	//strcat(input, buffer);
	return buffer;
}

int isdir(const char* path) { //if this returns 1 it is a DIR if 0 it is NOT
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISDIR(path_stat.st_mode);
}

/*
int fileExist (char* filename) {//checks if a file exists

	int file; 

	// Open file 
	file = open(filename, O_RDONLY); 
	if(file >= 0)
	{ 
        	return -1;
	}  
	else
	{
	close(file); 
	return 1;
	}
}
*/

int fileExist (char* filename) {
   
   	FILE *fptr; 
   
    	// Open file 
   	fptr = fopen(filename, "r"); 
   	if (fptr == NULL) { 
		//printf("File does not exist, check file path\n");
       		 return -1;
    	}  
  
    	fclose(fptr); 
    	return 1;
}


int dirExist (char* dir_name) {
	DIR * d;
    	d = opendir (dir_name);

    	if (! d) {
        	//fprintf (stderr, "Cannot open directory '%s': %s\n", dir_name, strerror (errno));
        	//exit (EXIT_FAILURE);
        	return -1;
    	}

	closedir(d);
	return 1;
}

int clientdir(const char* dir_name, char* projectname) {
    
	DIR * d;
    	d = opendir (dir_name);

    	if (! d) {
        	fprintf (stderr, "Cannot open directory '%s': %s\n", dir_name, strerror (errno));
        	exit (EXIT_FAILURE);
    	}
    
	while (1) {
        	struct dirent * entry;
        	const char * d_name;

        	entry = readdir (d);
        	if (! entry) {
           		break;
        	}

	        d_name = entry->d_name;
 		if (entry->d_type & DT_DIR) {

                  
            	if(strcmp(d_name, projectname) == 0) {
				//printf("Project: %s FOUND in client side | checkout NOT VALID\n", projectname);
				closedir(d);
				return 1;
		}
                /* Recursively call "newManifest" with the new path. */
          
		}
	}
		

	closedir(d);
	return -1;
}




int main(int argc, char* argv[]) {

	//this directory struct will be used to search for files in directories later.
/*
	struct dirent* de;
	DIR* dr = opendir(".");
	if(dr == NULL) {
		printf("Directory not found");
		return 0;
	} 
	closedir(dr);
*/	

//manifest test
	if(strcmp(argv[1], "manifest") == 0) {
		if(argc == 3) {
			char* dirname = argv[2];
			
			char manifestpath[100];
			//char manifestpath[50];
			strcpy(manifestpath, dirname);
			strcat(manifestpath, "/.Manifest");
			printf("Manifest path: %s\n", manifestpath); 
			printf("Filename: %s\n", dirname);
			
			//check if .Manifest is made if it is - remove it and make a new one 
			if(fileExist(manifestpath) == 1) {
				removeFile(manifestpath);
			} 
			       
			newManifest(dirname, manifestpath);
					
			return 0;
		} else {
			printf("manifest syntax: ./WTFClient manifest <filename>\n");		
			return 0;
		}
	}else if(strcmp(argv[1], "list") == 0) {
		if(argc == 3) {
			char* dirpath = argv[2];
			listFiles(dirpath);
			return 0;
		} else {
			printf("list syntax: ./WTFClient list <dirpath> \n");		
			return 0;
		}
	}else if(strcmp(argv[1], "dirs") == 0) {
		if(argc == 3) {
			char* dirpath = argv[2];
			listDirs(dirpath);
			return 0;
		} else {
			printf("list syntax: ./WTFClient list <dirpath> \n");		
			return 0;
		}

	} else if(strcmp(argv[1], "read") == 0) {
		if(argc == 3) {
			char* filename = argv[2];
			readFile(filename);
		} else {
			printf("read syntax: ./WTFClient read <filename> \n");
			return 0;
		}
	} else if(strcmp(argv[1], "delete") == 0) {
		if(argc == 3) {
			char* filename = argv[2];
			removeFile(filename);
		} else {
			printf("remove Syntax: ./WTFClient delete <filename> \n");
		} 
	} else if(strcmp(argv[1], "mfstruct") == 0) {
		if(argc == 3) {
			char* manifestpath = argv[2];
			//use manifest to struct function
			char* manifeststring = fileToString(manifestpath); 
			printf("THIS IS THE MANIFEST STRING: %s", manifeststring); 
			int size = manifestSize(manifeststring);
			printf("\n\n%i\n\n", size);
			manifest* MFFromFile;
			MFFromFile = malloc(size*sizeof(manifest));
			readManifest(manifeststring, size, MFFromFile);

			int i;
			printf("\n");
			for(i = 0; i < size; i++) {
				printf("%s\n", MFFromFile[i].path);
				printf("%s\n", MFFromFile[i].version);
				printf("%s\n", MFFromFile[i].hash);
				printf("\n");
			}
	
			for(i = 0; i<size; i++)
			{
				free(MFFromFile[i].path);
				free(MFFromFile[i].version);
				free(MFFromFile[i].hash);
			}
			free(MFFromFile);
			return 0;
		}
	}
	//here starts all the project commands
	else if(strcmp(argv[1],"configure") == 0) { //3.0
		if(argc == 4) {   
			char* host = argv[2];
			char* port = argv[3];
			removeFile(".configure");
			configure(host, port);
		} else {
			printf("configure Syntax: ./WTFClient configure <ip/hostname> <port> \n");
			return 0;
		}
	}else if(strcmp(argv[1],"checkout") == 0) { //3.1
		if(argc == 3) {
			char* projectname = argv[2];
			printf("You made it to checkout \n");
			//check for proper server connection
			//check that projectname exists on server (maybe from checking manifest?)
			//check that projectname DOES NOT exist on clientside
			if(clientdir(".",projectname) == 1) {
				printf("Project: %s FOUND in client side | checkout NOT VALID\n", projectname);
				return 0;
			} 			

		//check that configure has been run! check that .configure exists in this directory
			
			//printf("Fileexist return: %x", fileExist("/configure"));
			if(fileExist(".configure") == -1)	{
				printf(".configure file NOT FOUND | checkout NOT VALID\n"); 
				return 0;
			}
			
			//do checkout stuff 
			int socket = socketConnect();
			memset(buffer, 0, 1024);
			strcpy(buffer, argv[1]);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			strcpy(buffer, projectname);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			strcat(projectname, ".tar.gz");
			recieveTar(socket, projectname);
			///at this point, client recieves tarred project file, and it upzips
		
			
			//delete the tar
			if(fileExist(projectname) == 1) 
				removeFile(projectname); 
	
			return 0;
		} else {
			printf("checkout syntax: ./WTFClient checkout <filename>");
		}
	} else if(strcmp(argv[1], "update") == 0) { //3.2
		if(argc == 3) {
			char* projectname = argv[2];
			printf("You made it to update \n");
			
			//error checking here
			//if project doesnt EXIST on server, fail
			//-server sends error message
			//if client cant communicate
			//-client gets cant connect to server error message
			//if .configure was not run 
			if(fileExist(".configure") ==  -1)	{
				printf(".configure file NOT FOUND | update NOT VALID\n"); 
				return 0;
			}
			//client MUST have the local project- otherwise it cant compare its .Manifest with the servers!
			if(dirExist(projectname) == -1) {
				printf("%s NOT FOUND | CANT UPDATE WITHOUT LOCAL PROJECT", projectname);
				return 0;
			}

			//do update stuff
			//ask for .Manifest of server side project name
			int socket = socketConnect();
			memset(buffer, 0, 1024);
			strcpy(buffer, argv[1]);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			strcpy(buffer, projectname);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			
			char mpath[50];
			strcpy(mpath,projectname);
			strcat(mpath,"/.Manifest");

			char updatepath[50];
			strcpy(updatepath, projectname); 
			strcat(updatepath, "/.Update");


			strcat(projectname, ".servermanifest");
			recieveFile(socket, projectname);
			
			printf("Recieved [%s] from server \n");
			printf("Beginning to compare...\n"); 

			//compare manifests

			char* clientmanifeststring = fileToString(mpath); 
			//printf("THIS IS THE MANIFEST STRING: %s", manifeststring); 
			int clientsize = manifestSize(clientmanifeststring);
			//printf("\n\n%i\n\n", size);
			manifest* clientMF;
			clientMF = malloc(clientsize*sizeof(manifest));
			readManifest(clientmanifeststring, clientsize, clientMF);

			char* servermanifeststring = fileToString(projectname); 
			//printf("THIS IS THE MANIFEST STRING: %s", manifeststring); 
			int serversize = manifestSize(servermanifeststring);
			//printf("\n\n%i\n\n", size);
			manifest* serverMF;
			serverMF = malloc(serversize*sizeof(manifest));
			readManifest(servermanifeststring, serversize, serverMF);

			int i;
			int j;
			char found = 'x';
			char emptyUpdate = 'o';
			printf("\n");
			//prints out all the file paths, versions, and hashes
			for(i = 0; i < clientsize; i++) {
				for(j = 0; j < serversize; j++) {
					if(strcmp(clientMF[i].path,serverMF[j].path) == 0) { //SAME FILE
						
						found = 'o';
						//SAME FILE, SAME VERSION, HASH DIFFERENT = UPLOAD
						if(strcmp(clientMF[i].hash, serverMF[j].hash) != 0) { //different hash
						if(strcmp(clientMF[i].version, serverMF[j].version) == 0) { //same path version 
						
							
							//printf("DIFF HASH HAS BEEN FOUND BETWEEN 2 SAME FILENAMES\n");
							writeFile(updatepath, "U");
							writeFile(updatepath, "\t");
							writeFile(updatepath, clientMF[i].path);
							writeFile(updatepath, "\t");
							writeFile(updatepath, clientMF[i].version);
							writeFile(updatepath, "\t");
							writeFile(updatepath, clientMF[i].hash);
							writeFile(updatepath, "\n");
							emptyUpdate = 'x';
						}
						}
						//SAME PATH, SAME HASH, DIFFERENT VERSION CODE = MODIFY
						//if(strcmp(clientMF[i].version, serverMF[j].version) != 0) { //different version
						if(strcmp(clientMF[i].hash, serverMF[j].hash) != 0) { //same hash
						if(strcmp(clientMF[1].version, serverMF[1].version) != 0) { //different manifest version
							//printf("DIFF VERSION FOUND");
							writeFile(updatepath, "M");
							writeFile(updatepath, "\t");
							writeFile(updatepath, clientMF[i].path);
							writeFile(updatepath, "\t");
							writeFile(updatepath, clientMF[i].version);
							writeFile(updatepath, "\t");
							writeFile(updatepath, clientMF[i].hash);
							writeFile(updatepath, "\n");
							emptyUpdate = 'x';
						}
						}
						//}
									
					}
				}
				if(found == 'x') { //CLIENT FILE NOT IN SERVER = UPLOAD
					if(strcmp(clientMF[1].version, serverMF[1].version) == 0) { //CLIENT FILE NOT IN SERVER | MANIFEST VERSIONS SAME = UPLOAD
					writeFile(updatepath, "U");
					writeFile(updatepath, "\t");
					writeFile(updatepath, clientMF[i].path);
					writeFile(updatepath, "\t");
					writeFile(updatepath, clientMF[i].version);
					writeFile(updatepath, "\t");
					writeFile(updatepath, clientMF[i].hash);
					writeFile(updatepath, "\n");
					emptyUpdate = 'x';
					} else { //CLIENT FILE NOT IN SERVER | MANIFEST VERSIONS DIFFERENT = DELETED
					writeFile(updatepath, "D");
					writeFile(updatepath, "\t");
					writeFile(updatepath, clientMF[i].path);
					writeFile(updatepath, "\t");
					writeFile(updatepath, clientMF[i].version);
					writeFile(updatepath, "\t");
					writeFile(updatepath, clientMF[i].hash);
					writeFile(updatepath, "\n");
					emptyUpdate = 'x';	
					} 
				}
				
				found = 'x';
			}
			found = 'x';
			for(i = 0; i < serversize; i++) {
				for(j = 0; j < clientsize; j++) {
					if(strcmp(serverMF[i].path, clientMF[j].path) == 0) {
						found = 'o';

						
					}
				}
				if(found == 'x') { //SERVER HAS FILE, CLIENT DOES NOT = ADDED
					if(strcmp(serverMF[1].version, clientMF[1].version) != 0) { //DIFFERENT MANIFEST VERSION
						writeFile(updatepath, "A");
						writeFile(updatepath, "\t");
						writeFile(updatepath, clientMF[i].path);
						writeFile(updatepath, "\t");
						writeFile(updatepath, clientMF[i].version);
						writeFile(updatepath, "\t");
						writeFile(updatepath, clientMF[i].hash);
						writeFile(updatepath, "\n");
					}
				}
				found = 'x';
			}

			
			//frees the clientMF and serverMF structs
			for(i = 0; i<clientsize; i++)
			{
				free(clientMF[i].path);
				free(clientMF[i].version);
				free(clientMF[i].hash);
			}
			free(clientMF);
			for(i = 0; i<serversize; i++)
			{
				free(serverMF[i].path);
				free(serverMF[i].version);
				free(serverMF[i].hash);
			}
			free(serverMF);
			
			if(emptyUpdate == 'o') {
				printf("Up to date\n");
				writeFile(updatepath, "");
			} else {
				readFile(updatepath);
			}


			return 0;
		} else { 
			printf("update syntax: ./WTFClient update <filename>");
		}
	} else if(strcmp(argv[1], "upgrade") == 0) { //3.3
		if(argc == 3) {
			char* projectname = argv[2];
			printf("You made it to upgrade \n");
			
			//error checking here
			//if projectname does not exist on server
			//-server sends error message upgrade does not happen

			//if .Update does not exist
			char updatepath[50];
			strcpy(updatepath, projectname);
			strcat(updatepath, "/.Update");
			if(fileExist(updatepath) != 1) {
				printf(".Update does not exist!");
				return 0;
			}

			//if .Update is empty 
			
			printf(".Update is empty!");
			return 0;

			//do upgrade stuff
			//char* updatestring = fileToString(updatepath);

			return 0;
		} else { 
			printf("upgrade syntax: ./WTFClient upgrade <filename>");
		}
	} else if(strcmp(argv[1], "commit") == 0) { //3.4
		if(argc == 3) {
			char* projectname = argv[2];
			printf("You made it to commit \n");
			
			//error checking here
			//cant communicate with server
			//-error message shows up

			//if project/.Manifest can't be fetched from server
			//-server sends error message and commit does not happen

			//if projectname does not exist on server
			//-server sends message that project does not exist and commit does not happen

			//if .Update 

			//do commit stuff
			return 0;
		} else { 
			printf("commit syntax: ./WTFClient commit <filename>");
		}
	} else if(strcmp(argv[1], "push") == 0) { //3.5
		if(argc == 3) {
			char* projectname = argv[2];
			printf("You made it to push \n");

			

			char updatepath[50];
			strcpy(updatepath, projectname);
			strcat(updatepath, "/.Update"); 
			
			//error checking here
			//cant communication with server
			//-error message shows up
			
			//if project does not exist on client side
			struct stat st;
			if(stat(projectname,&st) == -1) {
				printf("%s does not exist on client side",projectname);
				return 0; 
			}

			/*
			//if .Update has any "M" action codes
			//printf("UPDATE PATH: %s \n", updatepath);
			char* updatestring = fileToString(updatepath); 
			//printf("UPDATE STRING: %s \n", updatestring); 
			
			int i;
			//printf("we good\n");
			//printf("%c FIRST CHAR", updatestring[0]);
			if(updatestring[0] == 'M') {
				printf("M action code found in .Update Cannot push");
				return 0;
			}
			
			for(i = 0; updatestring[i] != '\0'; i++) {
				
				if(updatestring[i] == '\n') {
					//printf("new line char found\n");
					if(updatestring[i+1] == 'M') {
						printf("M action code found in .Update Cannot push\n");
						return 0;
					}
				}
			}
			*/
			//do push stuff
			int socket = socketConnect();
			printf("socket stuff\n");
			memset(buffer, 0, 1024);
			strcpy(buffer, argv[1]);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			strcpy(buffer, projectname);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			//strcat(projectname, ".tar.gz");
			tarAndSend(projectname, socket);
			
			strcat(projectname, ".tar.gz");
			removeFile(projectname);

			return 0;
		} else { 
			printf("push syntax: ./WTFClient push <filename>");
		}
	} else if(strcmp(argv[1], "create") == 0) { //3.6
		if(argc == 3) {
			char* projectname = argv[2];
			//printf("You made it to create \n");
			
			//error checking here
			//if client cant communicate with server
			if(fileExist(".configure") ==  -1)	{
				printf(".configure file NOT FOUND | create NOT VALID\n"); 
				return 0;
			}

			//-sends cant connect to server error message
			//if projectname EXISTS on server side
			//-server sends message that it EXISTS and can't create

			int socket = socketConnect();
			memset(buffer, 0, 1024);
			strcpy(buffer, argv[1]);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			strcpy(buffer, projectname);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			read(socket, buffer, 1024);
			printf("%s", buffer);

			//create local directory of project and places EMPTY .Manifest that server sent
			mkdir(projectname, 0700);
			char mpath[50];
			strcpy(mpath,projectname);
			strcat(mpath,"/.Manifest");
			writeFile(mpath, "");

			return 0;
		} else { 
			printf("create syntax: ./WTFClient create <filename>");
		}
	} else if(strcmp(argv[1], "destroy") == 0) { //3.7
		if(argc == 3) {
			char* projectname = argv[2];
			//printf("You made it to destroy \n");
			
			//error checking here
			//if projectname DOES NOT EXISTS on server side
			//-server sends message that it DOES NOT EXISTS and can't destroy
			//if client cant communicate with server
			if(fileExist(".configure") ==  -1)	{
				printf(".configure file NOT FOUND | destroy NOT VALID\n"); 
				return 0;
			}



			int socket = socketConnect();
			memset(buffer, 0, 1024);
			strcpy(buffer, argv[1]);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			strcpy(buffer, projectname);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			read(socket, buffer, 1024);
			printf("%s", buffer);

			return 0;
		} else { 
			printf("destroy syntax: ./WTFClient destroy <projectname>");
		}

	} else if(strcmp(argv[1], "add") == 0) { //3.8
		if(argc == 4) {
			char* projectname = argv[2];
			char* filename = argv[3];
			printf("You made it to add \n");
			
			//error checking here
			//if project doesnt exists on client
			struct stat st;
			
			
			if(stat(projectname,&st) == -1) {
				printf("%s does not exist on client side",projectname);
				return 0; 
			}
			
			//
			//
			
			/*
			//the old add
			int socket = socketConnect();
			memset(buffer, 0, 1024);
			strcpy(buffer, argv[1]);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			strcpy(buffer, projectname);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			//strcat(projectname, ".tar.gz");
			tarAndSend(projectname, socket);
			
			strcat(projectname, ".tar.gz");
			removeFile(projectname);
			*/
			char filepath[50];
			strcpy(filepath, projectname);
			strcat(filepath, "/");
			strcat(filepath, filename);
			
			char mpath[50];
			strcpy(mpath, projectname);
			strcat(mpath, "/.Manifest");
			//printf("mpath: %s | Filename: %s | Filepath: %s\n", mpath, filename, filepath);

			writeFile(mpath, filepath);
			writeFile(mpath, "\t");
			
			writeFile(mpath, "1");
			writeFile(mpath, "\t");
			
			//empty string hash
			writeFile(mpath, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
			writeFile(mpath, "\n"); 
	
			 
			printf(".Manifest is updated\n"); 
			

			return 0;
		} else { 
			printf("add syntax: ./WTFClient add <projectname> <filename>");
		}
	} else if(strcmp(argv[1], "remove") == 0) { //3.9
		if(argc == 4) {
			char* projectname = argv[2];
			char* filename = argv[3];
			printf("You made it to remove \n");
			
			//error checking here
			//if project doesnt exists on client
			struct stat st;
			
			if(stat(projectname,&st) == -1) {
				printf("%s does not exist on client side",projectname);
				return 0; 
			}
			
			char filepath[50];
			strcpy(filepath, projectname);
			strcat(filepath, "/");
			strcat(filepath, filename);
			
			char mpath[50];
			strcpy(mpath, projectname);
			strcat(mpath, "/.Manifest");

			//deleteManifest
			deleteFromManifest(projectname, filepath, mpath);
			
	
			return 0;
		} else { 
			printf("remove syntax: ./WTFClient remove <projectname> <filename>");
		}
	} else if(strcmp(argv[1], "currentversion") == 0) { //3.10
		if(argc == 3) {
			char* projectname = argv[2];
			//printf("You made it to currentversion \n");
			
			//error checking here
			//if project doesnt exists, the server sends message that directory was not found
			int socket = socketConnect();
			memset(buffer, 0, 1024);
			strcpy(buffer, argv[1]);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);
			strcpy(buffer, projectname);
			write(socket, buffer, 1024);
			memset(buffer, 0, 1024);

			char cvpath[50];
			strcpy(cvpath,projectname);
			strcat(cvpath, ".CV");
			strcat(projectname, ".CV.tar.gz");
			//recieveFile(socket, projectname);
			recieveTar(socket, projectname);
	
			printf("\n");
			printf("tar file recieved and untarred");
			printf("\n");
			printf("Reading Current Version File\n");
			
			readFile(cvpath);
			printf("\n");
			removeFile(projectname);
			removeFile(cvpath);
	
			return 0;
		} else { 
			printf("currentversion syntax: ./WTFClient currentversion <projectname>");
		}
	} else if(strcmp(argv[1], "history") == 0) { //3.11
		if(argc == 3) {
			char* projectname = argv[2];
			//printf("You made it to history \n");
			
			//error checking here
			//if project doesnt exists, the server sends message that directory was not found
			
			
	
			return 0;
		} else { 
			printf("history syntax: ./WTFClient history <projectname>");
		}
	} else if(strcmp(argv[1], "rollback") == 0) { //3.12
		if(argc == 3) {
			char* projectname = argv[2];
			//printf("You made it to rollback \n");
			
			//error checking here
			//if project doesnt exists, the server sends message that directory was not found
			
			
	
			return 0;
		} else { 
			printf("rollback syntax: ./WTFClient rollback <projectname>");
		}

/////////////////////////////////////////
	} else {
		printf("Your command: [%s] was not recognized \n",argv[1]);
		return 0; 
	}


	
	return 0;
}
