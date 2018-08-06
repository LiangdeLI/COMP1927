#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Game.h"
#include "Places.h"
#include "HunterView.h"

#define MESSAGE_SIZE 100

static int inTheirPass(int dir, HunterView gameState);

void decideHunterMove(HunterView gameState)
{
    if(giveMeTheRound(gameState) == 0){
    if (whoAmI(gameState) == PLAYER_LORD_GODALMING) registerBestPlay("KL", "I'll start here");
    else if (whoAmI(gameState) == PLAYER_DR_SEWARD) registerBestPlay("TO", "I'll start here");
    else if (whoAmI(gameState) == PLAYER_VAN_HELSING) registerBestPlay("GA", "I'll start here");
    else if (whoAmI(gameState) == PLAYER_MINA_HARKER) registerBestPlay("BU", "I'll start here");
    return;
    }
    /*int myOption = 0;
    LocationID * MyPossibleGo = whereCanIgo(gameState, &myOption, TRUE, TRUE, TRUE);
    time_t t;
    srand((unsigned) time(&t));
    int direction = rand() % (myOption);
    char * whereIGo = idToAbbrev(MyPossibleGo[direction]);// give back abbreviation
    char message[MESSAGE_SIZE] = "I'm going to ";
    char *whereGo = idToName(MyPossibleGo[direction]);//give back name
    int i = 0;
    int index = 13;
    while (whereGo[i] != '\0')
    {
        message[index] = whereGo[i];
        index++;
        i++;
    }
    message[index] = '\0';
    registerBestPlay(whereIGo, message);*/
    if (whoAmI(gameState) == PLAYER_LORD_GODALMING) {
        /*if (giveMeTheRound(gameState) % 2 == 0) {
            if(howHealthyIs(gameState, PLAYER_LORD_GODALMING) > 3)
                registerBestPlay("KL", "Boo");
            else
                registerBestPlay("GA", "Hoo");
        } else {
            if(howHealthyIs(gameState, PLAYER_LORD_GODALMING) > 3)
                registerBestPlay("GA", "Boo");
            else
                registerBestPlay("KL", "Hoo");
        }*/
        registerBestPlay("KL", "I stay here");
    } else if (whoAmI(gameState) == PLAYER_VAN_HELSING) {
        /*if (giveMeTheRound(gameState) % 2 == 0) {
            if(howHealthyIs(gameState, PLAYER_VAN_HELSING) > 3)
                registerBestPlay("GA", "Boo");
            else
                registerBestPlay("CD", "Hoo");
        } else {
            if(howHealthyIs(gameState, PLAYER_VAN_HELSING) > 3)
                registerBestPlay("KL", "Boo");
            else
                registerBestPlay("CD", "Hoo");
        }*/
        registerBestPlay("GA", "I stay here");
    } else {
        /*int numConnections = 0;
        LocationID *moves = whereCanIgo(gameState, &numConnections, TRUE, TRUE, TRUE);
        LocationID whereToGo = moves[0];
        registerBestPlay(idToAbbrev(whereToGo), "..");*/

        int myOption = 0;
        LocationID * MyPossibleGo = whereCanIgo(gameState, &myOption, TRUE, FALSE, TRUE);
        time_t t;
        srand((unsigned) time(&t));
        int direction = (rand() + giveMeTheRound(gameState)) % (myOption);
        int i=0;
        while (inTheirPass(direction, gameState) == 1){
            direction++;
            direction = direction % myOption;
            i++; 
            if(i==myOption) break;
        }
        char * whereIGo = idToAbbrev(MyPossibleGo[direction]);// give back abbreviation
        char message[MESSAGE_SIZE] = "I'm going to ";
        char *whereGo = idToName(MyPossibleGo[direction]);//give back name
        i = 0;
        int index = 13;
        while (whereGo[i] != '\0')
        {
            message[index] = whereGo[i];
            index++;
            i++;
        }
        message[index] = '\0';
        registerBestPlay(whereIGo, message);
    }
     
}

int inTheirPass(int dir, HunterView gameState)
{
    LocationID trail[TRAIL_SIZE];
    int i;
    for(i=0;i<4;i++){
        if(i==whoAmI(gameState)) continue;
        giveMeTheTrail(gameState, i, trail);
        if(trail[0]==dir || trail[1]==dir) return 1;
    }
    return 0;
}
