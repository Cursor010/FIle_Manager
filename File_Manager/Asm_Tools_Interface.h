#pragma once

#include "Common.h"

extern "C" void Draw_Line_Horizontal(CHAR_INFO * screen_buffer, SPos pos, ASymbol symbol);
extern "C" void Draw_Line_Vertical(CHAR_INFO * screen_buffer, SPos pos, ASymbol symbol);
extern "C" void Show_Colors(CHAR_INFO * screen_buffer, SPos pos, CHAR_INFO symbol);
extern "C" void Clear_Area(CHAR_INFO * screen_buffer, SArea_Pos area_pos, ASymbol symbol);
extern "C" int Draw_Text(CHAR_INFO * screen_buffer, SText_Pos pos, const wchar_t* str);
extern "C" void Draw_Limited_Text(CHAR_INFO * screen_buffer, SText_Pos pos, const wchar_t* str, unsigned short limit);