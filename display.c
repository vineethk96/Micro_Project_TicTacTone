#include "display.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"

Graphics_Context g_sContext;

void InitDisplay() {
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext,
                         &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, FOREGROUNDCOLOR);
    Graphics_setBackgroundColor(&g_sContext, BACKGROUNDCOLOR);
    Graphics_setFont(&g_sContext, &g_sFontCmtt16);
    Graphics_clearDisplay(&g_sContext);
}

void DrawTime(unsigned minutes, unsigned seconds) {
    char buf[5];
    buf[4] = (seconds % 10) + '0';
    buf[3] = (seconds / 10) % 10 + '0';
    buf[2] = ':';
    buf[1] = (minutes % 10) + '0';
    buf[0] = (minutes / 10) % 10 + '0';
    Graphics_drawStringCentered(&g_sContext, (int8_t *) buf, 5, 64, 120, OPAQUE_TEXT);
}

void DrawMessage(char *s, uint32_t color) {
    Graphics_setForegroundColor(&g_sContext, color);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) s, -1, 64, 8, OPAQUE_TEXT);
}

void DrawPower(char *s, uint32_t color) {
    Graphics_setForegroundColor(&g_sContext, color);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) s, -1, 64, 120, OPAQUE_TEXT);
}

void DrawScore(int computerscore, int humanscore, uint32_t color) {
    char buf[16] = "Score XX - XX ";
    buf[ 7] =  (char) ('0' + ( computerscore % 10));
    buf[ 6] =  (char) ('0' + ((computerscore/10) % 10));
    buf[12] =  (char) ('0' + ( humanscore % 10));
    buf[11] =  (char) ('0' + ((humanscore/10) % 10));
    Graphics_setForegroundColor(&g_sContext, color);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) buf, -1, 64, 8, OPAQUE_TEXT);
}

void DrawBoard(tcellstate map[9]) {
    int8_t *symbolMap[3] = {" ", "X", "O"};

    Graphics_setForegroundColor(&g_sContext, LINECOLOR);
    Graphics_drawLineH(&g_sContext, 19,        109,    49);
    Graphics_drawLineH(&g_sContext, 19,        109,    79);
    Graphics_drawLineV(&g_sContext, 49,        19,     109);
    Graphics_drawLineV(&g_sContext, 79,        19,     109);

    // Draw standard X O
    Graphics_setForegroundColor(&g_sContext, FOREGROUNDCOLOR);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[0]], -1,  32,  32, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[1]], -1,  64,  32, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[2]], -1,  96,  32, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[3]], -1,  32,  64, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[5]], -1,  96,  64, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[6]], -1,  32,  96, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[7]], -1,  64,  96, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[8]], -1,  96,  96, OPAQUE_TEXT);
}

void DrawWinner(tcellstate map[9], int winner, uint32_t color) {
    int8_t *symbolMap[3] = {" ", "X", "O"};

    Graphics_setForegroundColor(&g_sContext, color);

    switch (winner) {
    case 1: // row 1
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[0]], -1,  32,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[1]], -1,  64,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[2]], -1,  96,  32, OPAQUE_TEXT);
        break;
    case 2: // row 2
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[3]], -1,  32,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[5]], -1,  96,  64, OPAQUE_TEXT);
        break;
    case 3: // row 3
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[6]], -1,  32,  96, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[7]], -1,  64,  96, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[8]], -1,  96,  96, OPAQUE_TEXT);
        break;
    case 4: // col 1
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[0]], -1,  32,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[3]], -1,  32,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[6]], -1,  32,  96, OPAQUE_TEXT);
        break;
    case 5: // col 2
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[1]], -1,  64,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[7]], -1,  64,  96, OPAQUE_TEXT);
        break;
    case 6: // col 3
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[2]], -1,  96,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[5]], -1,  96,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[8]], -1,  96,  96, OPAQUE_TEXT);
        break;
    case 7: // diagonal
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[0]], -1,  32,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[8]], -1,  96,  96, OPAQUE_TEXT);
        break;
    case 8: // back diagonal
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[2]], -1,  96,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[6]], -1,  32,  96, OPAQUE_TEXT);
        break;
    }
}

void DrawUnsigned(unsigned line, char prefix[4], unsigned n) {
    char buf[8]    = "XXXXXXXX";
    char hexstr[16] = "0123456789ABCDEF";

    buf[0]  = hexstr[(n >> 28) & 0xF];
    buf[1]  = hexstr[(n >> 24) & 0xF];
    buf[2]  = hexstr[(n >> 20) & 0xF];
    buf[3]  = hexstr[(n >> 16) & 0xF];
    buf[4]  = hexstr[(n >> 12) & 0xF];
    buf[5] = hexstr[(n >>  8) & 0xF];
    buf[6] = hexstr[(n >>  4) & 0xF];
    buf[7] = hexstr[(      n) & 0xF];
    //Graphics_drawStringCentered(&g_sContext, (int8_t *) buf, 13, 64, 16 + (line - 1) * 16, OPAQUE_TEXT);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) buf, -1, 64, 120, OPAQUE_TEXT);
}

/*
char convert2Hex(unsigned n)
{
    char buf[8]    = "XXXXXXXX";
    char hexstr[16] = "0123456789ABCDEF";

    DrawMessage("beforeCon", EMPHASISCOLOR);

    buf[0]  = hexstr[(n >> 28) & 0xF];
    buf[1]  = hexstr[(n >> 24) & 0xF];
    buf[2]  = hexstr[(n >> 20) & 0xF];
    buf[3]  = hexstr[(n >> 16) & 0xF];
    buf[4]  = hexstr[(n >> 12) & 0xF];
    buf[5] = hexstr[(n >>  8) & 0xF];
    buf[6] = hexstr[(n >>  4) & 0xF];
    buf[8] = hexstr[(      n) & 0xF];

    DrawMessage("afterConv", EMPHASISCOLOR);

    return buf;
}
*/
