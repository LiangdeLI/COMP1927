// testList.c - testing DLList data type
// Written by John Shepherd, March 2013

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "DLList.h"
#define MAX 100

int main(int argc, char *argv[])
{
	DLList myList;
	char new[MAX];
	
	//testing for getDLList()
	printf("===Testing for getDLList===\n");
	printf("Create an DLList by reading items from stdin\n");
	printf("Please write an input:\n");
	myList = getDLList(stdin);
	printf("The list now becomes:\n");
	putDLList(stdout,myList);
	assert(validDLList(myList));
	printf("\n\n\n");
	
	//testing for DLListBefore
	printf("===Testing for DLListBefore===\n");
	printf("The list now is:\n");
	putDLList(stdout,myList);
    printf("Read new line and insert in front of current position\n");
    printf("Please write an input:\n");
	fgets(new,MAX,stdin);
	new[strlen(new)-1] = '\0';
	DLListBefore(myList,new);
	printf("The list now becomes:\n");
	putDLList(stdout,myList);
	assert(validDLList(myList));
	printf("\n\n\n");
	
	//testing for DLListAfter
	printf("===Testing for DLListAfter===\n");
	printf("The list now is:\n");
	putDLList(stdout,myList);
    printf("Read new line and insert after current position\n");
    printf("Please write an input:\n");
	fgets(new,MAX,stdin);
	new[strlen(new)-1] = '\0';
	DLListAfter(myList,new);
	printf("The list now becomes:\n");
	putDLList(stdout,myList);
	assert(validDLList(myList));
	printf("\n\n\n");
	
	//testing for DLListDelete
	printf("===Testing for DLListDelete===\n");
	printf("The list now is:\n");
	putDLList(stdout,myList);
	DLListDelete(myList);
	printf("The list now becomes:\n");
	putDLList(stdout,myList);
	assert(validDLList(myList));
	printf("\n\n\n");
	
	//testing for DLListDelete
	printf("===Testing for DLListDelete===\n");
	printf("The list now is:\n");
	putDLList(stdout,myList);
	DLListDelete(myList);
	printf("The list now becomes:\n");
	putDLList(stdout,myList);
	assert(validDLList(myList));
	printf("\n\n\n");
	
	//testing for DLListDelete
	printf("===Testing for DLListDelete===\n");
	printf("The list now is:\n");
	putDLList(stdout,myList);
	DLListDelete(myList);
	printf("The list now becomes:\n");
	putDLList(stdout,myList);
	assert(validDLList(myList));
	printf("\n\n\n");
	
	//testing for DLListDelete
	printf("===Testing for DLListDelete===\n");
	printf("The list now is:\n");
	putDLList(stdout,myList);
	DLListDelete(myList);
	printf("The list now becomes:\n");
	putDLList(stdout,myList);
	assert(validDLList(myList));
	printf("\n\n\n");
	return 0;
}
