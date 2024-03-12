#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <ctime>

//------------------------------------------------------------------------------------------------------------
class AFile_Descriptor
{
public:
	AFile_Descriptor(unsigned int attributes, unsigned int size_low, unsigned int size_high, wchar_t* file_name, FILETIME last_write_time);
	unsigned int attributes_;
	unsigned long long file_size_;
	std::wstring file_name_;
	SYSTEMTIME stLocal_;

private:
	SYSTEMTIME stUTC_;
	FILETIME last_write_time_;

};
//------------------------------------------------------------------------------------------------------------
struct SPos
{
	SPos(unsigned short x_pos, unsigned short y_pos, unsigned short screen_width, unsigned short len);

	unsigned short X_Pos;
	unsigned short Y_Pos;
	unsigned short Screen_Width;
	unsigned short Len;
};
//------------------------------------------------------------------------------------------------------------
struct SText_Pos
{
	SText_Pos(unsigned short x_pos, unsigned short y_pos, unsigned short screen_width, unsigned short attributes);

	unsigned short X_Pos;
	unsigned short Y_Pos;
	unsigned short Screen_Width;
	unsigned short Attributes;
};
//------------------------------------------------------------------------------------------------------------
struct SArea_Pos
{
	SArea_Pos(unsigned short x_pos, unsigned short y_pos, unsigned short screen_width, unsigned char width, unsigned char height);

	unsigned short X_Pos;
	unsigned short Y_Pos;
	unsigned short Screen_Width;
	unsigned char Width, Height;
};
//------------------------------------------------------------------------------------------------------------
enum struct EMoveDirection
{
	UP = 0,
	LEFT = 1,
	DOWN = 2,
	RIGHT = 3
};
//------------------------------------------------------------------------------------------------------------
class ASymbol
{
public:
	ASymbol(wchar_t main_symbol, unsigned short attributes, wchar_t start_symbol, wchar_t end_symbol);

	wchar_t Main_Symbol;
	unsigned short Attributes;
	wchar_t Start_Symbol;
	wchar_t End_Symbol;
};
//------------------------------------------------------------------------------------------------------------
enum struct EGetFilesMode
{
	UPDATE = 0,
	ENTRANCE = 1,
	EXIT = 2
};
//------------------------------------------------------------------------------------------------------------
struct SConverter
{
	std::wstring convertSize(const unsigned long long& size) const;
};
//------------------------------------------------------------------------------------------------------------
struct SMessageBox
{
	bool showMessageBox(const std::wstring& title, const std::wstring& text) const;
};
//------------------------------------------------------------------------------------------------------------
struct STime
{
	short checkUpdateTime(unsigned short& lastTimMin, bool& needRedraw) const;
	std::wstring getTime() const;
};
//------------------------------------------------------------------------------------------------------------