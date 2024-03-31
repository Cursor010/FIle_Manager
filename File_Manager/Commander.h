#pragma once

#include <stdio.h>
#include <ctime>
#include <memory>

#include "Panel.h"
#include "Menu.h"

//------------------------------------------------------------------------------------------------------------
class AsCommander
{
public:
	~AsCommander();
	bool Init();
	void run();

private:
	bool update();
	void addNextMenuItem(unsigned short& index, unsigned short& x_pos, const unsigned short x_step, const wchar_t* key, const wchar_t* name);
	void buildMenu();
	void checkHandleInput(INPUT_RECORD& input_record, unsigned long& records_count);

	bool can_run_ = true;
	bool need_redraw_ = true;
	bool transfer_mode_ = false;
	bool copy_mode_ = false;

	unsigned short last_time_min_{};
	unsigned short screen_half_width_{};

	HANDLE std_input_handle_{};
	HANDLE std_output_handle_{};
	HANDLE screen_buffer_handle_{};

	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info_{};

	std::unique_ptr<CHAR_INFO[]> screen_buffer_;
	std::unique_ptr<SMessageBox> massage_box_;
	std::unique_ptr<STime> time_;

	std::shared_ptr<APanel> left_panel_;
	std::shared_ptr<APanel> right_panel_;
	std::shared_ptr<APanel> active_panel_;

	std::vector<std::unique_ptr<AMenu_Item>> menu_items_;
};
//------------------------------------------------------------------------------------------------------------
