
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pos = 0;
int largestWord = 0;

struct Dimensions //used to store the dimensions of an array broken up by non alphabetical characters
{
	int length;
	int width;
};
typedef struct wordFreq //data structure that acts as both the nodes in the priority queue and the leafs of the tree. This is a bit of an abstract way of doing the huffman code, but I think it will remove redundant entries in memory, as well as be easier to keep track of, at the cost of space
{
	char *word;
	int freq;
	int depth;
	char *code;
	struct wordFreq *next;
	struct wordFreq *left;
	struct wordFreq *right;
} wordFreq;
typedef struct codeEntry
{
	char *word;
	char *code;
} codeEntry;
struct Dimensions getDimensions(char input[]) //function that accepts the input string argv, and finds out exactly how many sequences of alphabetical characters there are, and how long the largest string of them is. With this information, allocating memory for the array will be much easier
{
	int i;
	struct Dimensions D;
	int onlySeparators = 0;
	int word = 0;
	int numSeparators = 0;
	int onlyAlpha = 0;
	int separatorSequence = 1;
	for(i = 0; input[i] != 0; i++)
	{
		//if(isalpha(input[i]) == 0) 
		if(input[i] == '\n' || input[i] == '\t' || input[i] == ' ')
		{
			//onlyAlpha = 1;
			if(word > 0)
			{
				numSeparators++;
			}
			if(word == 0)
			{
				word++;
				numSeparators++;
			}
			if(word > largestWord)
			{
			//	printf("largest Word: %i\n", largestWord);
			//	printf("Word: %i\n", word);
				largestWord = word; //This tries to keep track of the largest sequence of alphabetical characters, so that the string array can be properly sized
			}
			word = 0;
			if(separatorSequence == 0)
			{
				numSeparators++;	//This tracks how many times a non aplhabetical character or series of non alphabetical characters appears between alphabetical characters	
			//	printf("Number of Separators: %i", numSeparators);
			//	printf("\n");	
			}
			separatorSequence = 1;
		}
		else
		{
			//printf("LetterFound\n");
			separatorSequence = 0;
			if(onlySeparators == 1)
			{
				onlySeparators = 0; //This shows the code that the input string has at least 1 alphabetical character, meaning the program won't throw an error
			}
			
			word++;
		//	printf("Word: %i\n", word);

		}
	}
		//if(isalpha(input[i-1] == 0))
		if(input[i-1] == '\n' || input[i-1] == '\t' || input[i-1] == ' ')
		{
			numSeparators--;
		}

		//printf("LAST THING READ: %crtyerty", input[i-1]);
		 
		if(onlyAlpha = 0 && largestWord == 0)
		{
			largestWord = word;
		}
		if(word > largestWord)
			{
			//	printf("largest Word: %i\n", largestWord);
			//	printf("Word: %i\n", word);
				largestWord = word; //This tries to keep track of the largest sequence of alphabetical characters, so that the string array can be properly sized
			}

		if(onlySeparators == 1)
		{
		//	printf("No alphabetical character in input string, please try agian");
		}
		else
		{
		D.length = largestWord;
		D.width = numSeparators+1;
		//printf("\n\n\n\n%i\n\n\n\n", largestWord);
		}
		return D;
}
/**int strcmpTranslated(const void *a, const void *b)		failed attempt to make a wrapper for strcmp to work with Qsort
{
	char const *elementA = char const *a;
	char const *elementB = char const *b;
	return strcmp(elementA, elementB);
}*/

int makeCodes(wordFreq *current, char* code, int depth, codeEntry *codeBook, int whichCode) //This recursive method finds the binary value that the tree will process to find words in the 
{
	if(current->left != NULL)
	{
//		printf("Going left\n");
		code[depth] = '0';
		makeCodes(current->left, code, depth+1, codeBook, whichCode);
	}
	if(current->right != NULL)
	{
//		printf("going right");
		code[depth] = '1';
		makeCodes(current->right, code, depth+1, codeBook, whichCode);
	}
	if(current->left == NULL && current->right == NULL)
	{
	//	printf("%s: ", current->word);
		int i;
//		printf("\n\n Value of Which Code: %i\n\n", whichCode);
		codeBook[pos].word = strdup(current->word);
		codeBook[pos].code = strdup(code);
		pos++;
		for (i = 0; i < depth; ++i) 			
		{						
	//		printf("%c", code[i]);  		
    		}								
	//	printf("\n");											

	}
}
codeEntry *readCodeBook(char *input)
{
//	printf("reading");
	int i = 0;
	int bsize = 0;
	int curr = 0;
	char read = 'c';
	int wPos = 0;
	char temp[20]={"\0"};
	for(i = 0; input[i]!= 0;i++)
	{
		if(input[i] == '\n')
		{
			bsize++;
		}
	}
	codeEntry* readCB = malloc(bsize * sizeof(codeEntry));
	for(i = 0; input[i]!=0;i++)
	{
		if(input[i] == '\n')
		{
		//	printf("moving to write code\n");
			curr++;
			read = 'c';
			pos = 0;
		}
		else if(input[i] == '\t')
		{
			readCB[curr].code =  strdup(temp);
			memset(temp, 0,sizeof(temp));
		//	printf("Moving to write word: Code is:%s\n", readCB[curr].code);
			curr++;
			read = 'w';
			pos = 0;
		}
		else
		{
			if(read == 'c')
			{
				
				temp[pos] = input[i];
			//	readCB[curr].code[pos] = input[i];
				pos++;
			}
			if(read == 'w')
			{
				temp[pos] = input[i];
			//	readCB[curr].word[pos] = input[i];
				pos++;
			}
		}
	}
	return readCB;	
}
char* makeCodeBook(codeEntry *codeBook) //iteratively prints the values of the codebook
{
	int i;
	char *output = malloc(sizeof(char)*500);
	output[0] = '\0';
	for(i = 0; i<pos;i++)
	{	
		//if(codeBook[i].word == " ")														//
		//{				
	//		printf("(Space character)");												     //			
//		}																	//THIS IS WHERE THE CODEBOOK IS PRINTED
		printf("%s\t%s\n", codeBook[i].code, codeBook[i].word);
		strcat(output, codeBook[i].code);
		strcat(output, "\t");
		strcat(output, codeBook[i].word);
		strcat(output, "\n");								//
	}
	return output;
}
void newEntry(wordFreq *current, char* wordTemp)//creates a new wordFreq entry at the end of the linked list
{
	wordFreq *newEntry = (wordFreq *)malloc(sizeof(wordFreq));
	newEntry->word = strdup(wordTemp);
	newEntry->freq = 1;
	newEntry->next = current->next;
	newEntry->left = NULL;
	newEntry->right = NULL;
	current->next = newEntry;
	return;
}
wordFreq moveNode(wordFreq *head, wordFreq* target)
{
	if(target == head)
	{
		head = head->next;
	}
}
char *getWord(codeEntry *codeBook, char* code)
{
	int count;

}
void freeTree(wordFreq *current)//recursively frees the leaves of the tree
{
	if(current == NULL)
	{
		return;
	}
	freeTree(current->left);
	freeTree(current->right);
	free(current->word);
	free(current->left);
	free(current->right);
	free(current->next);
	free(current);
}
void removeNodeFromQueue(wordFreq **head, wordFreq *target)//this function is used after a node is placed into the tree, but not yet removed from the queue. This happens because the data structre is the same, and whether or not its in whichever queue or tree is based on the pointers. If there's no 'next' pointer, then it has been removed from the queue, but if there are no 'left' or 'right' pointers, then the element isn't in the tree
{
	wordFreq *current;
	current = (*head);
//	printf("\nstarting removal");
	if(current == target)
	{
//		printf("\nTarget Word is head: %s", target->word);
		current = (*head)->next;
		(*head)->next = NULL;
		(*head) = current;
		return;
	}
	else if(target->next==NULL)
	{
		while(current->next!=target)
		{
			current = current->next;
		}
		current->next = NULL;
//		printf("\nTarget Word: %s", target->word);
		return;
	}
	else if(target->next!=NULL)
	{
		while(current->next!=target)
		{
			current = current->next;
		}
		current->next = target->next;
		target->next = NULL;
//		printf("\nTarget Word: %s", target->word);
	
		return;
	}
	else
	{
	//	printf("Target node not found");
		return;
	}
	
}
void newBranch(wordFreq **head, wordFreq *lowest, wordFreq *secondLowest, int totalFreq)//This method turns a priority queue entry into a leaf. It does this by creating a node with the two lowest values as children (lowest on the left, second lowest on the right), and inserting into the end of the priority queue. After it does this,
{
//	printf("\nmaking branch");
	wordFreq *newEntry = (wordFreq *)malloc(sizeof(wordFreq));
	newEntry->word = NULL;
	newEntry->freq = totalFreq;
	newEntry->left = lowest;
	newEntry->right = secondLowest;
	newEntry->next = NULL;
	wordFreq *current;
	current = *head;
	while(current->next!=NULL)
	{
		current = current->next;
	}
	current->next = newEntry;
	removeNodeFromQueue(head, lowest);
	removeNodeFromQueue(head, secondLowest);
}
void freeList(wordFreq *head)//frees all entries in the linked list starting from the head, mostly for debugging.
{
	wordFreq* temp;
	while(head!=NULL)
	{
//		printf("Freeing element\n");
		temp = head;
		head = head->next;
		free(temp);
	}
//	printf("List Free");
	return;
}
wordFreq *findFreq(int target, wordFreq *head, int sameValue) //accepts a frequency and returns with a pointer to the wordFreq node that has that frequency. This method has a quick failsafe in case there are two entries that share the same 'minimum' frequency, by way of the variable 'samevalue', which will tell the program to skip the first entry with that frequency if searching for the second lowest entry
{
	wordFreq *current;
	current = head;
	while(current != NULL)
	{
		//if(current-> freq == target && current->word!= NULL && sameValue == 0)
		if(current-> freq == target && sameValue == 0)
		{
//			printf("Node with target freqency found, word is: %s\n", current->word);
			return current;
		}
		//else if(current-> freq == target && current->word!= NULL && sameValue == 1)
		else if(current-> freq == target && sameValue == 1)
		{
			sameValue--;			
		}
		current = current->next;
	}
//	printf("Node not found with target frequency");
	return head;
}

int findLowest(wordFreq *head)//returns the integer value of the lowest frequency of all the elements in the linked list by iterating until the end and comparing the frequency values to one another. NOTE: This search goes 'left to right', starting from the head
{
	wordFreq *current;
	current = head;
	int lowest = current->freq;
	while(current!=NULL)
	{
		//if(current->freq < lowest && current->word!=NULL)
		if(current->freq < lowest)
		{
			lowest = current->freq;
		}
		current = current->next;
	}
//	printf("Found Lowest\n");
	return lowest;
}

int findSecondLowest(int lowest, wordFreq *head)//returns the integer value of the second lowest frequency of all the elements in the linked list. has a failsafe if there is a second entry who's frequency is the same as the lowest frequency that is found first, by comparing the address of the entry with the lowest number to the current entry, and skipping that one if they match. NOTE: this search goes 'left to right', starting from the head
{
	wordFreq *current;
	wordFreq *min = findFreq(lowest, head, 0);
	current = head;
	int sLowest;
	if(current->freq == lowest)
	{
		sLowest = current->next->freq;
	}
	else
	{
		sLowest = current->freq;
	}
	while(current!=NULL)
	{
	//	if(current->freq < sLowest && (current->freq!= lowest || current != min) && current->word!=NULL)
		if(current->freq < sLowest && (current->freq!= lowest || current != min))
		{
			sLowest = current->freq;
		}
		current = current->next;
	}
//	printf("Found Second Lowest: %i\n", sLowest);
	return sLowest;
}
wordFreq *huffmanCode(wordFreq *head)//this method organizes the priority queue into the binary search truee, finding the 2 least frequent values and making them leafs to a new branch. This method is run in a loop in 'stuff', until all the entries are placed into branches, and the 'head' points at the root of the tree
{
	int firstBranch = 0;
	int sameValue = 0;
//	printf("Starting huffman code");
	int lowest = findLowest(head);
//	printf("Lowest: %i\n", lowest);
	int secondLowest = findSecondLowest(lowest, head);
	wordFreq *smallest = findFreq(lowest, head, sameValue);
	if(lowest == secondLowest)
	{
		sameValue = 1;
	}
	wordFreq *secondSmallest = findFreq(secondLowest, head, sameValue);
	wordFreq *current;
	current = head;
	wordFreq **headref = &head;
	//removeNodeFromQueue(headref, current);
//	printf("\nValue of head: %s\n", head->word);
	newBranch(headref, smallest, secondSmallest,lowest+secondLowest);
	/**while(head->next != NULL)
	{
		current = head;
		if(current->freq > lowest)
		{
			lowest = current->freq;
		}
	}*/
//	printf("reached end of huffman code");
	return head;
}
char* compress(codeEntry *codeBook, char** wordArray, int arrayLength, int arrayWidth, int count)//Takes the array already made when tokenizing, and goes line by line, checking which words match up to what codebook, and printing the corresponding huffman code for it, outputting a binary stream
{
int i;
int j;
int k;
char temp[500];
char *output = malloc(sizeof(char)*500);
output[0] = '\0';
char wordTemp[arrayLength];
	for (k = 0; k<arrayWidth;k++)
	{
		for (j = 0; j< arrayLength;j++)
		{
		//	printf("(%c)", wordArray[j][k]);	//prints everything out
		//	if(wordArray[j][k]!='\0')
		//	{
				wordTemp[j] = wordArray[j][k];
				
		//	}
	//		printf("\n\nWordTemp IS()%s()\n\n", wordTemp);
		}
	//	printf("%s\n", wordTemp);
		for(i = 0; i<count;i++)
		{
	//		printf("%s\n", codeBook[i].word);
		//	printf("%s\n", wordTemp);
			if(strcmp(wordTemp, codeBook[i].word)==0)										//	
			{															//
				printf("%s", codeBook[i].code);
				strcat(output, codeBook[i].code);										//	THIS IS WHERE THE BINARY CONVERSION/COMPRESSION IS STORED 
			}															//
		}																//
		strcpy(wordTemp, "");
	}
//	printf("Compressed output is:%s", output);
	return output;

}
char* decompress(codeEntry *codeBook, char*input, int count)//this function 'translates' a given encoded series, putting it back into english via conversion using a huffman codebook
{
	printf("\n");
	char temp[20] = {'\0'};
	char* output = malloc(sizeof(char) *2000);
	output[0] = '\0';
	int i;
	int k;
	int j;
	int match = 0;
	int tempCount = 0;
	for(i = 0; i<strlen(input);i++)
	{
		match = 0;
		temp[tempCount] = input[i];
	//	printf("Temp: %s",temp);
		for(k = 0; k<count;k++)
		{
		//	printf("%s\t", temp);
		//	printf("%s\n", codeBook[k].code);
			if(strcmp(temp, codeBook[k].code) == 0)
			{
			//	printf("%s()", codeBook[k].word);
				if(strcmp(codeBook[k].word, "\\t") == 0)
				{
					strcat(output, "\t");
				}
				else if(strcmp(codeBook[k].word, "\\n") == 0)
				{
					strcat(output, "\n");
				}
				else if(strcmp(codeBook[k].word, "\\r") == 0)
				{
					strcat(output, "\r");
				}

				else
				{
					strcat(output, codeBook[k].word);
				}
				match = 1;
				memset(temp, 0,sizeof(temp));
		//		*temp = 0;
		//		printf("\n%s\n", temp);
				tempCount = 0;	
			}
		}
		if(match == 0)
		{
			tempCount++;
		}
	}
//printf("Decompressed String is: %s", output);
return output;
}

char* stuff(char *input, char flag)
{
	int i;
	int j;
	int k;
	int count = 0;
	int onlySeparators = 1;
	int word = 0;
	int largestWord = 0;
	int numSeparators = 0;
	int separatorSequence = 1;
	int arrayLength = 0;
	int arrayWidth = 0;
	struct Dimensions arraySize;
	arraySize = getDimensions(input);
	arrayLength = arraySize.length+1; //was getting a segmentation fault if the length wasn't increased by one.
	arrayWidth = arraySize.width;																		
//	printf("Array Length: %i,Array Width: %i", arrayLength, arrayWidth);									//	debug code
	char **wordArray;
	int lengthLoc= 0;
	int widthLoc = 0;
	char *compressedOutput;
	char wordTemp[arrayLength];
	wordFreq *head = (wordFreq *)malloc(sizeof(wordFreq));
	wordFreq *current;
	//word array is an array of pointers to pointers. The first set of pointers representing the "rows" which each point to the "columns" This way the memory can be allocated based on the size found in the struct function above. 
	wordArray = malloc((arrayLength) * sizeof(char *));
	for(i = 0; i< arrayLength; i++)
	{
		wordArray[i] = malloc(arrayWidth* sizeof(char *));
	}
	

	for(i = 0; input[i] != '\0'; i++)
	{
		
		//if(isalpha(argv[1][i]) == 0)
		//if(fInput!= '\n' || fInput != '\t' || fInput != '\r')
		if(input[i] == ' ' || input[i] == '\t' || input[i] == '\n') //uses newline as delimiters rather than all non alphanumeric characters
		{
		//	printf("\nSpace Found");
			lengthLoc = 0;
			if(i!= 0)
			{
				widthLoc++;
			}
		//	printf("%i", widthLoc);
			if(input[i] == '\t')
			{
				wordArray[lengthLoc][widthLoc] = '\\';
				lengthLoc++;
				wordArray[lengthLoc][widthLoc] = 't';
			}
			else if(input[i] == '\n')
			{
				wordArray[lengthLoc][widthLoc] = '\\';
				lengthLoc++;
				wordArray[lengthLoc][widthLoc] = 'n';
			}
			else if(input[i] == '\r')
			{
				wordArray[lengthLoc][widthLoc] = '\\';
				lengthLoc++;
				wordArray[lengthLoc][widthLoc] = 'r';
			}
			else{
			wordArray[lengthLoc][widthLoc] = (char)input[i];}


			/**if(word == 0)
			{	
			//	printf("\nSaving space character\n");
				wordArray[lengthLoc][widthLoc] = (char)argv[1][i];
				printf("wordArray:qwer%c()", wordArray[lengthLoc][widthLoc]);
				//printf("%i eliurghsleirghseilrghseilurghseliuhgslirghslighseg", widthLoc);  					debug stuff
				widthLoc++;
				lengthLoc = 0;	
				word++;
				
			}
			else if(word > 0)
			{
				printf("\nSaving space character after word\n");
				widthLoc++;
				lengthLoc = 0;
				wordArray[lengthLoc][widthLoc] = (char)argv[1][i];
	//			printf("wordArray:%c()", wordArray[lengthLoc][widthLoc]);
	//			printf("wordArray:%c()", argv[1][i]);
				
				//printf("%i eliurghsleirghseilrghseilurghseliuhgslirghslighseg", widthLoc);  					debug stuff
				word = 0;
			}*/
		}
		else
		{
			if(input[i-1] == ' ' || input[i-1] == '\t' || input[i-1] == '\n') 
			{
				widthLoc++;
				lengthLoc = 0;
			}

		//	printf("\nLetterFound");											//	justdebuggthings.jpg
			separatorSequence = 0;
			if(onlySeparators == 1)
			{
				onlySeparators = 0; //This shows the code that the input string has at least 1 alphabetical character, meaning the program won't throw an error
			}
			
			wordArray[lengthLoc][widthLoc] = (char)input[i];
	//		printf("%s\n", argv[1]);
			//printf("%c\n", argv[1][i]);								
	//		printf("%c\n", wordArray[lengthLoc][widthLoc]);
	//		printf("%d\n", wordArray[lengthLoc][widthLoc]);							
			lengthLoc++;						//moves the column address down one, indicating the start of another word
			//printf("%i", lengthLoc);									
			word++;

		}
	}
	for (k = 0; k<arrayWidth;k++)
	{
		for (j = 0; j< arrayLength;j++)
		{
		//	printf("(%c)", wordArray[j][k]);	//prints everything out
			wordTemp[j] = wordArray[j][k];
		//	printf("\n\nWordTemp IS()%s()\n\n", wordTemp);
		}
		if(head->freq == 0)
		{
		//	printf("making head");
		//	printf("\n\n%s", wordTemp);
			head->word = strdup(wordTemp);
			head->freq = 1;
			head->next = NULL;
			count++;
		//	printf("head word is: %s", head->word);
		}
		else
		{
		//	printf("\nHead made, wordTemp is:()%s()", wordTemp);
		//	printf("\ncurrent word is: ()%s()", current->word);
			while(current != NULL)
			{
				if(strcmp(current->word,wordTemp) == 0)
				{
			//		printf("increasing frequency");
					current->freq++;
					break;
				}
				else if(current->next == NULL)
				{
					count++;
			//		printf("Making New Wordentry");
				//	printf("\n()%s()\n", wordTemp);
					newEntry(current, wordTemp);
					break;
				}
				current = current->next;
			}
		}
		current = head;
		
	//	printf("\n\n\n\n The Word Is: %s\n\n\n\n", wordTemp);
		strcpy(wordTemp, "");	
//		printf("\n");
	}
	int codes[count];
	while(current != NULL)
	{
	//	printf("\nWord: %s     Frequency:%i\n", current->word, current->freq);
		current = current->next;
	}
		current = head;
	
	while(head->next!=NULL)
	{
		head = huffmanCode(head);
	}
//	printf("Reached end of huffman code\n");
//	printf("\n\nHead:%s\n\n", current->word);
	/**while(current != NULL)
	{
		printf("\nWord: %s     Frequency:%i\n", current->word, current->freq);
		current = current->next;
	}*/
	char array[10];
	int depth = 0;
//	printf("starting codes\n");
	current = head;
	codeEntry codeBook[count];
	pos = 0;
	int cBLength = makeCodes(current, array, depth, codeBook, 0);
	char *bOutput;
	bOutput = makeCodeBook(codeBook);								//FOR COMPRESS TAG
//	printf("\nBoutput is:\n%s\n", bOutput);
//	printf("Starting compression");
//	compress(codeBook, wordArray, arrayLength, arrayWidth, count);

//	compressedOutput = compress(codeBook, wordArray, arrayLength, arrayWidth, count);		//FOR COMPRESS TAG
//	decompress(codeBook, compressedOutput, count);

/////////////////////////////////
	if(flag == 'b') {
			for(i = 0; i<arrayLength;i++)	//frees memory
	{
		free(wordArray[i]);	//goes through the columns first, which have to be accessed in a similar way to the way they were allocated
	}
	free(wordArray);		//then the rows
//	printf("Array freed\n");

	freeTree(current);

		return bOutput;
	}
	else if(flag == 'c') {
		compressedOutput = compress(codeBook, wordArray, arrayLength, arrayWidth, count);
	for(i = 0; i<arrayLength;i++)	//frees memory
	{
		free(wordArray[i]);	//goes through the columns first, which have to be accessed in a similar way to the way they were allocated
	}
	free(wordArray);		//then the rows
//	printf("Array freed\n");

	freeTree(current);

		return compressedOutput;
	}
	else if(flag == 'd') {
		compressedOutput = compress(codeBook, wordArray, arrayLength, arrayWidth, count);		//FOR COMPRESS TAG
		char* decompressed = decompress(codeBook, compressedOutput, count);
		for(i = 0; i<arrayLength;i++)	//frees memory
	{
		free(wordArray[i]);	//goes through the columns first, which have to be accessed in a similar way to the way they were allocated
	}
	free(wordArray);		//then the rows
//	printf("Array freed\n");

	freeTree(current);

		return decompressed;
	}
	else {
		return;
	}

//	readCodeBook(bOutput);
//	decompress(codeBook, compress(codeBook, wordArray, arrayLength, arrayWidth, count), count);
	/*printf("\n%s\t%i\n", current->word, current->freq);
	current = current->right;
	printf("\n%s\t%i\n", current->word, current->freq);
	current = current->left;
	printf("\n%s\t%i\n", current->word, current->freq);
	current = current->right;


	printf("\n%s\t%i\n", current->word, current->freq);*/
	for(i = 0; i<arrayLength;i++)	//frees memory
	{
		free(wordArray[i]);	//goes through the columns first, which have to be accessed in a similar way to the way they were allocated
	}
	free(wordArray);		//then the rows
//	printf("Array freed\n");

	freeTree(current);
}


/*
int main(int argc, char *argv[])
{
	if(argv[1] == NULL)
	{
		printf("Empty String, please try again");		//throws an error if the string is empty or nonexistent
		return 0;
	}
	if(argc > 2)
	{
		printf("Too many strings, please try agian");		//throws an error if more than one string is sent
		return 0;
	}
	stuff(argv[1]);
*/
	/**printf("%c", (char)argv[1][0]);
	//char fInput = getc(file);
	int i;
	int j;
	int k;
	int count = 0;
	int onlySeparators = 1;
	int word = 0;
	int largestWord = 0;
	int numSeparators = 0;
	int separatorSequence = 1;
	int arrayLength = 0;
	int arrayWidth = 0;
	struct Dimensions arraySize;
	arraySize = getDimensions(argv[1]);
	arrayLength = arraySize.length+1; //was getting a segmentation fault if the length wasn't increased by one.
	arrayWidth = arraySize.width;																		
//	printf("Array Length: %i,Array Width: %i", arrayLength, arrayWidth);									//	debug code
	char **wordArray;
	int lengthLoc= 0;
	int widthLoc = 0;
	
	char wordTemp[arrayLength];
	wordFreq *head = (wordFreq *)malloc(sizeof(wordFreq));
	wordFreq *current;
	//word array is an array of pointers to pointers. The first set of pointers representing the "rows" which each point to the "columns" This way the memory can be allocated based on the size found in the struct function above. 
	wordArray = malloc((arrayLength) * sizeof(char *));
	for(i = 0; i< arrayLength; i++)
	{
		wordArray[i] = malloc(arrayWidth* sizeof(char *));
	}
	

	for(i = 0; argv[1][i] != '\0'; i++)
	{
		
		//if(isalpha(argv[1][i]) == 0)
		//if(fInput!= '\n' || fInput != '\t' || fInput != '\r')
		if(argv[1][i] == ' ' || argv[1][i] == '\t' || argv[1][i] == '\n') //uses newline as delimiters rather than all non alphanumeric characters
		{
		//	printf("\nSpace Found");
			lengthLoc = 0;
			if(i!= 0)
			{
				widthLoc++;
			}
		//	printf("%i", widthLoc);
			wordArray[lengthLoc][widthLoc] = (char)argv[1][i];
			if(word == 0)
			{	
			//	printf("\nSaving space character\n");
				wordArray[lengthLoc][widthLoc] = (char)argv[1][i];
				printf("wordArray:qwer%c()", wordArray[lengthLoc][widthLoc]);
				//printf("%i eliurghsleirghseilrghseilurghseliuhgslirghslighseg", widthLoc);  					debug stuff
				widthLoc++;
				lengthLoc = 0;	
				word++;
				
			}
			else if(word > 0)
			{
				printf("\nSaving space character after word\n");
				widthLoc++;
				lengthLoc = 0;
				wordArray[lengthLoc][widthLoc] = (char)argv[1][i];
	//			printf("wordArray:%c()", wordArray[lengthLoc][widthLoc]);
	//			printf("wordArray:%c()", argv[1][i]);
				
				//printf("%i eliurghsleirghseilrghseilurghseliuhgslirghslighseg", widthLoc);  					debug stuff
				word = 0;
			}
		}
		else
		{
			if(argv[1][i-1] == ' ' || argv[1][i-1] == '\t' || argv[1][i-1] == '\n') 
			{
				widthLoc++;
				lengthLoc = 0;
			}

		//	printf("\nLetterFound");											//	justdebuggthings.jpg
			separatorSequence = 0;
			if(onlySeparators == 1)
			{
				onlySeparators = 0; //This shows the code that the input string has at least 1 alphabetical character, meaning the program won't throw an error
			}
			
			wordArray[lengthLoc][widthLoc] = (char)argv[1][i];
	//		printf("%s\n", argv[1]);
			//printf("%c\n", argv[1][i]);								
	//		printf("%c\n", wordArray[lengthLoc][widthLoc]);
	//		printf("%d\n", wordArray[lengthLoc][widthLoc]);							
			lengthLoc++;						//moves the column address down one, indicating the start of another word
			//printf("%i", lengthLoc);									
			word++;

		}
	}
	for (k = 0; k<arrayWidth;k++)
	{
		for (j = 0; j< arrayLength;j++)
		{
		//	printf("(%c)", wordArray[j][k]);	//prints everything out
			wordTemp[j] = wordArray[j][k];
		//	printf("\n\nWordTemp IS()%s()\n\n", wordTemp);
		}
		if(head->freq == 0)
		{
		//	printf("making head");
		//	printf("\n\n%s", wordTemp);
			head->word = strdup(wordTemp);
			head->freq = 1;
			head->next = NULL;
			count++;
		//	printf("head word is: %s", head->word);
		}
		else
		{
		//	printf("\nHead made, wordTemp is:()%s()", wordTemp);
		//	printf("\ncurrent word is: ()%s()", current->word);
			while(current != NULL)
			{
				if(strcmp(current->word,wordTemp) == 0)
				{
			//		printf("increasing frequency");
					current->freq++;
					break;
				}
				else if(current->next == NULL)
				{
					count++;
			//		printf("Making New Wordentry");
				//	printf("\n()%s()\n", wordTemp);
					newEntry(current, wordTemp);
					break;
				}
				current = current->next;
			}
		}
		current = head;
		
	//	printf("\n\n\n\n The Word Is: %s\n\n\n\n", wordTemp);
		strcpy(wordTemp, "");	
//		printf("\n");
	}
	int codes[count];
	while(current != NULL)
	{
	//	printf("\nWord: %s     Frequency:%i\n", current->word, current->freq);
		current = current->next;
	}
		current = head;
	
	while(head->next!=NULL)
	{
		head = huffmanCode(head);
	}
//	printf("Reached end of huffman code\n");
//	printf("\n\nHead:%s\n\n", current->word);
	while(current != NULL)
	{
		printf("\nWord: %s     Frequency:%i\n", current->word, current->freq);
		current = current->next;
	}
	char array[10];
	int depth = 0;
//	printf("starting codes\n");
	current = head;
	codeEntry codeBook[count];
	pos = 0;
	int cBLength = makeCodes(current, array, depth, codeBook, 0);
	makeCodeBook(codeBook);
//	printf("Starting compression");
	compress(codeBook, wordArray, arrayLength, arrayWidth, count);
	decompress(codeBook, "11010011010010011011", count);
	printf("\n%s\t%i\n", current->word, current->freq);
	current = current->right;
	printf("\n%s\t%i\n", current->word, current->freq);
	current = current->left;
	printf("\n%s\t%i\n", current->word, current->freq);
	current = current->right;
	printf("\n%s\t%i\n", current->word, current->freq);
	for(i = 0; i<arrayLength;i++)	//frees memory
	{
		free(wordArray[i]);	//goes through the columns first, which have to be accessed in a similar way to the way they were allocated
	}
	free(wordArray);		//then the rows
//	printf("Array freed\n");

	freeTree(current);*/
//	return 0;
//}
