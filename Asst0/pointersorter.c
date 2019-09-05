/*
Sorts in DESCENDING ORDER
./pointersorter “thing stuff otherstuff blarp”
./pointersorter “thing1stuff3otherstuff,blarp"
(white space or any NON-alphabet letter can be a separator)

thing
stuff
otherstuff
blarp

Another Example: marching > march
Z > A > z > a

Zoology
Zoo
Apple
zoo
apple 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//global variable declarations
char *input;
int i = 0;
int j = 0;
//int n = 0;
int start;
int end;
int wordLength;

//C Struct for Linked List Node
struct Node {
	char *value;
	struct Node *next;
};

//starts making tokens containing the start and end point of the word
//if alphabetic char is not found, loop breaks and acts as a "seperator"

char *tokenizer(char *tempstr){
	while(input[i] != '\0'){
		if(isalpha(input[i])){
			i++;
		} else {
			break;
		}
	}

	end = i;
	wordLength = end - start + 1;
	char *token = (char*)malloc((wordLength)*(sizeof(char)));
	
	int lastElement = 0;

	for(j = start; j < end; j++,lastElement++){
		token[lastElement] = tempstr[j];
	}
	
	token[lastElement] = '\0';
	return token;
}

void swap(char** word, int i, int j){
	char* temp = word[i];
	word[i] = word[j];
	word[j] = temp;
}


//Partitions the array which places all the words less than pivot to the left and greater words to the right.

int partition (char** words, int start, int end){
	char* ptr = words[end];
	int i = (start - 1);
 
	for (j = start; j <= end - 1; j++){

//		if(isupper(words[j]) && strcmp(words[j],ptr) < 0) {
//			
//			i++;
//			swap(words,i,j);
//		}
		if (strcmp(words[j],ptr) > 0){		
			i++;
			swap(words,i,j);
		}
			
	}
	swap(words,i+1,end);
	return (i + 1);
}



//Recursively partitions the array. Takes input array as first parameter.
//Takes start points and end point as parameters. 

void quickSort(char ** input, int start, int end){
	if (start < end){
		int p = partition(input, start, end); 
		quickSort(input, start, p - 1);  
		quickSort(input, p + 1, end);
	}
}

int main(int argc, char **argv){
	if (argc != 2) {
		fprintf(stderr, "ERROR: Wrong input!\n");
		return 1;
	}

	input = argv[1];
	char *input_str = argv[1];
	int inputLength = strlen(input_str);

	struct Node *p;
	struct Node *head = 0;
	struct Node *tail = 0;
	struct Node *next;
	int n = 0;
	
	while(input[i] != '\0' && i < inputLength){
		start = i;
		char *output = tokenizer(input_str);
		if(strlen(output) <= 0){
			free(output);
			i++;
			continue;
		}
		p = (struct Node *)malloc(sizeof(struct Node));
		n++;
		p->value = output;
		p->next = 0;
		if(head == 0){
			head = p;
			tail = p;
		}else{
			tail->next = p;
			tail = p;
		}

		i++;
	}
	char** strings = (char**)malloc(n*sizeof(char*));
	n = 0;
	int numWords = 0;
	for(p = head; p != 0 ; p = p->next, n++){
		strings[n] = p->value;
		numWords++;
	}

	if(numWords == 0){
		fprintf(stderr, "ERROR: Blank String!\n");
		free(strings);
		return 1;
	}

  	quickSort(strings, 0,numWords-1);
   	for(n = 0; n < numWords; n++ ){
		printf("%s\n", strings[n]);
		free(strings[n]);
   	}

//free allocated memory
  	free(strings);
  	for (p = head; p != 0; p = next){
		next = p->next;
		free(p);
   }

	return 0;
}
