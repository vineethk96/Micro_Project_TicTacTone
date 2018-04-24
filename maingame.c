#include <stdlib.h>
#include "swtimer.h"
#include "maplogic.h"
#include "button.h"
#include "display.h"
#include "hwtimer.h"

#include "goertzel.h"
#include "dtmf.h"
#include "microphone.h"
#include "sound.h"

#define DURATION 800
Graphics_Context g_sContext;

// This is the state definition for the top-level state machine,
// implemented in ProcessStep. As you build the game, you will
// have to define additional states to implement the actual game
// logic.
typedef enum {idle, game_S1, game_S2, gameOn, abortGame} state_t;

// This structure is a game state. It keeps track of the current
// playing field, the score achieved by the human, and the score
// achieved by the computer. The game state gets passed around
// during the game to allow different software components access
// to the game state.

typedef struct {
    tcellstate map[9];
    int computerscore;
    int humanscore;
} gamestate_t;

//userdefined variables****************************************************

typedef enum {compTurn, userTurn, noGame, gameOver} turn_t;
unsigned glbListening = 0;

int userScore = 0;
int compScore = 0;


//*************************************************************************

// This function implements the functionality of Tic Tac Tone during
// the idle state, i.e. when it is waiting for the player to provide
// a 'start game' command.
//
// This function is called every time the top-level FSM visits the
// idle state. The function has five parameters. You are welcome to
// add more, but know that the reference solution was implemented using
// only these 5 parameters.
//    b1   = 1 when button S1 is pressed, 0 otherwise
//    b2   = 1 when button S2 is pressed, 0 otherwise
//    sec  = 1 when the second-interval software timer elapses
//    ms50 = 1 when the 50ms-interval software timer elapses
//    G    = gamestate, as defined above. Pass by reference to make
//           sure that changes to G within this function will be
//           propagated out of the function.
//
// Note that this function RETURNS a state_t. This can be used to influence
// the state transition in the top-level FSM that will call processIdle.
// Currently, the ProcessIdle always returns idle (which means the top-level
// FSM will iterate in the idle state), but when the game is extended,
// the return state could change into something different (such as playing_circle
// or playing_cross, depending on whether S1 or S2 is pressed).
void clearEverything(gamestate_t *G)
{
    DTMFReset();
    ClearMap(G->map);
    DrawBoard(G->map);
}

void aborting(gamestate_t *G)
{
    AbortMap(G->map);
    DrawBoard(G->map);
    DrawMessage("              ", BACKGROUNDCOLOR);
    DrawMessage("I win!", EMPHASISCOLOR);
    PlaySound(note_silent, 3*DURATION);
}

void playNotes(int loc)
{
    switch (loc)
    {
    case 0:
        PlaySound(note_c5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        PlaySound(note_g5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    case 1:
        PlaySound(note_c5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        PlaySound(note_a5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    case 2:
        PlaySound(note_c5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        PlaySound(note_b5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    case 3:
        PlaySound(note_d5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        PlaySound(note_g5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    case 4:
        PlaySound(note_d5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        PlaySound(note_a5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    case 5:
        PlaySound(note_d5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        PlaySound(note_b5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    case 6:
        PlaySound(note_e5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        PlaySound(note_g5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    case 7:
        PlaySound(note_e5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        PlaySound(note_a5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    case 8:
        PlaySound(note_e5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        PlaySound(note_b5, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    case 10:    //comp wins
        DrawMessage("              ", BACKGROUNDCOLOR);
        DrawMessage("I win!", EMPHASISCOLOR);
        PlaySound(note_a4, 2*DURATION);
        PlaySound(note_b4, 2*DURATION);
        PlaySound(note_c4, 2*DURATION);
        PlaySound(note_silent, DURATION);

        break;
    case 11:    //user wins
        DrawMessage("              ", BACKGROUNDCOLOR);
        DrawMessage("you win!", EMPHASISCOLOR);
        PlaySound(note_c4, 2*DURATION);
        PlaySound(note_b4, 2*DURATION);
        PlaySound(note_a4, 2*DURATION);
        PlaySound(note_silent, DURATION);
        break;
    }

    return;
}

state_t ProcessIdle(int b1, int b2, int sec, int ms50, gamestate_t *G) {

    // These are the states of a _local_ FSM.
    // The state labels are prefixed with 'processidle' to
    // make sure they are distinct from the labels used for the
    // top-level FSM.
    //
    // The local FSM functionality plays a game of tic-tac-toe
    // against itself, using randomized moves. However, the
    // rules of tic-tac-toe are followed, including the game
    // map drawing and coloring over the reference solution.

    typedef enum {processidle_idle,
                  processidle_playingcircle,
                  processidle_playingcross,
                  processidle_winning} processidle_state_t;
    static processidle_state_t localstate = processidle_idle;

    unsigned w;

    //user defined variables**********************************************************
    typedef enum{titleMessage, scoreMessage, S1Message, S2Message} message_t;
    static message_t currMessage = titleMessage;
    static int totalSec = 0;
    Graphics_drawStringCentered(&g_sContext, "          ", -1, 64, 120, OPAQUE_TEXT);

    //********************************************************************************

    // We will run this local state machine only once per second,
    // therefore, we only run it when sec is 1. sec is
    // a software-timer generated in the main function.
    //
    // To add more functionality, you can extend this function. For example,
    // to display a label every three seconds, you can add a counter that is
    // incremented for every sec, modulo-3. When the counter is two, it means
    // that the three-second timer mark is reached.
    //
    // A longer counter period (eg modulo-12 iso modulo-3) can be used to
    // display rotating messages.

    if (sec) {

        if(totalSec % 3 == 0)
        {
            switch(currMessage)
            {
            case titleMessage:
                DrawMessage("              ", BACKGROUNDCOLOR);
                DrawMessage("* TicTacTone *", EMPHASISCOLOR);
                currMessage = scoreMessage;
                break;
            case scoreMessage:
                DrawScore(G->computerscore, G->humanscore, EMPHASISCOLOR);
                currMessage = S1Message;
                break;
            case S1Message:
                DrawMessage("              ", BACKGROUNDCOLOR);
                DrawMessage("S1: I start", EMPHASISCOLOR);
                currMessage = S2Message;
                break;
            case S2Message:
                DrawMessage("              ", BACKGROUNDCOLOR);
                DrawMessage("S2: You start", EMPHASISCOLOR);
                currMessage = titleMessage;
                break;
            }
        }

        switch (localstate) {
            case processidle_idle:
              // Initially, just draw the playing field
              ClearMap(G->map);
              DrawBoard(G->map);
              localstate = processidle_playingcircle;
              break;
            case processidle_playingcircle:
              // This is circle who is playing. A circle is
              // added in a random (but valid) location. Next,
              // we check if the game ends, which happens when
              // circle or cross would win, or when there's a tie.

              // Decide what position to play
              RandomAdd(G->map, circle);

              // Next, it's cross' turn
              localstate = processidle_playingcross;

              // If we win or tie, go to winning state instead
              if (CircleWins(G->map) || Tie(G->map))
                  localstate = processidle_winning;

              // update game board status on display
              DrawBoard(G->map);
              break;

            case processidle_playingcross:
              // This is cross who is playing. A cross is
              // added in a random (but valid) location. Next,
              // we check if the game ends, which happens when
              // circle or cross would win, or when there's a tie.

              // Decide what position to play
              RandomAdd(G->map, cross);

              // Next, it's circles' turn
              localstate = processidle_playingcircle;

              // If we win or tie, go to winning state instead
              if (CrossWins(G->map) || Tie(G->map))
                  localstate = processidle_winning;

              // update game board status on display
              DrawBoard(G->map);
              break;

            case processidle_winning:
              // This state is entered when there is a winner,
              // or it's a tie. In this state, we redraw the
              // winning combination in the emphasis color.
              // After that, we go for the next round.

              if (w = CircleWins(G->map))
                DrawWinner(G->map, w, EMPHASISCOLOR);

              if (w = CrossWins(G->map))
                DrawWinner(G->map, w, EMPHASISCOLOR);

              localstate = processidle_idle;
              break;
        }
        totalSec++;

    }

    if(b1)
    {
        DrawMessage("              ", BACKGROUNDCOLOR);
        DrawMessage("Thinking....", EMPHASISCOLOR);
        clearEverything(G);
        return game_S1;
    }
    else if(b2)
    {
        DrawMessage("              ", BACKGROUNDCOLOR);
        DrawMessage("Listening?", EMPHASISCOLOR);
        clearEverything(G);
        return game_S2;
    }

    return idle;
}

int getLocation()
{
    //start listening for dial tone
    int returnVal = 10;

    if (!glbListening)
    {
        DTMFPower();
        returnVal = DTMFlocation();
        DTMFReset();
        glbListening = 1;
    }

    return returnVal;
}

int checkCircleWin(gamestate_t *G)
{
    if(G->map[0] == circle)
    {
        if(G->map[1] == circle)          // top row
            return 2;
        else if(G->map[2] == circle)     // top row
            return 1;
        else if(G->map[4] == circle)     // diagonal
            return 8;
        else if(G->map[8] == circle)     // diagonal
            return 4;
        else if(G->map[3] == circle)     // first column
            return 6;
        else if(G->map[6] == circle)     // first column
            return 3;
    }
    else if(G->map[1] == circle)
    {
        if(G->map[4] == circle)          // second column
            return 7;
        else if(G->map[7] == circle)     // second column
            return 4;
    }
    else if(G->map[2] == circle)
    {
        if(G->map[5] == circle)          // third column
            return 8;
        else if(G->map[8] == circle)     // third column
            return 5;
        else if(G->map[4] == circle)     // anti diagonal
            return 6;
        else if(G->map[6] == circle)     // anti diagonal
            return 4;
        else if(G->map[1] == circle)     // top row
            return 0;
    }
    else if(G->map[3] == circle)
    {
        if(G->map[6] == circle)          // first column
            return 0;
        else if(G->map[4] == circle)     // middle row
            return 5;
        else if(G->map[5] == circle)     // middle row
            return 4;
    }
    else if(G->map[4] == circle)
    {
        if(G->map[5] == circle)          // middle row
            return 3;
        else if(G->map[6] == circle)     // diagonal
            return 2;
        else if(G->map[7] == circle)     // second column
            return 1;
        else if(G->map[8] == circle)     // anti diagonal
            return 0;
    }
    else if(G->map[5] == circle)
    {
        if(G->map[8] == circle)          // third column
            return 2;
    }
    else if(G->map[6] == circle)
    {
        if(G->map[7] == circle)          // bottom row
            return 8;
        else if(G->map[8] == circle)     // bottom row
            return 7;
    }
    else if(G->map[7] == circle)
    {
        if(G->map[8] == circle)          // bottom row
            return 6;
    }

    return 10;
}

void computerTurn(gamestate_t *G)
{
    int loc;

    loc = checkCircleWin(G);

    if(loc != 10)
    {
        //draw a circle at loc
        if(G->map[loc] == empty)
            G->map[loc] = cross;
        else
            loc = RandomAdd(G->map, cross);
    }
    else
    {
        //randomize a place to draw a circle
        loc = RandomAdd(G->map, cross);
    }

    DrawBoard(G->map);
    playNotes(loc);
}

turn_t gameOngoing(int b1, int b2, int sec, int ms50, gamestate_t *G, turn_t currTurn)
{
    int loc;
    unsigned w;
    turn_t returnCurrTurn = currTurn;

    if(CrossWins(G->map) || CircleWins(G->map) || Tie(G->map))
    {
        currTurn = gameOver;
        returnCurrTurn = gameOver;
    }

    switch(currTurn){
    case userTurn:
        if(b1)
        {
            aborting(G);
            currTurn = gameOver;
        }

        if(sec)
        {
            DrawMessage(" Listening? ", EMPHASISCOLOR);
        }

        loc = getLocation();

        if(loc != 10)
        {
            if(G->map[loc] == empty)
            {
                G->map[loc] = circle;
                DrawBoard(G->map);
                glbListening = 1;
                returnCurrTurn = compTurn;
                DrawMessage("              ", BACKGROUNDCOLOR);
                DrawMessage("Thinking....", EMPHASISCOLOR);
            }
            else
            {
                DrawMessage("              ", BACKGROUNDCOLOR);
                DrawMessage("illegal Move", EMPHASISCOLOR);
            }
        }
        //else{     // keep waiting }
        break;
    case compTurn:
        computerTurn(G);
        returnCurrTurn = userTurn;
        DrawMessage("              ", BACKGROUNDCOLOR);
        DrawMessage("Listening?", EMPHASISCOLOR);
        break;
    case gameOver:
        DrawMessage("              ", BACKGROUNDCOLOR);
        DrawMessage("Game Over", EMPHASISCOLOR);
        if (w = CircleWins(G->map))
        {
          DrawWinner(G->map, w, EMPHASISCOLOR);
          G->humanscore++;
          playNotes(11);
        }

        if (w = CrossWins(G->map))
        {
          DrawWinner(G->map, w, EMPHASISCOLOR);
          G->computerscore++;
          playNotes(10);
        }
        break;
    }

    return returnCurrTurn;
}

//Interrupt handler
void T32_INT1_IRQHandler() {
    unsigned vx;
    static unsigned SamplesListened = 0;
    if (glbListening) {
        vx = GetSampleMicrophone();
        DTMFAddSample(vx);
        SamplesListened++;

        if (SamplesListened == 400) {
            glbListening = 0;
            SamplesListened = 0;
        }
    }
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
}

// This is the top-level FSM, which is called from within
// the cyclic executive. You will have to extend this FSM
// with the game logic. The FSM takes four inputs:
//
//    b1   = 1 when button S1 is pressed, 0 otherwise
//    b2   = 1 when button S2 is pressed, 0 otherwise
//    sec  = 1 when the second-interval software timer elapses
//    ms50 = 1 when the 50ms-interval software timer elapses

void ProcessStep(int b1, int b2, int sec, int ms50) {
    static state_t S = idle;
    static gamestate_t G;
    static turn_t currTurn;

    switch (S) {
    case idle:
        S = ProcessIdle(b1, b2, sec, ms50, &G);
        break;
    case game_S1:
        currTurn = compTurn;
        S = gameOn;
        clearEverything(&G);
        b1 = false;
        break;
    case game_S2:
        currTurn = userTurn;
        S = gameOn;
        clearEverything(&G);
        break;
    case gameOn:
        if(currTurn == gameOver)
        {
            if(ms50)
            {
                S=idle;
                clearEverything(&G);
            }
        }

        currTurn = gameOngoing(b1, b2, sec, ms50, &G, currTurn);

        break;
    }
}

int main(void) {

    // Device Initialization
    InitTimer();
    InitSound();
    InitDisplay();
    InitButtonS1();
    InitButtonS2();

    // User added********************************************************************
    //InitADC();
    InitMicrophone();

    Interrupt_enableInterrupt(INT_T32_INT1);
    Interrupt_enableMaster();
    //*******************************************************************************

    // Software Timer - per second
    // Note that software timers MUST be tied to TIMER32_1_BASE;
    // TIMER32_1_BASE is configured in continuous mode
    // (TIMER32_0_BASE can then be used for periodic interrupts
    //  which will be needed to take samples from microphone)
    tSWTimer everySec;
    InitSWTimer(&everySec, TIMER32_1_BASE, SYSTEMCLOCK);
    StartSWTimer(&everySec);

    // Software Timer - per 50ms = 20Hz
    tSWTimer every50ms;
    InitSWTimer(&every50ms, TIMER32_1_BASE, SYSTEMCLOCK/20);
    StartSWTimer(&every50ms);

    // The cyclic executive is simple: read the buttons and software
    // timers, and call the top-level FSM in ProcessStep.
    glbListening = 1;

    //int b1 = 0;

    while (1) {
        //int prevB1 = b1;
        int b1    = ButtonS1Pressed();
        int b2    = ButtonS2Pressed();
        int sec   = SWTimerExpired(&everySec);
        int ms50  = SWTimerExpired(&every50ms);

        ProcessStep(b1, b2, sec, ms50);
    }

}
