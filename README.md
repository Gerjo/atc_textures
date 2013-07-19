atc_textures
============
Command line tool to convert PNG images into ATC textures. Internally relies on libPNG and libz. libPNG is bundled for convenience sake, libz must be installed separately.


ATC compression is a proprietary algorithm. There are no publicly available tools to create ATC textures with. Qualcomm did release static libraries to create your own tools with, hence this project was created. The output format uses the KTX container, which is well documented by Khronos: http://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/


This project is a side-artifact of my graduation project at Game Oven Studios. Here the project is part of a larger toolchain used to automatically create hardware compressed textures for Google's Android and Apple's iOS. NB: by no means should this code be taken as a benchmark of my personal coding-skill, this project does its job - and that is it.

Keywords: ATC_RGB_AMD, ATC_RGBA_EXPLICIT_ALPHA_AMD, ATC_RGBA_INTERPOLATED_ALPHA_AMD, Android Textures.



Usage:
============
./aticompress input.png output.ktc



Installation:
============
I have attached a makefile, just type 'make' to compile with default settings. You might need to install libz, if you haven't already.



Compatibility
============
This work is only tested on OS X 10.8 - I'll test/port to Linux when I get around.



Contact:
============
Gerard - gerjoo[at]gmail[dot]com



TODO:
============
- More error checking (such as, power of two tests);
- Mipmapping;
- Linux support;
- Fix memory leaks.



License:
============
I'm far from a licensing expert. The bits written by me are released under the MIT license, which is quite liberal - this includes main.cpp and makefile. All other code and binaries shall remain under their own respective licenses.



Sample:
============
![Comparison of ATC formats](https://raw.github.com/Gerjo/atc_textures/master/comparison.png "Logo Title Text 1")

