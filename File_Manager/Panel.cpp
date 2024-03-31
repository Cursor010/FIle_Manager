#include "Panel.h"

// APanel
//------------------------------------------------------------------------------------------------------------
APanel::APanel(unsigned short x_pos, unsigned short y_pos, unsigned short width, unsigned short height, std::unique_ptr<CHAR_INFO[]>& screen_buffer)
	: x_pos_(x_pos), y_pos_(y_pos), width_(width), height_(height), screen_buffer_(screen_buffer.get()), screen_width_(width * 2)
{
	wchar_t curr_dir_[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, curr_dir_);
	current_directory_ = static_cast<std::wstring>(curr_dir_);
}
//------------------------------------------------------------------------------------------------------------
std::wstring APanel::transfer_file_name_;
std::wstring APanel::existing_path_transf;
std::wstring APanel::copy_file_name_;
std::wstring APanel::existing_path_copy;
//------------------------------------------------------------------------------------------------------------
void APanel::update()
{// Updating the panel
	displayPanel();
	displayAllFiles();
	displayFileHighlight();
	displaySelectedFileInfo();
	displayCurrentDirectoryInfo();

	createText(static_cast<unsigned short>(width_ * 2 - time_->getTime().size() + 1), 0, time_->getTime().c_str(), 0xb0);
}
//------------------------------------------------------------------------------------------------------------
void APanel::onEnter()
{// Enter the directory / Exit the directory / Open the file
	file_descriptor_ = std::make_unique<AFile_Descriptor>(*files_[curr_file_index_]);

	if (file_descriptor_->attributes_ & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (file_descriptor_->file_name_ == L"..")
		{
			size_t found = current_directory_.find_last_of(L"\\");
			if (found != std::wstring::npos)
			{
				last_directory_name_ = current_directory_.substr(found + 1);
				current_directory_ = current_directory_.substr(0, static_cast<int>(found));
				getDirectoryFiles(EGetFilesMode::EXIT);
			}
		}
		else
		{
			current_directory_ = current_directory_ + L"\\" + file_descriptor_->file_name_;
			getDirectoryFiles(EGetFilesMode::ENTRANCE);
		}
	}
	else
	{
		std::wstring file_path = current_directory_ + L"\\" + file_descriptor_->file_name_;

		ShellExecute(NULL, L"open", file_path.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}
//------------------------------------------------------------------------------------------------------------
void APanel::getDirectoryFiles(EGetFilesMode get_files_mode)
{// Get the directory files
	bool find_last_dir = false;
	const std::wstring file_path = current_directory_ + L"\\*.*";
	WIN32_FIND_DATAW find_data{};
	HANDLE search_handle{};

	files_.clear();

	try
	{
		search_handle = FindFirstFileW(file_path.c_str(), &find_data);

		if (search_handle != INVALID_HANDLE_VALUE)
		{
			while (FindNextFileW(search_handle, &find_data))
			{
				file_descriptor_ = std::make_unique<AFile_Descriptor>(find_data.dwFileAttributes, find_data.nFileSizeLow, find_data.nFileSizeHigh, find_data.cFileName, find_data.ftLastWriteTime);
				files_.push_back(std::move(file_descriptor_));
			}
			switch (get_files_mode)
			{
			case EGetFilesMode::UPDATE:
				break;

			case EGetFilesMode::ENTRANCE:
				curr_file_index_ = 0;
				highlight_x_offset_ = 0;
				highlight_y_offset_ = 0;
				break;

			case EGetFilesMode::EXIT:
				for (auto it = files_.begin(); it != files_.end(); ++it)
				{
					if (!find_last_dir)
					{
						if ((*it)->file_name_ != last_directory_name_)
						{
							changeSelectedFilePosition(EMoveDirection::DOWN);
						}
						else
						{
							find_last_dir = true;
						}
					}
				}
				break;
			}
			FindClose(search_handle);
		}

	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception caught in APanel::getDirectoryFiles: " << e.what() << std::endl;
	}

}
//------------------------------------------------------------------------------------------------------------
void APanel::changeSelectedFilePosition(EMoveDirection move_direction)
{// Changing the position of the selected file
	const unsigned short HALF_WIDTH = width_ / 2;

	number_files_ = static_cast<unsigned short>(files_.size()) - 1;

	switch (move_direction)
	{
	case EMoveDirection::UP:
		if (curr_file_index_ > 0)
		{
			if (highlight_x_offset_ >= HALF_WIDTH && highlight_y_offset_ == 0)
			{
				highlight_x_offset_ -= HALF_WIDTH;
				highlight_y_offset_ += height_ - 6;
				--curr_file_index_;
			}
			else
			{
				--highlight_y_offset_;
				--curr_file_index_;
			}
		}
		break;
	case EMoveDirection::LEFT:
		if (highlight_x_offset_ >= HALF_WIDTH)
		{
			highlight_x_offset_ -= HALF_WIDTH;
			curr_file_index_ -= height_ - 5;
		}
		else
		{
			highlight_y_offset_ = 0;
			curr_file_index_ = 0;
		}
		break;
	case EMoveDirection::DOWN:
		if (highlight_x_offset_ < HALF_WIDTH)
		{
			if (curr_file_index_ + 1 <= number_files_)
			{
				if (highlight_y_offset_ > height_ - 7 && highlight_x_offset_ < HALF_WIDTH)
				{
					highlight_x_offset_ += HALF_WIDTH;
					highlight_y_offset_ = 0;
					++curr_file_index_;
				}
				else
				{
					++highlight_y_offset_;
					++curr_file_index_;
				}
			}
		}
		else
		{
			if (curr_file_index_ + 1 <= number_files_ && highlight_y_offset_ <= height_ - 7)
			{
				if (highlight_y_offset_ > height_ - 7 && highlight_x_offset_ < HALF_WIDTH)
				{
					highlight_x_offset_ += HALF_WIDTH;
					highlight_y_offset_ = 0;
					++curr_file_index_;
				}
				else
				{
					++highlight_y_offset_;
					++curr_file_index_;
				}
			}
		}
		break;
	case EMoveDirection::RIGHT:
		if (highlight_x_offset_ < HALF_WIDTH)
		{
			if (number_files_ <= height_ - 6)
			{
				highlight_y_offset_ = number_files_;
				highlight_x_offset_ = 0;
				curr_file_index_ = number_files_;
			}

			else if (curr_file_index_ + height_ - 5 <= number_files_)
			{
				highlight_x_offset_ += HALF_WIDTH;
				curr_file_index_ += height_ - 5;
			}
			else
			{
				highlight_x_offset_ += HALF_WIDTH;
				highlight_y_offset_ = number_files_ - height_ + 5;
				curr_file_index_ = number_files_;
			}
		}
		else
		{
			if (number_files_ > (height_ - 5) * 2)
			{
				highlight_y_offset_ = height_ - 6;
				curr_file_index_ = (height_ - 6) * 2 + 1;
			}
			else
			{
				highlight_y_offset_ += number_files_ - curr_file_index_;
				curr_file_index_ = number_files_;
			}
		}
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void APanel::displayPanel() const
{// Displaying the panel grid
	const ASymbol HORIZONTAL_SYMBOL_TOP(L'═', 0x1b, L'╔', L'╗');
	const ASymbol HORIZONTAL_SYMBOL_BOTTOM(L'═', 0x1b, L'╚', L'╝');
	const ASymbol VERTICAL_SYMBOL(L'║', 0x1b, L'║', L'║');
	const ASymbol MID_HORIZONTAL_SYMBOL(L'─', 0x1b, L'╟', L'╢');
	const ASymbol MID_VERTICAL_SYMBOL(L'║', 0x1b, L'╦', L'╨');

	ASymbol clearSymbol(L' ', 0x1b, L' ', L' ');
	SArea_Pos area_pos(x_pos_ + 1, y_pos_ + 1, screen_width_, width_ - 2, height_ - 2);
	Clear_Area(screen_buffer_.get(), area_pos, clearSymbol);

	createHorizontalLine(y_pos_, HORIZONTAL_SYMBOL_TOP);
	createHorizontalLine(y_pos_ + height_ - 1, HORIZONTAL_SYMBOL_BOTTOM);
	createHorizontalLine(y_pos_ + height_ - 3, MID_HORIZONTAL_SYMBOL);

	createVerticalLine(x_pos_, y_pos_ + 1, height_ - 4, VERTICAL_SYMBOL);
	createVerticalLine(x_pos_ + width_ - 1, y_pos_ + 1, height_ - 4, VERTICAL_SYMBOL);
	createVerticalLine(x_pos_ + width_ / 2, y_pos_, height_ - 4, MID_VERTICAL_SYMBOL);

	createText(x_pos_ + 1, 1, L"n", 0x16);
	createText(x_pos_ + width_ / 4 - 2, 1, L"Name", 0x16);
	createText(x_pos_ + (width_ / 4) * 3 - 2, 1, L"Name", 0x16);
}
//------------------------------------------------------------------------------------------------------------
void APanel::displayCurrentDirectoryInfo()
{// Display information about the current directory
	unsigned long long directory_size{};
	unsigned short count_directory{};
	unsigned short count_files{};

	std::wstring directory_info;

	for (size_t i = 0; i < files_.size(); ++i)
	{
		std::unique_ptr<AFile_Descriptor>& file_descriptor_ = files_[i];

		if (file_descriptor_->attributes_ & FILE_ATTRIBUTE_DIRECTORY)
		{
			++count_directory;
		}
		else
		{
			directory_size += file_descriptor_->file_size_;
			++count_files;
		}
	}

	directory_info = L" Bytes: " + converter_->convertSize(directory_size) + L", files: " + std::to_wstring(count_files) + L", folders: " + std::to_wstring(count_directory - 1) + L" ";

	createText(x_pos_ + static_cast<unsigned short>((width_ - directory_info.size()) / 2), height_ - 1, directory_info.c_str(), 0x1b);
	createText(x_pos_ + static_cast<unsigned short>((width_ - current_directory_.size()) / 2 + 1), 0, (L" " + current_directory_ + L" ").c_str(), directory_highlight_);
}
//------------------------------------------------------------------------------------------------------------
void APanel::displaySingleFile(AFile_Descriptor* file, const unsigned short x_offset, const unsigned short y_offset, const unsigned short bg_attribute) const
{// Display one file
	unsigned short attributes{};

	if (existing_path_transf == current_directory_ + L"\\" + file->file_name_)
	{
		attributes = bg_attribute | 0x06;
	}
	else if (existing_path_copy == current_directory_ + L"\\" + file->file_name_)
	{
		attributes = bg_attribute | 0x06;
	}
	else if (file->attributes_ & FILE_ATTRIBUTE_DIRECTORY)
	{
		attributes = bg_attribute | 0x0f;
	}
	else if (file->file_name_.length() >= 3 &&
		(file->file_name_.substr(file->file_name_.length() - 3) == L"exe" ||
			file->file_name_.substr(file->file_name_.length() - 3) == L"cmd"))
	{
		attributes = bg_attribute | 0x0a;
	}
	else
	{
		attributes = bg_attribute | 0x0b;
	}

	SText_Pos pos(x_pos_ + x_offset + 1, y_pos_ + y_offset + 2, screen_width_, attributes);
	Draw_Limited_Text(screen_buffer_.get(), pos, file->file_name_.c_str(), width_ / 2 - 2);
}
//------------------------------------------------------------------------------------------------------------
void APanel::displayAllFiles() const
{// Display all files
	unsigned short x_offset{};
	unsigned short y_offset{};

	for (const auto& file : files_) {
		displaySingleFile(file.get(), x_offset, y_offset, 0x10);
		++y_offset;

		if (y_offset >= height_ - 5)
		{
			if (x_offset == 0)
			{
				x_offset += width_ / 2;
				y_offset = 0;
			}
			else
			{
				break;
			}
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void APanel::displaySelectedFileInfo()
{// Display information about the selected file
	unsigned short time_pos_x;
	unsigned short size_file_pos_x;

	std::wstring file_time(17, L'\0');
	std::wstring file_size_{};

	file_descriptor_ = std::make_unique<AFile_Descriptor>(*files_[curr_file_index_]);

	swprintf(&file_time[0], file_time.size(), L"%02hu.%02hu.%02hu %02hu:%02hu", file_descriptor_->stLocal_.wDay, file_descriptor_->stLocal_.wMonth, file_descriptor_->stLocal_.wYear % 100, file_descriptor_->stLocal_.wHour, file_descriptor_->stLocal_.wMinute);

	if (file_descriptor_->attributes_ & FILE_ATTRIBUTE_DIRECTORY)
	{
		file_size_ = (file_descriptor_->file_name_ == L"..") ? L"Up" : L"Folder";
	}
	else
	{
		file_size_ = converter_->convertSize(file_descriptor_->file_size_);
	}

	time_pos_x = static_cast<unsigned short>(x_pos_ + width_ - file_time.size() + 2);
	size_file_pos_x = static_cast<unsigned short>(x_pos_ + width_ - file_time.size() - file_size_.size() + 1);

	createText(x_pos_ + 1, height_ - 2, file_descriptor_->file_name_.c_str(), 0x1b);
	createText(time_pos_x, height_ - 2, file_time.c_str(), 0x1b);
	createText(size_file_pos_x, height_ - 2, file_size_.c_str(), 0x1b);
}
//------------------------------------------------------------------------------------------------------------
bool APanel::deleteSelectedFile()
{// Delete the selected file
	try
	{
		file_descriptor_ = std::make_unique<AFile_Descriptor>(*files_[curr_file_index_]);

		if (file_descriptor_->file_name_ != L"..")
		{
			if (massage_box_->showMessageBox(L"Delete", L"Do you want to delete " + file_descriptor_->file_name_ + L"?"))
			{
				std::wstring path = current_directory_ + L"\\" + file_descriptor_->file_name_;

				if (std::filesystem::exists(path))
				{
					if (std::filesystem::is_regular_file(path))
					{
						std::filesystem::remove(path);
					}
					else if (std::filesystem::is_directory(path))
					{
						std::filesystem::remove_all(path);
					}

					if (number_files_ == curr_file_index_)
					{
						changeSelectedFilePosition(EMoveDirection::UP);
						--number_files_;
					}

					getDirectoryFiles(EGetFilesMode::UPDATE);
					displayCurrentDirectoryInfo();

					return true;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error in APanel::deleteSelectedFile: " << e.what() << std::endl;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool APanel::transferSelectedFile(const bool transfer_mode)
{// Cut file/Paste file
	try
	{
		file_descriptor_ = std::make_unique<AFile_Descriptor>(*files_[curr_file_index_]);

		if (file_descriptor_->attributes_ != FILE_ATTRIBUTE_DIRECTORY && !transfer_mode && massage_box_->showMessageBox(L"Transfer", L"Do you want to cut file " + file_descriptor_->file_name_ + L"?"))
		{
			transfer_file_name_ = file_descriptor_->file_name_;
			existing_path_transf = (current_directory_ + L"\\" + transfer_file_name_);

			files_.erase(files_.begin() + curr_file_index_);

			if (number_files_ == curr_file_index_)
			{
				changeSelectedFilePosition(EMoveDirection::UP);
			}

			return true;
		}
		else if (transfer_mode && massage_box_->showMessageBox(L"Transfer", L"Do you want to insert file " + transfer_file_name_ + L"?"))
		{
			std::wstring new_file_path = (current_directory_ + L"\\" + transfer_file_name_);
			if (MoveFile(existing_path_transf.c_str(), new_file_path.c_str()))
			{
				existing_path_transf.clear();
				return true;
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error in APanel::transferSelectedFile: " << e.what() << std::endl;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool APanel::copySelectedFile(const bool copy_mode)
{// Copy file/Paste file
	try
	{
		file_descriptor_ = std::make_unique<AFile_Descriptor>(*files_[curr_file_index_]);

		if (file_descriptor_->attributes_ != FILE_ATTRIBUTE_DIRECTORY && !copy_mode && massage_box_->showMessageBox(L"Copy", L"Do you want to copy file " + file_descriptor_->file_name_ + L"?"))
		{
			copy_file_name_ = file_descriptor_->file_name_;
			existing_path_copy = current_directory_ + L"\\" + copy_file_name_;

			files_.erase(files_.begin() + curr_file_index_);

			if (number_files_ == curr_file_index_)
			{
				changeSelectedFilePosition(EMoveDirection::UP);
			}

			return true;
		}
		else if (copy_mode && massage_box_->showMessageBox(L"Copy", L"Do you want to insert file " + copy_file_name_ + L"?"))
		{
			std::wstring new_file_path = current_directory_ + L"\\" + copy_file_name_;

			if (std::filesystem::exists(existing_path_copy)) 
			{
				if (std::filesystem::is_regular_file(existing_path_copy)) 
				{
					std::string fileName = std::filesystem::path(existing_path_copy).filename().string();

					std::string destinationPath;

					destinationPath.assign(current_directory_.begin(), current_directory_.end());

					destinationPath += "\\" + fileName;

					std::filesystem::copy_file(existing_path_copy, destinationPath, std::filesystem::copy_options::update_existing);

					existing_path_copy.clear();

					return true;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error in APanel::copySelectedFile: " << e.what() << std::endl;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void APanel::createHorizontalLine(const unsigned short y, const ASymbol& symbol) const
{
	SPos pos(x_pos_, y, screen_width_, width_ - 2);
	Draw_Line_Horizontal(screen_buffer_.get(), pos, symbol);
}
//------------------------------------------------------------------------------------------------------------
void APanel::createVerticalLine(const unsigned short x, const unsigned short y1, const unsigned short y2, const ASymbol& symbol) const
{
	SPos pos(x, y1, screen_width_, y2);
	Draw_Line_Vertical(screen_buffer_.get(), pos, symbol);
}
//------------------------------------------------------------------------------------------------------------
void APanel::createText(const unsigned short x, const unsigned short y, const wchar_t* text, const unsigned short attributes) const
{
	SText_Pos pos(x, y, screen_width_, attributes);
	Draw_Text(screen_buffer_.get(), pos, text);
}
//------------------------------------------------------------------------------------------------------------
void APanel::displayFileHighlight()
{// Display the highlight bar for the current file

	file_descriptor_ = std::make_unique<AFile_Descriptor>(*files_[curr_file_index_]);

	displaySingleFile(file_descriptor_.get(), highlight_x_offset_, highlight_y_offset_, 0x20);
}
//------------------------------------------------------------------------------------------------------------
void APanel::setDirectoryHighlight(unsigned short directory_highlight)
{
	directory_highlight_ = directory_highlight;
}
//------------------------------------------------------------------------------------------------------------

