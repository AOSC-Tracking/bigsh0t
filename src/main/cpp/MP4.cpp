/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <cstdint>
#include <limits>
#include <cmath>
#include <ios>

#include "Math.hpp"
#include "MP4.hpp"

MP4Parser::MP4Parser(const std::string& filename) : file(filename, std::ios_base::in | std::ios_base::binary) {
}

void MP4Parser::close() {
    file.close();
}

uint32_t MP4Parser::readUInt32() {
    uint32_t res = 0;
    uint8_t buf[4];
    file.read(reinterpret_cast<char*>(buf), 4);
    for (int i = 0; i < 4; ++i) {
        res <<= 8;
        res |= buf[i];
    }
    return res;
}

int16_t MP4Parser::readInt16() {
    int16_t res = 0;
    uint8_t buf[2];
    file.read(reinterpret_cast<char*>(buf), 2);
    for (int i = 0; i < 2; ++i) {
        res <<= 8;
        res |= buf[i];
    }
    return res;
}


uint8_t MP4Parser::readUInt8() {
    uint8_t buf[1];
    file.read(reinterpret_cast<char*>(buf), 1);
    return buf[0];
}


uint32_t MP4Parser::readUInt32LE() {
    uint32_t res = 0;
    uint8_t buf[4];
    file.read(reinterpret_cast<char*>(buf), 4);
    for (int i = 0; i < 4; ++i) {
        res |= buf[i] << (i * 8);
    }
    return res;
}

uint16_t MP4Parser::readUInt16LE() {
    uint16_t res = 0;
    uint8_t buf[2];
    file.read(reinterpret_cast<char*>(buf), 2);
    for (int i = 0; i < 2; ++i) {
        res |= buf[i] << (i * 8);
    }
    return res;
}

uint64_t MP4Parser::readUInt64() {
    uint64_t res = 0;
    uint8_t buf[8];
    file.read(reinterpret_cast<char*>(buf), 8);
    for (int i = 0; i < 8; ++i) {
        res <<= 8;
        res |= buf[i];
    }
    return res;
}

float MP4Parser::readFloat32LE() {
    union {
        float f;  // assuming 32-bit IEEE 754 single-precision
        uint32_t i;    // assuming 32-bit 2's complement int
    } u;

    u.i = readUInt32LE();
    return u.f;
}

bool MP4Parser::valid() {
    return !file.fail();
}

MP4Atom MP4Parser::readAtom() {
    MP4Atom hdr;
    if (file.fail()) {
        return hdr;
    }
    hdr.valid = true;
    hdr.pos = file.tellg();
    hdr.headerSize = 0;

    hdr.size = readUInt32();
    hdr.headerSize += 4;

    hdr.name = readUInt32();
    hdr.headerSize += 4;

    if (hdr.size == 1) {
        hdr.size = readUInt64();
        hdr.headerSize += 8;
    }
    if (hdr.name == TYPE_UUID) {
        file.read(reinterpret_cast<char*>(hdr.usertype), 16);
        hdr.headerSize += 16;
    }

    if (file.fail()) {
        hdr.valid = false;
    }
    return hdr;

}

void MP4Parser::skip(const MP4Atom& atom) {
    file.seekg(atom.pos + atom.size);
}

std::vector<MP4Atom> MP4Parser::list(const MP4Atom* atom) {
    std::vector<MP4Atom> res;
    file.seekg(0, std::ios_base::end);
    std::streampos maxPos = file.tellg();
    if (atom != nullptr) {
        maxPos = atom->pos + atom->size;
        seek(*atom);
    } else {
        file.seekg(0);
    }
    while (file.tellg() < maxPos) {
        MP4Atom a = readAtom();
        res.push_back(a);
        skip(a);
    }
    return res;
}

MP4Atom MP4Parser::find(const MP4Atom* atom, uint32_t name) {
    std::vector<MP4Atom> atoms = list(atom);
    for (MP4Atom& a : atoms) {
        if (a.name == name) {
            return a;
        }
    }
    MP4Atom res;
    res.valid = false;
    return res;
}

void MP4Parser::seek(const MP4Atom& atom) {
    file.seekg(atom.pos + atom.headerSize);
}

float MP4Parser::getDuration() {
    auto moov = find(nullptr, TYPE_MOOV);
    if (moov.valid) {
        auto mvhd = find(&moov, TYPE_MVHD);
        if (mvhd.valid) {
            seek(mvhd);
            uint32_t versionAndFlags = readUInt32();
            uint32_t createdAt = readUInt32();
            uint32_t modifiedAt = readUInt32();
            uint32_t timescale = readUInt32();
            uint32_t duration = readUInt32();
            return ((float) duration) / timescale;
        }
    }
    return -1;
}

bool MP4Parser::readRDTH(std::vector<Quaternion>& zenithData) {
    auto moov = find(nullptr, TYPE_MOOV);
    if (moov.valid) {
        auto udta = find(&moov, TYPE_UDTA);
        if (udta.valid) {
            auto rdth = find(&udta, TYPE_RDTH);
            if (rdth.valid) {
                seek(rdth);
                uint32_t frames = readUInt32LE();
                uint16_t unknown1 = readUInt16LE();
                uint16_t unknown2 = readUInt16LE();
                for (uint32_t frame = 0; frame < frames; ++frame) {
                    uint32_t a = readUInt32LE(); // Some kind of timer
                    uint32_t b = readUInt32LE(); // Always zero

                    Quaternion q;
                    q[0] = readFloat32LE(); // First is rotation amount
                    q[2] = readFloat32LE(); // Second is pitch
                    q[1] = readFloat32LE(); // third is roll
                    q[3] = -readFloat32LE(); // fourth is yaw

                    zenithData.push_back(q);
                }
                return true;
            }
        }
    }
    return false;
}

bool MP4Parser::readRDT5(std::vector<Quaternion>& zenithData) {
    auto moov = find(nullptr, TYPE_MOOV);
    if (moov.valid) {
        auto udta = find(&moov, TYPE_UDTA);
        if (udta.valid) {
            auto rdt5 = find(&udta, TYPE_RDT5);
            if (rdt5.valid) {
                seek(rdt5);
                // chunk size 112664
                // frames-times-two = 9384
                // 56 bytes leftover. 8 bytes MP4 Atom. 48 bytes more
                uint32_t frames = readUInt32();

                uint32_t unknown1 = readUInt32();
                uint32_t unknown2 = readUInt32();
                uint32_t unknown3 = readUInt32();
                uint32_t unknown4 = readUInt32();
                uint32_t unknown5 = readUInt32();

                std::vector<double> gx;
                std::vector<double> gy;
                std::vector<double> gz;

                for (uint32_t frame = 0; frame < frames; ++frame) {
                    int16_t a = readInt16(); // Gravity (-left right+)
                    int16_t b = readInt16(); // Gravity (-down up+)
                    int16_t c = readInt16(); // Gravity (-back front+)
                    int16_t d = readInt16();
                    int16_t e = readInt16();
                    int16_t f = readInt16();

                    Vector3 g;
                    g[0] = a / 16384.0;
                    g[1] = b / 16384.0;
                    g[2] = c / 16384.0;

                    if (g.norm2() < 0.1) {
                        g[0] = 0;
                        g[1] = -1.0;
                        g[2] = 0;
                    }
                    g.normalize();

                    gx.push_back(g[0]);
                    gy.push_back(g[1]);
                    gz.push_back(g[2]);
                }

                smooth (gx, 16, 0);
                smooth (gy, 16, 0);
                smooth (gz, 16, 0);

                for (int i = 0; i < gx.size(); ++i) {
                    Vector3 g;
                    g[0] = gz[i]; // back front (roll)
                    g[1] = gx[i]; // left right (pitch)
                    g[2] = gy[i]; // down up (yaw)

                    g.normalize();

                    // std::cout << g << std::endl;

                    Vector3 r;
                    r[0] = -g[1];
                    r[1] = g[0];
                    r[2] = 0.0;
                    if (!r.normalize()) {
                        // If |r| is too small, then we're almost precisely
                        // upside down. Use any rotation axis.
                        r[0] = 1.0;
                        r[1] = 0.0;
                        r[2] = 0.0;
                    }

                    double a = acos(-g[2]);

                    Quaternion cancelation;
                    cancelation.setQuaternionRotation(a, r[0], r[1], r[2]);

                    Matrix3 cancelationM;
                    cancelationM.identity();
                    rotateQuaternion(cancelationM, cancelation);

                    Vector3 canceled;
                    mulM3V3(cancelationM, g, canceled);

                    //std::cout << canceled << std::endl;

                    Vector3 ahead;
                    ahead[0] = 1.0;
                    ahead[1] = 0.0;
                    ahead[2] = 0.0;
                    Vector3 twisted;
                    mulM3V3(cancelationM, ahead, twisted);
                    //std::cout << twisted << std::endl;


                    Quaternion untwist;
                    untwist[0] = 1.0;
                    untwist[1] = 0.0;
                    untwist[2] = 0.0;
                    untwist[3] = 0.0;

                    // Attempt to face forward
                    // if current "forward" is straight down or up, then don't bother.
                    if (twisted[2] > (-1.0 + 0.000001) && twisted[2] < (1.0 - 0.000001)) {
                        double untwistA = atan2(twisted[1], twisted[0]);
                        untwist.setQuaternionRotation(-untwistA, 0, 0, 1);
                    }

                    /*
                    Matrix3 untwistM;
                    untwistM.identity();
                    rotateQuaternion(untwistM, untwist);

                    Vector3 untwistedV;
                    mulM3V3(untwistM, twisted, untwistedV);
                    std::cout << untwistedV << std::endl;

                    std::cout << std::endl;*/

                    Quaternion combined;
                    mulQQ(cancelation, untwist, combined);

                    Quaternion invQ;
                    invertQ(combined, invQ);

                    zenithData.push_back(invQ);
                }

                return true;
            }
        }
    }
    return false;
}

void MP4Parser::readZenithData(std::vector<Quaternion>& zenithData) {
    if (readRDTH(zenithData)) {
        return;
    }
    if (readRDT5(zenithData)) {
        return;
    }
}
