This project contains two parts. First is the System which creates and trains images which then used to recoginise images.
Second part is the GUI. It takes images and uses the System code to display output images and results containing for that input image.

You can run this code on both Linux and Windows. I used Ubuntu to run System code and Windows for GUI code.

Requirements:

C++ Compiler: A C++ compiler with support for C++14 or higher is required. Examples of such compilers include GCC, Clang, and Microsoft Visual Studio.

OpenCV: You can install OpenCV using package managers (e.g., apt, brew) or by building it from source.

CMake:
For Windows:
Visit the CMake download page: https://cmake.org/download/
Download the latest version of CMake for Windows by clicking on the "Windows x64 Installer" link.
Run the downloaded installer and follow the installation instructions.

Qt framework: This code uses Qt, a cross-platform application framework for developing application software. You need to install the Qt framework along with Qt Creator (the integrated development environment). You can download it from the official website: https://www.qt.io/download.

Set up the project: Create a new Qt Widgets Application project in Qt Creator. Add the provided code to the appropriate files (e.g., buildinglocator.h and buildinglocator.cpp). Include the necessary Qt header files if they are not already included.

DBoW2: The code relies on DBoW2, a library for image recognition and loop closure detection. You need to download and build the library from the source code provided at: https://github.com/dorian3d/DBoW2

You will need to build DBoW2. Here is how on Ubuntu:
1. sudo apt-get install cmake libopencv-dev libboost-all-dev
2. git clone https://github.com/dorian3d/DBoW2.git
3. 	cd DBoW2
	mkdir build
	cd build
4. cmake ..
5. make
6. sudo make install


First Run the System code then Run GUI on QT Creator. 

