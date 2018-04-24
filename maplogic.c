#include "maplogic.h"
#include <stdlib.h>

// This function returns true when a row contains three cells of state mark
int SameRow(tcellstate map[9], unsigned row, tcellstate mark) {
   unsigned start = (row - 1) * 3;
    return ((map[start  ] == map[start+1]) &&
            (map[start+1] == map[start+2]) &&
            (map[start  ] == mark));
}

// This function returns true when a column contains three cells of state mark
int SameCol(tcellstate map[9], unsigned col, tcellstate mark) {
    unsigned start = (col - 1);
    return ((map[start  ] == map[3+start]) &&
            (map[3+start] == map[6+start]) &&
            (map[start  ] == mark));
}

// This function returns true when a diagonal contains three cells of state mark
int SameDiag(tcellstate map[9], tcellstate mark) {
    return (((map[0] == map[4]) && (map[4] == map[8]) && (map[0] == mark)));
}

// This function returns true when a diagonal contains three cells of state mark
int SameDiagBack(tcellstate map[9], tcellstate mark) {
    return (((map[2] == map[4]) && (map[4] == map[6]) && (map[2] == mark)));
}

// This function returns true when X wins
int CrossWins(tcellstate map[9]) {
    if (SameRow(map, 1, cross))
        return 1;
    else if (SameRow(map, 2,   cross))
        return 2;
    else if (SameRow(map, 3,   cross))
        return 3;
    else if (SameCol(map, 1,   cross))
        return 4;
    else if (SameCol(map, 2,   cross))
        return 5;
    else if (SameCol(map, 3,   cross))
        return 6;
    else if (SameDiag(map,     cross))
        return 7;
    else if (SameDiagBack(map, cross))
        return 8;
    return 0;
}

// This function returns true when O wins
int CircleWins(tcellstate map[9]) {
    if (SameRow(map, 1, circle))
        return 1;
    else if (SameRow(map, 2,   circle))
        return 2;
    else if (SameRow(map, 3,   circle))
        return 3;
    else if (SameCol(map, 1,   circle))
        return 4;
    else if (SameCol(map, 2,   circle))
        return 5;
    else if (SameCol(map, 3,   circle))
        return 6;
    else if (SameDiag(map,     circle))
        return 7;
    else if (SameDiagBack(map, circle))
        return 8;
    return 0;
}

int Tie(tcellstate map[9]) {
    return ((map[0] != empty) &&
            (map[1] != empty) &&
            (map[2] != empty) &&
            (map[3] != empty) &&
            (map[4] != empty) &&
            (map[5] != empty) &&
            (map[6] != empty) &&
            (map[7] != empty) &&
            (map[8] != empty) &&
            (CircleWins(map) == 0)  &&
            (CrossWins (map) == 0));
}

// This function resets map state to empty
void ClearMap(tcellstate map[9]) {
    map[0] = map[1] = map[2] = empty;
    map[3] = map[4] = map[5] = empty;
    map[6] = map[7] = map[8] = empty;
}

void AbortMap(tcellstate map[9]) {
    unsigned i;
    for (i=0; i<9; i++)
        if (map[i] == empty)
            map[i] = cross;
}

// Adds symbol in a random empty location
int  RandomAdd(tcellstate map[9], tcellstate v) {
   unsigned numempty = 0;
   unsigned done = 0;
   int i;
   for (i=0; i<9; i++)
       if (map[i] == empty)
           numempty++;
   if (numempty == 0)
       return 0;
   while (!done) {
       i = rand() % 9;
       if (map[i] == empty) {
           map[i] = v;
           done = 1;
       }
   }
   return i;
}
