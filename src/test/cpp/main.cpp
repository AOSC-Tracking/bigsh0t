#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <emmintrin.h>
#include <mmintrin.h>
#include <intrin.h>
#include <iomanip>
#include "../../main/cpp/Math.hpp"
#include "../../main/cpp/Matrix.hpp"
#include "../../main/cpp/MP4.hpp"
#include "../../main/cpp/ImageProcessing.hpp"

void testMP4() {
    MP4Parser parser(std::string("c:\\temp\\test.mp4"));
    std::vector<Quaternion> qs;
    
    parser.readRDT5(qs);
    
    parser.close();
    std::cout << qs.size() << std::endl;
}

template<typename T>
void assertEquals(T actual, T expected) {
    if (actual != expected) {
        std::cout << "Expected " << std::hex << expected << " was " << actual << std::dec << std::endl;
        throw std::runtime_error("Assertion failed");
    }
}

bool assertComponentDifferenceLessThan(uint32_t a, uint32_t b, int diffmax, char* msg) {
    int c0a = (a >> 24) & 0xff;
    int c1a = (a >> 16) & 0xff;
    int c2a = (a >>  8) & 0xff;
    int c3a = (a      ) & 0xff;
    
    int c0b = (b >> 24) & 0xff;
    int c1b = (b >> 16) & 0xff;
    int c2b = (b >>  8) & 0xff;
    int c3b = (b      ) & 0xff;
    
    if (abs(c0a - c0b) >= diffmax || abs(c1a - c1b) >= diffmax ||
        abs(c2a - c2b) >= diffmax || abs(c3a - c3b) >= diffmax) {
        /*
        std::cout << "Difference @ " << (x + xoff) << "," << (y + yoff) << std::endl;
        std::cout << std::hex << a << " != " << b << std::dec << std::endl;
        
        uint32_t idx = y * width + x;
        std::cout << std::hex << frame[idx] << "," << frame[idx + 1] << "," << frame[idx+width] << "," << frame[idx+width+1] << std::endl;
        */
        std::cout << "Assertion failed" << std::endl;
            std::cout << msg << std::endl;
        std::cout << "Expected " << std::hex << b << " was " << a << std::dec << std::endl;
        return false;
    }
    return true;
}

uint32_t lerp(uint32_t a, uint32_t b, int x) {
    int c0a = (a >> 24) & 0xff;
    int c1a = (a >> 16) & 0xff;
    int c2a = (a >>  8) & 0xff;
    int c3a = (a      ) & 0xff;
    
    int c0b = (b >> 24) & 0xff;
    int c1b = (b >> 16) & 0xff;
    int c2b = (b >>  8) & 0xff;
    int c3b = (b      ) & 0xff;
    
    int c0c = c0a + (((c0b - c0a) * x) >> 7);
    int c1c = c1a + (((c1b - c1a) * x) >> 7);
    int c2c = c2a + (((c2b - c2a) * x) >> 7);
    int c3c = c3a + (((c3b - c3a) * x) >> 7);
    
    return (c0c << 24) | (c1c << 16) | (c2c << 8) | c3c;
}

uint32_t referenceBlerp(uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height) {
    uint32_t a = frame[ai];
    uint32_t b = frame[bi];
    uint32_t c = frame[ci];
    uint32_t d = frame[di];
    
    uint32_t e = lerp(a, b, ax);
    uint32_t f = lerp(c, d, ax);
    return lerp(e, f, ay);
}

void testBlerp() {
    int width = 4096;
    int height = 2048;
    size_t frameSize = width * height;
    uint32_t* frame = (uint32_t*) malloc(frameSize * sizeof(uint32_t));
    for (int i = 0; i < frameSize; ++i) {
        frame[i] = std::rand() & 0xffffffff;
    }
    
    frame[0] = 0xfffffd6e;
    frame[1] = 0xffffc8d6;
    frame[width] = 0xffffe705;
    frame[width + 1] = 0xffff95d0;
    
    // 110 214 = 188
    //   5 208 = 157
    //           164.75 (a4)
    // Expected: ffffb4a5
    assertEquals(referenceBlerp(frame, 0, 1, width, width + 1, 96, 96, width, height), 0xffffb4a4);
    assertEquals(int64Blerp(frame, 0, 1, width, width + 1, 96, 96, width, height), 0xffffb4a4);
    #ifdef USE_SSE
    assertEquals(sseBlerp(frame, 0, 1, width, width + 1, 96, 96, width, height), 0xffffb4a4);
    #endif
    
    std::cout << "Testing." << std::endl;
    
    for (int x = 0; x < width - 1; ++x) {
        for (int y = 0; y < height - 1; ++y) {
            int xoff = std::rand() % 128;
            int yoff = std::rand() % 128;
            
            int baseIdx = y * width + x;
            
            auto ref = referenceBlerp(frame, baseIdx, baseIdx + 1, baseIdx + width, baseIdx + width + 1, xoff, yoff, width, height);
            
            auto a = int64Blerp(frame, baseIdx, baseIdx + 1, baseIdx + width, baseIdx + width + 1, xoff, yoff, width, height);
            if (!assertComponentDifferenceLessThan(a, ref, 3, "i64")) {
                std::cout << std::hex << frame[baseIdx] << "," << frame[baseIdx + 1] << "," << frame[baseIdx + width] << "," << frame[baseIdx + width + 1] << std::dec << "," << xoff << "," << yoff << std::endl;
                throw std::runtime_error("Assertion failed");
            }
            
            #ifdef USE_SSE
            auto b = sseBlerp(frame, baseIdx, baseIdx + 1, baseIdx + width, baseIdx + width + 1, xoff, yoff, width, height);
            assertComponentDifferenceLessThan(b, ref, 3, "sse");
            #endif
        }
    }    
    
    free(frame);
}

int main(int argc, char* argv[]) {
    testBlerp();
    return 0;
}