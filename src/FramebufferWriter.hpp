#ifndef __FRAMEBUFFERWRITER_HPP__
#define __FRAMEBUFFERWRITER_HPP__

#include <cstdint>
#include "Closeable.hpp"

typedef struct{
	uint8_t b; //blue
	uint8_t	g; //green
	uint8_t	r; //red
	uint8_t	u; //unused
} pixel;

class FramebufferWriter : public Closeable{
private:
	pixel setColor;			//The color set for drawing
	uint16_t width;			//Width of the screen in pixels
	uint16_t height;		//Height of the screen in pixels
	uint32_t lineLength;

	std::size_t size;		//Size of the memory buffers
	pixel *buffer;			//Mapped framebuffer
	pixel *nextBuffer;		//Memory buffer to store buffer changes to be applied
public:
	struct{					//Contains the file descriptors for the resources
		int framebuffer;	//Framebuffer file descriptor
		int tty;			//Terminal file descriptor
	} descriptors;

public:
	/*
	* Constructor.
	* Acquires the required resources.
	* Throws InitializationError if the resource acquisition fails.
	*/
	FramebufferWriter();

	/*
	* Destructor.
	* Releases the acquired resources by calling onClose.
	*/
	~FramebufferWriter();
	
	/*
	* Returns the width of the screen in pixels.
	*/
	uint16_t getWidth();

	/*
	* Returns the height of the screen in pixels.
	*/
	uint16_t getHeight();
	
	/*
	* Sets the color used to draw the on the screen.
	* r: the red value of the color (0-255).
	* g: the green value of the color (0-255).
	* b: the blue value of the color (0-255).
	*/
	void color(uint8_t r, uint8_t g, uint8_t b);

	/*
	* Sets the color used to draw the on the screen.
	* color: Pixel type structure with r, g, b values.
	*/
	void color(const pixel& _color);

	/*
	* Sets the color used to draw the on the screen.
	* greyScale: the grayscale value of the color (0-255).
	*/
	void color(uint8_t greyScale);

	/*
	* Returns a the color being used.
	*/
	pixel getColor();

	/*
	* Fills all the pixels on the screen either with a color.
	* r: the red value of the color (0-255).
	* g: the green value of the color (0-255).
	* b: the blue value of the color (0-255).
	*/
	void background(uint8_t r, uint8_t g, uint8_t b);

	/*
	* Fills all the pixels on the screen either with a color.
	* greyScale: the grayscale value of the color (0-255).
	*/
	void background(uint8_t greyScale);

	/*
	* Implements Closeable onClose method.
	* Releases the acquired resources.
	*/
	void onClose();

	/*
	* Applies the prepared buffer to the screen.
	*/
	void applyBuffer();

	/*
	* Draws a rectangle to the screen using the set color.
	* x: X coordinates of the top left corner of the rectangle.
	* y: Y coordinates of the top left corner of the rectangle.
	* width: The width in pixel of the rectangle.
	* height: The height in pixel of the rectangle.
	*/
	void rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

    /*
	* Draws a pixel to the screen using the set color.
	* x: X coordinates of pixel to be drawn.
	* y: Y coordinates of pixel to be drawn. 
	*/
	void drawPixel(uint16_t x, uint16_t y); 
	
    /*
	* Draws a line from a point to another using the set color.
	* x0: X coordinates of the first point.
	* y0: Y coordinates of the first point. 
	* x1: X coordinates of the second point.
	* y1: Y coordinates of the second point. 
	*/
	void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

#endif