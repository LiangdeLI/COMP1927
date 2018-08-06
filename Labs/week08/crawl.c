// crawl.c ... build a graph of part of the web
// Written by John Shepherd, September 2015
// Uses the cURL library and functions by Vincent Sanders <vince@kyllikki.org>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <curl/curl.h>
#include <assert.h>
#include "stack.h"
#include "set.h"
#include "queue.h"
#include "graph.h"
#include "html.h"
#include "url_file.h"

#define BUFSIZE 1024

void setFirstURL(char *, char *);
void normalise(char *, char *, char *, char *, int);

int main(int argc, char **argv)
{
	URL_FILE *handle;
	char buffer[BUFSIZE];
	char baseURL[BUFSIZE];
	char firstURL[BUFSIZE];
	char next[BUFSIZE];
	int  maxURLs;
    int  counter = 0;
	if (argc > 2) {
		strcpy(baseURL,argv[1]);
		setFirstURL(baseURL,firstURL);
		maxURLs = atoi(argv[2]);
		//if (maxURLs < 40) maxURLs = 40;
	}
	else {
		fprintf(stderr, "Usage: %s BaseURL MaxURLs\n",argv[0]);
		exit(1);
	}
		
	// You need to modify the code below to implement:
	//
	Queue q = newQueue();
	printf("%s\n", firstURL);
	enterQueue(q, firstURL);// add firstURL to the ToDo list
	Graph g = newGraph(maxURLs);// initialise Graph to hold up to maxURLs
	Set seen = newSet();// initialise set of Seen URLs
	showQueue(q);	
	//printf("num of things in queue %d\n", emptyQueue(q)); 
	while(emptyQueue(q) != 1 && nElems(seen)<=maxURLs){ // while (ToDo list not empty and Graph not filled) {
	  	//printf("is queue not empty\n");
	    strcpy(next, leaveQueue(q));// grab Next URL from ToDo list
	    if ( strstr(next, "unsw.edu.au") == NULL /*|| isElen(seen, next)*/) {
	    	continue;// if (not allowed) continue
	    }
	    if (!(handle = url_fopen( next, "r"))) {
		    fprintf(stderr,"Couldn't open %s\n", next);
		    exit(1);
	    }
	    while(!url_feof(handle)) {// foreach line in the opened URL {
	   		//printf("opening url?\n");
		    url_fgets(buffer,sizeof(buffer),handle);
		    //fputs(buffer,stdout);
		    int pos = 0;
		    char result[BUFSIZE];
		    memset(result,0,BUFSIZE);
		    while ((pos = GetNextURL(buffer, firstURL, result, pos)) > 0) {// foreach URL on that line {
		    	//printf("in the while loop\n");
			    if ((isElem(seen,next) && isElem(seen,result))){// if (Graph not filled or both URLs in Graph)
			        addEdge(g, next, result);// add an edge from Next to this URL
			    }
			    if (!isElem(seen, result) && nVertices(g)<maxURLs){// if (this URL not Seen already) {
			        insertInto(seen, result);// add it to the Seen set
	                enterQueue(q, result);// add it to the ToDo list
			        //g->nV = addVertex(result, g->vertex, g->nV) + 1; //add vertex and implement number of vertex
			        addEdge(g, next, result);
			        counter++;
			        printf("%dFound: '%s'\n",counter,result);
		            memset(result,0,BUFSIZE);
		        }
		        memset(result,0,BUFSIZE);
		    }
	    }
	    url_fclose(handle);// close the opened URL
	    sleep(0.1);
	    //printf("next %s\n", next); 
	    
	    
	}
	
	showGraph(g, 0); 
	return 0;
}

// setFirstURL(Base,First)
// - sets a "normalised" version of Base as First
// - modifies Base to a "normalised" version of itself
void setFirstURL(char *base, char *first)
{
	char *c;
	if ((c = strstr(base, "/index.html")) != NULL) {
		strcpy(first,base);
		*c = '\0';
	}
	else if (base[strlen(base)-1] == '/') {
		strcpy(first,base);
		strcat(first,"index.html");
		base[strlen(base)-1] = '\0';
	}
	else {
		strcpy(first,base);
		strcat(first,"/index.html");
	}
}
