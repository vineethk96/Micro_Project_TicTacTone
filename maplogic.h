#ifndef MAPLOGIC_H_
#define MAPLOGIC_H_

// This type presents the three possible states for a cell
typedef enum {empty, cross, circle} tcellstate;

// This function returns true when X wins
int       CrossWins(tcellstate map[9]);

// This function returns true when O wins
int       CircleWins(tcellstate map[9]);

// This function resets map sate to empty
void       ClearMap(tcellstate map[9]);

// This function fills all empty cells with 'X'
void       AbortMap(tcellstate map[9]);

// Adds a symbol v in a random empty location
int       RandomAdd(tcellstate map[9], tcellstate v);

// This function returns true of neither O nor X wins and no more moves are possible
int        Tie(tcellstate map[9]);

#endif /* MAPLOGIC_H_ */
