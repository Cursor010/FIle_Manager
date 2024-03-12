<p align="center">
  <a href="#about project">About project</a> |
  <a href="#screenshots">Screenshots</a> |
  <a href="#feature">Feature</a> |
  <a href="#building">Building</a> |
  <a href="#management">Management</a> |
</p>

## About project:
The project is a financial manager who knows how to work with basic file operations. The program consists of two panels identical in design and functionality, a current time line and a menu.
### Each panel has: 
+ Two tabs for file output;
+ The name of the current directory;
+ Information about the current directory;
+ Information about the selected file;

## Screenshots:
![program interface](https://github.com/Cursor010/FIle_Manager/tree/main/screenshots/ProgramIntarface.png)
![changing panel](https://github.com/Cursor010/FIle_Manager/tree/main/screenshots/ChangingPanel.png)
![cutting file](https://github.com/Cursor010/FIle_Manager/tree/main/screenshots/CuttingFile.png)
![deleting file](https://github.com/Cursor010/FIle_Manager/tree/main/screenshots/DeletingFile.png)
![closing program](https://github.com/Cursor010/FIle_Manager/tree/main/screenshots/ClosingProgram.png)

## Feature: 
+ Information output in alphanumeric mode, in which the entire screen is divided into cells;
+ Created without the help of frameworks;

## Building: 

+ Project->Properties->C++ Language Standard->ISO C++17 Standard (/std:c++17) or above;

+ Project->Build Customizations->masm(.targets, .props);

+ Solution Explorer->Asm_Tools.asm->Properties->Configuration Properties->General->Item Type->Microsoft Macro Assembler;

+ Solution Explorer->Asm_Tools.asm->File->Save Asm_Tools.asm as->Save with Encoding->Encoding->Unicode (UTF-8 without signature) - Codepage 65001;

## Management:
+ Q — switching off the active panel;
+ W — move up;
+ A — move to the left/to the beginning;
+ S — move down;
+ D — move to the right/to the end;
+ F5 — cutting the file (at a low level, the file is cut, when the file is re-inserted along the selected path);
+ F8 — deleting the file/folder/archive;
+ F10 — exit the program;
