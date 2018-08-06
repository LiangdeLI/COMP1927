// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Game.h"
#include "DracView.h"
#include "Map.h"
#include "Places.h"

int readTrail (LocationID location, DracView gameState);

void decideDraculaMove(DracView gameState)
{
   if(giveMeTheRound(gameState) == 0) {
   	   registerBestPlay("BR", "I'll start here");
   	   return;
   }
   //LocationID myLocation = whereIs(gameState, PLAYER_DRACULA);
   //int length = 0;
   int i = 0;
   int numConnections = 0;
   LocationID *moves = whereCanIgo(gameState, &numConnections, TRUE, TRUE);

/*   int numLocations = numConnections;
   fprintf(stderr,"numlocations = %d\n", numLocations);
   srand(time(NULL));
   choice = (rand() % numLocations);
   fprintf(stderr,"choice = %d\n", choice);
   char *bestmove;
   
   if (moves[choice] >= MIN_MAP_LOCATION && moves[choice] <= MAX_MAP_LOCATION) {
      bestmove = idToAbbrev(moves[choice]);
   } else {
      bestmove = "HI";
   }*/

	LocationID whereToGo = moves[0];
	int moveChosen = 0;	
	while (i < numConnections) {
		if (!readTrail(moves[i], gameState)) {
			fprintf (stderr, "Move %d", i);
			whereToGo = moves[i];
			moveChosen = 1;
			break;
		} 
		i++;
	}

	if (moveChosen == 0) {
		registerBestPlay("HI", "No move found so hid");
		free(moves);
		return;
	}
	
   registerBestPlay(idToAbbrev(whereToGo), "Muahahahaha");
   free(moves);
}

int readTrail (LocationID location, DracView gameState) {
	LocationID trail[TRAIL_SIZE] = {0};
	giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
	int i = 0;
	while (i < TRAIL_SIZE) {
		if (trail[i] == location) {
			return 1;
		}
		i++;
	}
	return 0;
}
