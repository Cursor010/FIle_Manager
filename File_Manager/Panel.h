#pragma once

#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>

#include "Asm_Tools_Interface.h"

//------------------------------------------------------------------------------------------------------------
class APanel
{
public:
	APanel(unsigned short x_pos, unsigned short y_pos, unsigned short width, unsigned short height, std::unique_ptr<CHAR_INFO[]>& screen_buffer);
	void update();
	void onEnter();
	void getDirectoryFiles(EGetFilesMode get_files_mode);
	void changeSelectedFilePosition(EMoveDirection move_direction);
	bool deleteSelectedFile();
	bool transferSelectedFile(const bool transfer_mode);
	void setDirectoryHighlight(unsigned short directory_highlight);

private:
	void displayPanel() const;
	void displayCurrentDirectoryInfo();
	void displayAllFiles() const;
	void displaySingleFile(AFile_Descriptor* file, const unsigned short x_offset, const unsigned short y_offset, const unsigned short bg_attribute) const;
	void displaySelectedFileInfo();
	void displayFileHighlight();
	void createHorizontalLine(const unsigned short y, const ASymbol& symbol) const;
	void createVerticalLine(const unsigned short x, const unsigned short y1, const unsigned short y2, const ASymbol& symbol) const;
	void createText(const unsigned short x, const unsigned short y, const wchar_t* text, const unsigned short attributes) const;

	unsigned short width_{};
	unsigned short height_{};
	unsigned short screen_width_{};
	unsigned short x_pos_{};
	unsigned short y_pos_{};
	unsigned short highlight_x_offset_{};
	unsigned short highlight_y_offset_{};
	unsigned short curr_file_index_{};
	unsigned short number_files_{};
	unsigned short directory_highlight_{};

	std::wstring last_directory_name_{};
	std::wstring current_directory_{};

	static std::wstring transfer_file_name_;
	static std::wstring existing_file_path_;

	std::unique_ptr<CHAR_INFO[]> screen_buffer_;
	std::unique_ptr<AFile_Descriptor> file_descriptor_;
	std::unique_ptr<SConverter> converter_;
	std::unique_ptr<SMessageBox> massage_box_;
	std::unique_ptr<STime> time_;

	std::vector<std::unique_ptr<AFile_Descriptor>> files_;
};
//------------------------------------------------------------------------------------------------------------
