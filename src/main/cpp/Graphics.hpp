/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef Graphics_HPP
#define Graphics_HPP

#include <inttypes.h>
#include <string>

class Graphics {
public:
	Graphics(uint32_t* buffer, int width, int height);
	~Graphics();
	
	void plot (int x, int y, uint32_t mask, uint32_t color);
	void fillRect (int x, int y, int w, int h, uint32_t mask, uint32_t color);
	void drawRect (int x, int y, int w, int h, uint32_t mask, uint32_t color);
	void drawText (int x, int y, const std::string& text, uint32_t mask, uint32_t color);
	
public:
	uint32_t* buffer;
	int width;
	int height;
};

#endif