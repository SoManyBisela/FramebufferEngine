#include "FramebufferWriter.hpp"

#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <cstring>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <termios.h>

#include <sstream>

#include "InitializationError.hpp"

#define fbdev "/dev/fb0"
#define ttydev "/dev/tty"
#define MOD_NON 0 //no set mode
#define MOD_TXT 1 //text mode
#define MOD_GRA 2 //graphic mode

FramebufferWriter::FramebufferWriter() :
	descriptors({-1,-1}),
	buffer(nullptr),
	nextBuffer(nullptr),
	setColor({0}),
	Closeable(){
	/*
	* setting the terminal to graphics mode is necessary in order
	* to display the framebuffer without being interrupted from the terminal
	*/

	// opening the tty device
	descriptors.tty = open(ttydev, O_RDWR);
	if(descriptors.tty < 0){
		//call onClose to clear opened resources up to this point
		onClose();
		throw InitializationError("An error occurred while opening tty device.");
	}

	
	//setting the tty device to graphics mode
	if(ioctl(descriptors.tty, KDSETMODE, KD_GRAPHICS) == -1){
		onClose(); //call onClose to clear opened resources up to this point
		throw InitializationError("An error occurred while setting tty graphics mode.");
	}
	/**/

	
	// opening the framebuffer device
	descriptors.framebuffer = open(fbdev, O_RDWR);
	if(descriptors.framebuffer < 0){
		//call onClose to clear opened resources up to this point
		onClose();
		throw InitializationError("An error occurred while opening framebuffer device.");
	}

	fb_fix_screeninfo fix;
	fb_var_screeninfo var;

	// retrieving fixed framebuffer settings
	if(ioctl(descriptors.framebuffer, FBIOGET_FSCREENINFO, &fix) == -1){
		//call onClose to clear opened resources up to this point
		onClose();
		throw InitializationError("An error occurred while accessing framebuffer fixed info.");
	} 

	// retrieving variable framebuffer settings
	if(ioctl(descriptors.framebuffer, FBIOGET_VSCREENINFO, &var) == -1){
		//call onClose to clear opened resources up to this point
		onClose();
		throw InitializationError("An error occurred while accessing framebuffer variable info.");
	}

	/*
	* checking if requirements for correct functioning are met.
	* Throwing otherwise.
	*/

	int bitsPerPixel = var.bits_per_pixel;
	if(bitsPerPixel != sizeof(pixel) * 8){
		//call onClose to clear opened resources up to this point
		onClose();
		std::ostringstream cause;
		cause << "Framebuffer bits per pixel do not meet requirements"
				<< "\tRequired: " << (sizeof(pixel) * 8) << "\tFound: " << bitsPerPixel;
		throw InitializationError(cause.str());
	}

	//acquiring required info from framebuffer
	width = var.xres;
	height = var.yres;
	lineLength = fix.line_length;
	size = lineLength * height;

	//mapping framebuffer to memory
	buffer = (pixel*)mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptors.framebuffer, 0);


	if(buffer == MAP_FAILED){
		//call onClose to clear opened resources up to this point
		onClose();
		throw InitializationError("An error occurred while mapping framebuffer to memory.");
	}

	nextBuffer = (pixel*)malloc(size);
	if(nextBuffer == nullptr){
		//call onClose to clear opened resources up to this point
		onClose();
		throw InitializationError("An error occurred while allocating second buffer.");
	}

	memset(nextBuffer, 0, size);
}

FramebufferWriter::~FramebufferWriter(){
	onClose();
}

void FramebufferWriter::onClose(){
	// freeing allocated memory to buffer
	if(nextBuffer != nullptr){
		free(nextBuffer);
		nextBuffer = nullptr;
	}else{
		std::cout << "nextBuffer appears not created" << std::endl;
	}

	// unmapping framebuffer from memory
	if(buffer != nullptr && buffer != MAP_FAILED){
		munmap(buffer, size) == -1;
		buffer = nullptr;
	}else{
		std::cout << "buffer appears not created" << std::endl;
	}

	// closing framebuffer device
	if(descriptors.framebuffer >= 0){
		if(close(descriptors.framebuffer) == -1){
			std::cout << "descriptors.framebuffer failed to close" << std::endl;
		}
		descriptors.framebuffer = -1;
	}else{
		std::cout << "descriptors.framebuffer appears not created" << std::endl;
	}

	if(descriptors.tty >= 0){
		//flushing the characters written while in graphic mode
		if(tcflush(descriptors.tty, TCIOFLUSH) == -1){
			std::cout << "descriptors.tty: unable to flush unread characters" << std::endl;
		};

		// setting tty device to text mode
		if(ioctl(descriptors.tty, KDSETMODE, KD_TEXT) == -1){
			std::cout << "descriptors.tty: unable to set text mode" << std::endl;
		};

		//closing tty device
		if(close(descriptors.tty) == -1){
			std::cout << "descriptors.tty failed to close" << std::endl;
		}
		descriptors.tty = -1;
	}else{
		std::cout << "descriptors.tty appears not created" << std::endl;
	}
}

uint16_t FramebufferWriter::getWidth(){
	return width;
}

uint16_t FramebufferWriter::getHeight(){
	return height;
}

void FramebufferWriter::applyBuffer(){
	if(buffer == nullptr || nextBuffer == nullptr)
		return;
	memcpy(buffer, nextBuffer, size);
}

void FramebufferWriter::color(uint8_t r, uint8_t g, uint8_t b){
	setColor.r = r;
	setColor.g = g;
	setColor.b = b;
}

void FramebufferWriter::color(const pixel& _color){
	color(_color.r, _color.g, _color.b);
}

void FramebufferWriter::color(uint8_t greyScale){
	color(greyScale,greyScale,greyScale);
}

void FramebufferWriter::drawPixel(uint16_t x, uint16_t y){
	if(nextBuffer == nullptr)
		return;
	if(x >= width || y >= height || x < 0 || y < 0){
		return;
	}
	memcpy(nextBuffer + x + lineLength / sizeof(pixel) * y, &setColor, sizeof(pixel));
}

void FramebufferWriter::rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height){
	for(int i = 0; i < width; i++){
		for(int j= 0; j < height; j++){
			drawPixel(x+i, y+j);
		}
	}
}

void FramebufferWriter::background(uint8_t r,uint8_t g,uint8_t b){
	if(nextBuffer == nullptr)
		return;
	pixel _color;
	_color.r = r;
	_color.g = g;
	_color.b = b;
	memcpy(nextBuffer, &_color, sizeof(pixel));
	size_t copiedUntil = sizeof(pixel);
	while(copiedUntil < size){
		size_t copySize = (size - copiedUntil) * (size - copiedUntil <= copiedUntil) + (copiedUntil) * ( size - copiedUntil > copiedUntil);
		memcpy(nextBuffer + copiedUntil / sizeof(pixel), nextBuffer, copySize);
		copiedUntil += copySize;
	}
}

void FramebufferWriter::background(uint8_t greyscale){
	background(greyscale, greyscale, greyscale);
}

void FramebufferWriter::line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	double slope = (double)(y1 - y0) / (x1 - x0);
	if(slope <= 1 && slope >= -1){
		//y slope is more than x slope
		if(x1 < x0){
			std::swap(x1, x0);
			std::swap(y1, y0);
		}

		for(int i = 0; i <= x1 - x0 ; i++){
			drawPixel(x0 + i, y0 + i * slope);
		}
	}else{
		slope = (double)(x1 - x0) / (y1 - y0);
		//x slope is more than y slope
		if(y1 < y0){
			std::swap(x1, x0);
			std::swap(y1, y0);
		}
		
		for(int i = 0; i <= y1 - y0 ; i++){
			drawPixel(x0 + i * slope, y0 + i);
		}
	}
}
