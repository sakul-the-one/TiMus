#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <graphx.h>

char * HandleKeyBoard(uint8_t Ikey);
void GetMessage(char * buf, size_t lenght);
void Init(char * _name);
void Close();
#endif