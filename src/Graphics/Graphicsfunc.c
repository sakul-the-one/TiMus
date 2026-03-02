#include "Graphics/Graphicsfunc.h"
#include <fileioc.h>
#include <string.h>
#include <ti/getcsc.h>
#include <stdio.h>
#include <stdlib.h>
#include <graphx.h>
//#include <keypadc.h>

#define MaxScreenX 320
#define MaxScreenY 240
#define Border 30
#define gfx_gray 0x6B
#define MaxMessages 15
#define PreDel 5 /*The number of chars that dont count in the message*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-W#pragma-messages"

char message[32];
char name[PreDel];
int8_t CMpos = -1;
char * messages[MaxMessages];
uint8_t RowPos = 0;

void UpdateHistory() 
{
    #define offset 5
    #define distanceMul 11
    gfx_SetColor(gfx_white);
    //Middle Part:
    gfx_FillRectangle(0, Border, MaxScreenX, MaxScreenY - 2 *Border);
    gfx_SetColor(gfx_black);
    for (int i = 0; i < MaxMessages; i++) 
    {
        if(messages[i] == NULL) return;
        gfx_PrintStringXY(messages[i], 4, distanceMul * i + Border + offset);
    }
}
void UpdateMessage() 
{
    gfx_SetColor(gfx_gray);
    //Buttom Border:
    gfx_FillRectangle(0,MaxScreenY-Border,MaxScreenX, Border);
    //Top border:
    gfx_FillRectangle(0,0,MaxScreenX, Border);
    gfx_SetColor(gfx_black);
    gfx_PrintStringXY("Message: ",1, MaxScreenY - Border + 10);
    int8_t CC = CMpos - PreDel;
    if(CC < 0)
        CC = 0;
    gfx_PrintStringXY("Max Chars: ", 1, 5); gfx_PrintInt(CC, 2); gfx_PrintString("/27");//Why 27? 32 Max Chars - 5 for name and - 1 for \0!
    gfx_PrintStringXY(message, 50, MaxScreenY - Border + 10);//Compiler will optimize it anyways :)
}
void AddMessages(char * NewMessage) 
{
    if (RowPos < MaxMessages) 
    {
        messages[RowPos] = NewMessage;
        RowPos++;
        return;
    }
    free(messages[0]);
    for (int i = 1; i < MaxMessages; i++)
        messages[i-1] = messages[i];
    messages[MaxMessages -1] = NewMessage;
}
void PrintUI() 
{
    //gfx_screen();
    //Reset Screen
    gfx_FillScreen(gfx_white);
    gfx_SetColor(gfx_gray);
    //Top Border
    gfx_FillRectangle(0,0,MaxScreenX, Border);
    //Buttom Border
    //gfx_FillRectangle(0,MaxScreenY-Border,MaxScreenX, Border);
    gfx_SetColor(gfx_black);
    gfx_PrintStringXY("Max Chars: 0/26", 1, 5); //Why 27? 32 Max Chars - 5 for name and - 1 for \0!
    UpdateHistory();
    UpdateMessage();
}
void ResetMessage() 
{
    for (int i = 4; i < 32; i++) //0, 1, 2 &3 should be ' '!
    {
        message[i] = '\0';
    }
    for (int i = 0; i < PreDel; i++) //0, 1, 2, 3 &4 should be ' '!, otherwise it wouldnt show the message!
    {
        message[i] = ' ';
    }
}
char * HandleKeyBoard(uint8_t Ikey) 
{
    char key = ' ';
    //char test[2];
    if(CMpos == -1) 
    {
        ResetMessage();
        UpdateMessage();
        CMpos = PreDel;
    }
    switch (Ikey)
    {
        case sk_Enter: for (int i = 0; i < PreDel; i++) message[i] = name[i];
            CMpos = -1;
            message[31] = '\0';
            return message;break;
        case sk_Math: key = 'A'; break;
        case sk_Apps: key = 'B'; break;
        case sk_Prgm: key = 'C'; break;
        case sk_Recip: key = 'D'; break;
        case sk_Sin: key = 'E'; break;
        case sk_Cos: key = 'F'; break;
        case sk_Tan: key = 'G'; break;
        case sk_Power: key = 'H'; break;
        case sk_Square: key = 'I'; break;
        case sk_Comma: key = 'J'; break;
        case sk_LParen: key = 'K'; break;
        case sk_RParen: key = 'L'; break;
        case sk_Div: key = 'M'; break;
        case sk_Log: key = 'N'; break;
        case sk_7: key = 'O'; break;
        case sk_8: key = 'P'; break;
        case sk_9: key = 'Q'; break;
        case sk_Mul: key = 'R'; break;
        case sk_Ln: key = 'S'; break;
        case sk_4: key = 'T'; break;
        case sk_5: key = 'U'; break;
        case sk_6: key = 'V'; break;
        case sk_Sub: key = 'W'; break;
        case sk_Store: key = 'X'; break;
        case sk_1: key = 'Y'; break;
        case sk_2: key = 'Z'; break;
        case sk_Del: if(CMpos == -1) return NULL;CMpos--; message[CMpos] = ' '; UpdateMessage(); return NULL;
        case sk_0: key = ' '; break;
        case sk_DecPnt: key = '.'; break;
        case sk_Chs: key = '?'; break;
        case sk_Yequ: key = '!'; break;
        //case sk_: key = '?'; break; //1. idk the key, 2. Who needs a question anyways??
        default:
            return NULL;
    }
    //test[0] = key;
    //test[1] = '\0';
    //GetMessage(test, 2);
    if(CMpos < PreDel)
        CMpos = PreDel;
    if (CMpos >= 31)
        return NULL;
    message[CMpos] = key;
    CMpos++;
    UpdateMessage();
    return NULL; 
}

void GetMessage(char * buf, size_t lenght) 
{
    /*if(lenght < 32) //if smaller than 32, als 32 is our Max
    {//*/
        char * newBuf = malloc(lenght);
        buf[lenght-1] = '\0';//We force it to be 0 Terminated -> No overflow :)
        strcpy(newBuf, buf);
        AddMessages(newBuf);
        UpdateHistory();
    /*}
    else 
    {
        int LenghtLeft = lenght;
        int pos = 0;
        while (LenghtLeft > 0) 
        {
            int chunkSize = (LenghtLeft >= 16) ? 16 : LenghtLeft;
            char * newBuf = malloc(33);
            for (int i = pos, j = 0; i < pos + chunkSize; i++, j++) 
            {
                newBuf[j] = buf[i];
            }
            pos += 32;
            LenghtLeft -= 32;
            newBuf[32] = '\0'; //technically not needed, as it is always Null terminated, but better save than sorry!
            AddMessages(newBuf);
        }
    }//*/
}

void Init(char * _name)
{
    for(int i = 0; i < 3; i++)
        name[i] = _name[i];
    name[3] = ':';
    name[4] = ' ';
    for(int i = 0; i < 5; i++)
        if(name[i] == '\0')
            name[i] = ' ';
    gfx_Begin();
    for(int i = 0; i < MaxMessages; i++)
        messages[i] = NULL;
    PrintUI();
}
void Close() 
{
    gfx_End();
}

#pragma GCC diagnostic pop