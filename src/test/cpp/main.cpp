#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include "../../main/cpp/sse_compat.hpp"
#include <iomanip>
#include "../../main/cpp/Math.hpp"
#include "../../main/cpp/Matrix.hpp"
#include "../../main/cpp/MP4.hpp"
#include "../../main/cpp/ImageProcessing.hpp"
#include "../../main/cpp/SummedAreaTable.hpp"
#include "../../main/cpp/EMoR.hpp"

void testMP4() {
    MP4Parser parser(std::string("c:\\temp\\test.mp4"));
    std::vector<Quaternion> qs;

    parser.readRDT5(qs);

    parser.close();
    std::cout << qs.size() << std::endl;
}

void fail() {
    throw std::runtime_error("Assertion failed");
}


template<typename T>
void assertEquals(T actual, T expected) {
    if (actual != expected) {
        std::cout << "Expected " << std::hex << expected << " was " << actual << std::dec << std::endl;
        throw std::runtime_error("Assertion failed");
    }
}

void assertTrue(bool condition) {
    if (!condition) {
        throw std::runtime_error("Assertion failed");
    }
}


bool assertComponentDifferenceLessThan(uint32_t a, uint32_t b, int diffmax, const char* msg) {
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

    for (int x = 0; x < width - 1; ++x) {
        for (int y = 0; y < height - 1; ++y) {
            int xoff = std::rand() % 128;
            int yoff = std::rand() % 128;

            int baseIdx = y * width + x;

            auto ref = referenceBlerp(frame, baseIdx, baseIdx + 1, baseIdx + width, baseIdx + width + 1, xoff, yoff, width, height);

            auto a = int64Blerp(frame, baseIdx, baseIdx + 1, baseIdx + width, baseIdx + width + 1, xoff, yoff, width, height);
            if (!assertComponentDifferenceLessThan(a, ref, 3, "i64")) {
                std::cout << std::hex << frame[baseIdx] << "," << frame[baseIdx + 1] << "," << frame[baseIdx + width] << "," << frame[baseIdx + width + 1] << std::dec << "," << xoff << "," << yoff << std::endl;
                fail();
            }

#ifdef USE_SSE
            auto b = sseBlerp(frame, baseIdx, baseIdx + 1, baseIdx + width, baseIdx + width + 1, xoff, yoff, width, height);
            if (!assertComponentDifferenceLessThan(b, ref, 3, "sse")) {
                fail();
            }
#endif
        }
    }

    free (frame);
}

void benchmarkBlerp() {
    int width = 4096;
    int height = 2048;
    size_t frameSize = width * height;
    uint32_t* frame = (uint32_t*) malloc(frameSize * sizeof(uint32_t));
    for (int i = 0; i < frameSize; ++i) {
        frame[i] = std::rand() & 0xffffffff;
    }

    std::cout << "Benchmarking refrence... ";
    auto startRef = std::chrono::steady_clock::now();

    for (int iter = 0; iter < 10; ++iter) {
        for (int x = 0; x < width - 1; ++x) {
            for (int y = 0; y < height - 1; ++y) {
                int baseIdx = y * width + x;
                uint32_t ref = referenceBlerp(frame, baseIdx, baseIdx + 1, baseIdx + width, baseIdx + width + 1, 64, 64, width, height);
                frame[baseIdx] = ref;
            }
        }
    }

    auto endRef = std::chrono::steady_clock::now();

    // 4968ms
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(endRef - startRef).count() << "ms" << std::endl;

    std::cout << "Benchmarking int64... ";
    auto startInt64 = std::chrono::steady_clock::now();

    for (int iter = 0; iter < 10; ++iter) {
        for (int x = 0; x < width - 1; ++x) {
            for (int y = 0; y < height - 1; ++y) {
                int baseIdx = y * width + x;
                uint32_t ref = int64Blerp(frame, baseIdx, baseIdx + 1, baseIdx + width, baseIdx + width + 1, 64, 64, width, height);
                frame[baseIdx] = ref;
            }
        }
    }

    auto endInt64 = std::chrono::steady_clock::now();
    // 3262ms
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(endInt64 - startInt64).count() << "ms" << std::endl;

#ifdef USE_SSE
    std::cout << "Benchmarking SSE... ";
    auto startSSE= std::chrono::steady_clock::now();

    for (int iter = 0; iter < 10; ++iter) {
        for (int x = 0; x < width - 1; ++x) {
            for (int y = 0; y < height - 1; ++y) {
                int baseIdx = y * width + x;
                uint32_t ref = sseBlerp(frame, baseIdx, baseIdx + 1, baseIdx + width, baseIdx + width + 1, 64, 64, width, height);
                frame[baseIdx] = ref;
            }
        }
    }

    auto endSSE = std::chrono::steady_clock::now();
    // 2010ms
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(endSSE - startSSE).count() << "ms" << std::endl;
#endif

    free(frame);
}


void testFastAtan2() {
    double maxTolerableErr = 2.28e-07;
    double pixelError = (2 * M_PI / maxTolerableErr) / 128;
    double maxErr = 0.0;
    for (int iter = 0; iter < 300000; ++iter) {
        int x = std::rand() % 100000 + 1;
        int y = std::rand() % 100000 + 1;
        if (iter < 100000) {
            x = iter;
            y = 100000;
        } else if (iter <= 200000) {
            x = 100000;
            y = iter - 100000;
        }
        double fa = fastAtan2(y, x);
        double ref = atan2(y, x);

        double error = abs(fa - ref);
        if (error > maxErr) {
            maxErr = error;
        }
    }
    assertTrue(maxErr <= maxTolerableErr);
}

void testFastAtan2Double() {
    double maxTolerableErr = 2.28e-07;
    double pixelError = (2 * M_PI / maxTolerableErr) / 128;
    double maxErr = 0.0;
    for (int iter = 0; iter < 300000; ++iter) {
        double x = (std::rand() % 100000 + 1) / 100000.0;
        double y = (std::rand() % 100000 + 1) / 100000.0;
        if (iter < 100000) {
            x = iter;
            y = 100000;
        } else if (iter <= 200000) {
            x = 100000;
            y = iter - 100000;
        }
        double fa = fastAtan2(y, x);
        double ref = atan2(y, x);

        double error = abs(fa - ref);
        if (error > maxErr) {
            maxErr = error;
        }
    }
    assertTrue(maxErr <= maxTolerableErr);
}


void testTransform() {
    int width = 4096;
    int height = 2048;
    size_t frameSize = width * height;
    uint32_t* frame = (uint32_t*) malloc(frameSize * sizeof(uint32_t));
    for (int i = 0; i < frameSize; ++i) {
        frame[i] = std::rand() & 0xffffffff;
    }

    std::cout << "Benchmarking reference... "; // original was 7190ms, now 3743
    auto startRef = std::chrono::steady_clock::now();

    Transform360Support t360(width, height);
    for (int iter = 0; iter < 10; ++iter) {
        transform_360(t360, frame, frame, width, height, 0, height, 1.0, 1.0, 1.0, Interpolation::NONE);
    }

    auto endRef = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(endRef - startRef).count() << "ms" << std::endl;

    free(frame);
}

void testSummedAreaTable() {
    uint32_t zeros[] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    };
    uint32_t in[] = {
        0, 1, 2, 3, 4,
        5, 6, 7, 8, 9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    SummedAreaTable sat(5,4);
    sat.compute(zeros, 5, 0, 0, 5, 4);
    sat.dump();
    assertEquals(sat.sum(0, 0, 1, 1), (uint32_t) 0);

    sat.compute(in, 5, 0, 0, 5, 4);
    sat.dump();
    assertEquals(sat.sum(0, 0, 1, 1), (uint32_t) 0);
    assertEquals(sat.sum(0, 0, 2, 2), (uint32_t) (0 + 1 + 5 + 6));
    assertEquals(sat.sum(1, 1, 2, 2), (uint32_t) (6 + 7 + 11 + 12));
    assertEquals(sat.sum(0, 0, 5, 4), (uint32_t) 190);
    assertEquals(sat.sum(1, 1, 3, 2), (uint32_t) (6 + 7 + 8 + 11 + 12 + 13));

    assertEquals(sat.sum(3, 1, 4, 2), (uint32_t) (8 + 9 + 5 + 6 + 13 + 14 + 10 + 11));

    sat.compute(in, 5, 1, 1, 3, 2);
    assertEquals(sat.sum(0, 0, 2, 2), (uint32_t) (6 + 7 + 11 + 12));
    assertEquals(sat.sum(1, 0, 2, 2), (uint32_t) (7 + 8 + 12 + 13));
    assertEquals(sat.sum(2, 0, 2, 2), (uint32_t) (8 + 6 + 13 + 11));
    sat.dump();
}

void testEMoR() {
    std::vector<double> parameters;
    //Ra-3.30760216712952 Rb2.92867398262024 Rc0.716169774532318 Rd-0.31006196141243 Re-0.453573703765869
    parameters.push_back(-3.30760216712952);
    parameters.push_back(2.92867398262024);
    parameters.push_back(0.716169774532318);
    parameters.push_back(-0.31006196141243);
    parameters.push_back(-0.453573703765869);
    EMoR emor;
    emor.compute(parameters, 16, 255);
    emor.makeMonotone();
    emor.initialize();
    EMoR inv = EMoR().compute(parameters, 8, 65536);
    inv.makeMonotone();
    inv.initialize();
    for (int i = 0; i < 256; i += 1) {
        std::cout << emor.sampleInt(i * 256) << ";" << inv.sampleInt(i) << std::endl;
    }
}

void testShrinkAndAccumulate() {
    uint32_t in[] = {
         0,  1,  2,  3,  4,  5,  6,  7,
         5,  6,  7,  8,  9, 10, 11, 12,
        10, 11, 12, 13, 14, 15, 16, 17,
        15, 16, 17, 18, 19, 20, 21, 22
    };
    uint32_t out[] {
        0, 0, 0, 0,
        0, 0, 0, 0, 1
    };
    uint32_t expected[] {
        0+1+5+6, 2+3+7+8, 4+5+9+10, 6+7+11+12,
        10+11+15+16, 12+13+17+18, 14+15+19+20, 16+17+21+22
    };
    shrinkAndAccumulate(in, out, 8, 4, 2, 4, 2);
    for (int i = 0; i < 8; ++i) {
        assertEquals(out[i], expected[i]);
    }
    assertEquals(out[8], (uint32_t) 1);
}

void testSampleBilinearWrappedClampedMono() {
    uint32_t in[] = {
        0, 100,
        200, 300
    };

    auto sample = sampleBilinearWrappedClampedMono(in, 0.5, 0.5, 2, 2);
    assertEquals(sample, (uint32_t) 150);
}

void testDiff() {
    uint32_t a[] = {
        0, 100,
        200, 300
    };
    uint32_t b[] = {
        0, 0,
        0, 0
    };

    auto d1 = diff(a, b, 2, 2, INT64_MAX);
    auto d2 = diff(b, a, 2, 2, INT64_MAX);
    assertEquals(d1, (uint64_t) (100 + 200 + 300));
    assertEquals(d2, (uint64_t) (100 + 200 + 300));
}

typedef void (*TestCase)();

void runTest(const char* name, TestCase testCase) {
    try {
        std::cout << "Running test case " << name << " ... ";
        testCase();
        std::cout << "SUCCESS" << std::endl;
    } catch (std::runtime_error& err) {
        std::cout << "FAILED" << std::endl;
        throw err;
    }
}

#define RUN_TEST(F) runTest(#F, F)

int main(int argc, char* argv[]) {
    RUN_TEST(testDiff);
    RUN_TEST(testSampleBilinearWrappedClampedMono);
    RUN_TEST(testShrinkAndAccumulate);
    //RUN_TEST(testEMoR);
    //RUN_TEST(testSummedAreaTable);
    //RUN_TEST(testBlerp);
    //RUN_TEST(testFastAtan2);
    //RUN_TEST(testFastAtan2Double);
    //RUN_TEST(testTransform);
    return 0;
}