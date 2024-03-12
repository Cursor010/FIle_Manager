#include "Common.h"

// AFile_Descriptor
//------------------------------------------------------------------------------------------------------------
AFile_Descriptor::AFile_Descriptor(unsigned int attributes, unsigned int size_low, unsigned int size_high, wchar_t* file_name, FILETIME last_write_time)
	: attributes_(attributes), file_name_(file_name), last_write_time_(last_write_time)
{
	file_size_ = ((unsigned long long)size_high << 32) | (unsigned long long)size_low;
	FileTimeToSystemTime(&last_write_time_, &stUTC_);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC_, &stLocal_);
}
//------------------------------------------------------------------------------------------------------------




// SPos
//------------------------------------------------------------------------------------------------------------
SPos::SPos(unsigned short x_pos, unsigned short y_pos, unsigned short screen_width, unsigned short len)
	: X_Pos(x_pos), Y_Pos(y_pos), Screen_Width(screen_width), Len(len)
{
}
//------------------------------------------------------------------------------------------------------------




// SText_Pos
//------------------------------------------------------------------------------------------------------------
SText_Pos::SText_Pos(unsigned short x_pos, unsigned short y_pos, unsigned short screen_width, unsigned short attributes)
	: X_Pos(x_pos), Y_Pos(y_pos), Screen_Width(screen_width), Attributes(attributes)
{
}
//------------------------------------------------------------------------------------------------------------




// SArea_Pos
//------------------------------------------------------------------------------------------------------------
SArea_Pos::SArea_Pos(unsigned short x_pos, unsigned short y_pos, unsigned short screen_width, unsigned char width, unsigned char height)
	: X_Pos(x_pos), Y_Pos(y_pos), Screen_Width(screen_width), Width(width), Height(height)
{
}
//------------------------------------------------------------------------------------------------------------





// ASymbol
//------------------------------------------------------------------------------------------------------------
ASymbol::ASymbol(wchar_t main_symbol, unsigned short attributes, wchar_t start_symbol, wchar_t end_symbol)
	: Main_Symbol(main_symbol), Attributes(attributes), Start_Symbol(start_symbol), End_Symbol(end_symbol)
{

}
//------------------------------------------------------------------------------------------------------------




// SConverter
//------------------------------------------------------------------------------------------------------------
std::wstring SConverter::convertSize(const unsigned long long& size) const
{
	const unsigned long long GB = 1024ull * 1024 * 1024;
	const unsigned long long MB = 1024ull * 1024;
	const unsigned long long KB = 1024;

	std::wostringstream convert_size{};

	if (size >= GB)
	{
		convert_size << std::fixed << std::setprecision(2) << static_cast<double>(size) / GB << L"G";
	}
	else if (size >= MB)
	{
		convert_size << std::fixed << std::setprecision(2) << static_cast<double>(size) / MB << L"M";
	}
	else if (size >= KB)
	{
		convert_size << std::fixed << std::setprecision(2) << static_cast<double>(size) / KB << L"K";
	}
	else
	{
		convert_size << size;
	}

	return convert_size.str();
}
//------------------------------------------------------------------------------------------------------------




// SMessageBox
//------------------------------------------------------------------------------------------------------------
bool SMessageBox::showMessageBox(const std::wstring& title, const std::wstring& text) const
{
	int msgbox_ID = MessageBox(NULL, text.c_str(), title.c_str(), MB_ICONQUESTION | MB_YESNO);

	if (msgbox_ID == IDYES)
	{
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------




// STime
//------------------------------------------------------------------------------------------------------------
short STime::checkUpdateTime(unsigned short& lastTimMin, bool& needRedraw) const
{
	SYSTEMTIME lt_local;

	GetLocalTime(&lt_local);

	if (lt_local.wMinute != lastTimMin)
	{
		lastTimMin = lt_local.wMinute;
		needRedraw = true;
	}

	return lt_local.wMinute;
}
//------------------------------------------------------------------------------------------------------------
std::wstring STime::getTime() const
{
	SYSTEMTIME lt_local;

	std::wstring local_time(6, L'\0');

	GetLocalTime(&lt_local);

	swprintf(&local_time[0], local_time.size(), L"%02d:%02d", lt_local.wHour, lt_local.wMinute);

	return local_time;
}
//------------------------------------------------------------------------------------------------------------