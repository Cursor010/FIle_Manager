#include "Commander.h"

// AsCommander
//------------------------------------------------------------------------------------------------------------
AsCommander::~AsCommander()
{
	CloseHandle(screen_buffer_handle_);
	CloseHandle(std_output_handle_);
	CloseHandle(std_input_handle_);
}
//------------------------------------------------------------------------------------------------------------
bool AsCommander::Init()
{// Initializing the screen buffer
	unsigned int screen_buffer_size{};

	try
	{
		HWND consoleWindow = GetConsoleWindow();
		LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
		SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME);

		SMALL_RECT windowSize = { 0, 0, 79, 24 };
		SetConsoleWindowInfo(screen_buffer_handle_, TRUE, &windowSize);

		std_input_handle_ = GetStdHandle(STD_INPUT_HANDLE);
		std_output_handle_ = GetStdHandle(STD_OUTPUT_HANDLE);

		screen_buffer_handle_ = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CONSOLE_TEXTMODE_BUFFER, 0);

		if (std_input_handle_ == INVALID_HANDLE_VALUE || std_output_handle_ == INVALID_HANDLE_VALUE || screen_buffer_handle_ == INVALID_HANDLE_VALUE)
		{
			printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
			return false;
		}

		if (!SetConsoleActiveScreenBuffer(screen_buffer_handle_))
		{
			printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
			return false;
		}

		if (!GetConsoleScreenBufferInfo(screen_buffer_handle_, &screen_buffer_info_))
		{
			printf("GetConsoleScreenBufferInfo failed - (%d)\n", GetLastError());
			return false;
		}
		screen_buffer_size = (int)screen_buffer_info_.dwSize.X * (int)screen_buffer_info_.dwSize.Y;

		screen_buffer_ = std::make_unique<CHAR_INFO[]>(screen_buffer_size);
		memset(screen_buffer_.get(), 0, screen_buffer_size * sizeof(CHAR_INFO));

		screen_half_width_ = screen_buffer_info_.dwSize.X / 2;

		left_panel_ = std::make_shared<APanel>(0, 0, screen_half_width_, screen_buffer_info_.dwSize.Y - 2, screen_buffer_);
		right_panel_ = std::make_shared<APanel>(screen_half_width_, 0, screen_half_width_, screen_buffer_info_.dwSize.Y - 2, screen_buffer_);

		left_panel_->getDirectoryFiles(EGetFilesMode::ENTRANCE);
		right_panel_->getDirectoryFiles(EGetFilesMode::ENTRANCE);

		left_panel_->setDirectoryHighlight(0xb0);
		right_panel_->setDirectoryHighlight(0x1b);

		active_panel_ = left_panel_;

		buildMenu();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception caught in AsCommander::Init: " << e.what() << std::endl;
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsCommander::run()
{// Launching the main application loop
	unsigned long records_count{};
	INPUT_RECORD input_record[128]{};

	while (can_run_)
	{
		checkHandleInput(input_record[0], records_count);
		time_->checkUpdateTime(last_time_min_, need_redraw_);

		if (need_redraw_)
		{
			if (!update())
			{
				return;
			}

			need_redraw_ = false;
		}
		Sleep(10);
	}
	exit(1);
}
//------------------------------------------------------------------------------------------------------------
bool AsCommander::update()
{// Updating panels
	COORD screen_buffer_pos{};

	try
	{
		right_panel_->update();
		left_panel_->update();

		for (size_t i = 0; i < 12; i++)
		{
			if (menu_items_[i] != nullptr)
				menu_items_[i]->Draw(screen_buffer_.get(), screen_buffer_info_.dwSize.X);
		}

		if (!WriteConsoleOutput(screen_buffer_handle_, screen_buffer_.get(), screen_buffer_info_.dwSize, screen_buffer_pos, &screen_buffer_info_.srWindow))
		{
			printf("WriteConsoleOutput failed - (%d)\n", GetLastError());
			return false;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception caught in AsCommander::update: " << e.what() << std::endl;
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsCommander::addNextMenuItem(unsigned short& index, unsigned short& x_pos, const unsigned short x_step, const wchar_t* key, const wchar_t* name)
{// Forming the next menu item
	menu_items_.push_back(std::make_unique<AMenu_Item>(x_pos, screen_buffer_info_.dwSize.Y - 1, 12, key, name));
	x_pos += x_step;

	if (index == 2)
		--x_pos;
}
//------------------------------------------------------------------------------------------------------------
void AsCommander::buildMenu()
{// Forming a menu
	unsigned short index{};
	unsigned short x_pos{};
	const unsigned short x_step = screen_buffer_info_.dwSize.X / 12;

	const wchar_t* MENU_ITEMS[] = { L"Help", L"Copy", L"View", L"Edit", L"Transfer", L"RenMov", L"MakeDir", L"Delete", L"Config", L"Quit", L"Plugin", L"Screen" };
	const wchar_t* MENU_ITEM_NUMBER[] = { L"1", L" 2", L" 3", L" 4", L" 5", L" 6", L" 7", L" 8", L" 9", L" 10" , L" 11", L" 12" };

	for (size_t i = 0; i < sizeof(MENU_ITEMS); ++i)
	{
		addNextMenuItem(index, x_pos, x_step, MENU_ITEM_NUMBER[i], MENU_ITEMS[i]);

		if (i == 1)
		{
			x_pos--;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsCommander::checkHandleInput(INPUT_RECORD& input_record, unsigned long& records_count)
{// Checking keypress events
	try
	{
		if (PeekConsoleInput(std_input_handle_, &input_record, 1, &records_count))
		{
			if (records_count > 0)
			{
				ReadConsoleInput(std_input_handle_, &input_record, 1, &records_count);
				if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown) {

					switch (input_record.Event.KeyEvent.wVirtualKeyCode)
					{
					case 0x79: // F10
						if (massage_box_->showMessageBox(L"Ouit", L"Do you want to quit File Manager?"))
						{
							can_run_ = false;
						}
						break;

					case 0x26: // UP
						active_panel_->changeSelectedFilePosition(EMoveDirection::UP);
						break;

					case 0x25: // LEFT
						active_panel_->changeSelectedFilePosition(EMoveDirection::LEFT);
						break;

					case 0x28: // DOWN
						active_panel_->changeSelectedFilePosition(EMoveDirection::DOWN);
						break;

					case 0x27: // RIGHT
						active_panel_->changeSelectedFilePosition(EMoveDirection::RIGHT);
						break;

					case 0x0D: //ENTER
						active_panel_->onEnter();
						break;

					case 0x51: // Q
						active_panel_ = (active_panel_ == left_panel_) ? right_panel_ : left_panel_;
						if (active_panel_ == left_panel_)
						{
							left_panel_->setDirectoryHighlight(0xb0);
							right_panel_->setDirectoryHighlight(0x1b);
						}
						else
						{
							left_panel_->setDirectoryHighlight(0x1b);
							right_panel_->setDirectoryHighlight(0xb0);
						}
						break;

					case 0x77: // F8
						active_panel_->deleteSelectedFile();
						left_panel_->getDirectoryFiles(EGetFilesMode::UPDATE);
						right_panel_->getDirectoryFiles(EGetFilesMode::UPDATE);
						break;

					case 0x74: // F5
						if (active_panel_->transferSelectedFile(transfer_mode_))
						{
							left_panel_->getDirectoryFiles(EGetFilesMode::UPDATE);
							right_panel_->getDirectoryFiles(EGetFilesMode::UPDATE);

							transfer_mode_ = !transfer_mode_;
						}
						break;
						
					case 0x71: // F2
						if (active_panel_->copySelectedFile(copy_mode_))
						{
							left_panel_->getDirectoryFiles(EGetFilesMode::UPDATE);
							right_panel_->getDirectoryFiles(EGetFilesMode::UPDATE);

							copy_mode_ = !copy_mode_;
						}
						break;

					default:
						break;
					}
					need_redraw_ = true;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception caught in AsCommander::checkHandleInput: " << e.what() << std::endl;
	}
}
//------------------------------------------------------------------------------------------------------------
