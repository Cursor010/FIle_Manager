#include "Menu.h"

// AMenu_Item
//------------------------------------------------------------------------------------------------------------
AMenu_Item::AMenu_Item(unsigned short x_pos, unsigned short y_pos, unsigned short len, const wchar_t* key, const wchar_t* name)
	: X_Pos(x_pos), Y_Pos(y_pos), Len(len), Key(key), Name(name)
{
}
//------------------------------------------------------------------------------------------------------------
void AMenu_Item::Draw(CHAR_INFO* screen_buffer, unsigned short screen_width)
{
	int key_str_len;

	SText_Pos key_pos(X_Pos, Y_Pos, screen_width, 0x07);
	key_str_len = Draw_Text(screen_buffer, key_pos, Key);

	SText_Pos name_pos(X_Pos + key_str_len, Y_Pos, screen_width, 0xb0);
	Draw_Limited_Text(screen_buffer, name_pos, Name, Len);
}
//------------------------------------------------------------------------------------------------------------



