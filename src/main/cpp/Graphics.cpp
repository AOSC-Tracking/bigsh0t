/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>

#include "Graphics.hpp"
#include "font8x8.hpp"


Graphics::Graphics(uint32_t* buffer, int width, int height) {
    this->buffer = buffer;
    this->width = width;
    this->height = height;
}

Graphics::~Graphics() {
}

void Graphics::plot (int x, int y, uint32_t mask, uint32_t color) {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }
    int i = y * width + x;
    buffer[i] &= mask;
    buffer[i] |= color;
}

void Graphics::fillRect (int x, int y, int w, int h, uint32_t mask, uint32_t color) {
    for (int yp = y; yp < y + h; ++yp) {
        for (int xp = x; xp < x + w; ++xp) {
            plot(xp, yp, mask, color);
        }
    }
}

void Graphics::drawRect (int x, int y, int w, int h, uint32_t mask, uint32_t color) {
    for (int yp = y; yp < y + h; ++yp) {
        plot(x, yp, mask, color);
        plot(x + w - 1, yp, mask, color);
    }
    for (int xp = x + 1; xp < x + w - 1; ++xp) {
        plot(xp, y, mask, color);
        plot(xp, y + h - 1, mask, color);
    }
}

void Graphics::drawText (int x, int y, const std::string& text, uint32_t mask, uint32_t color) {
    int xpos = x;
    int ypos = y;
    for (const char& c : text) {
        if (c >= 0 && c < 0x80) {
            if (c == 0xa) {
                xpos = x;
                ypos += 20;
            } else if (c == 0xd) {
            } else {
                unsigned char* bitmap = font8x8_basic[c];
                for (int cy = 0; cy < 8; ++cy) {
                    unsigned char rowBits = bitmap[cy];
                    for (int cx = 0; cx < 8; ++cx) {
                        if ((rowBits & 0x01) > 0) {
                            fillRect (xpos + cx * 2, ypos + cy * 2, 2, 2, mask, color);
                        }
                        rowBits >>= 1;
                    }
                }
                xpos += 16;
            }
        }
    }
}

void Graphics::blt(const uint32_t* in, int x0, int y0, int inWidth, int inHeight) {
    for (int x = 0; x < inWidth; ++x) {
        for (int y = 0; y < inHeight; ++y) {
            buffer[(y + y0) * width + (x + x0)] = in[y * inWidth + x];
        }
    }
}
