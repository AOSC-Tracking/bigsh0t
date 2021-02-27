/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MP4_HPP
#define MP4_HPP

#include <string>
#include <vector>
#include <fstream>

#include "Matrix.hpp"

const uint32_t TYPE_UUID = 0x75756964;
const uint32_t TYPE_MOOV = 0x6d6f6f76;
const uint32_t TYPE_MVHD = 0x6d766864;
const uint32_t TYPE_UDTA = 0x75647461;
const uint32_t TYPE_RDTD = 0x52445444;
const uint32_t TYPE_RDTG = 0x52445447;
const uint32_t TYPE_RDTH = 0x52445448;

const uint32_t TYPE_RDT1 = 0x52445431;
const uint32_t TYPE_RDT2 = 0x52445432;
const uint32_t TYPE_RDT3 = 0x52445433;
const uint32_t TYPE_RDT4 = 0x52445434;
const uint32_t TYPE_RDT5 = 0x52445435;
const uint32_t TYPE_RDT6 = 0x52445436;
const uint32_t TYPE_RDT7 = 0x52445437;
const uint32_t TYPE_RDT8 = 0x52445438;


class MP4Atom {
  public:
    uint64_t pos;
    uint64_t size;
    uint32_t name;
    uint8_t usertype[16];
    uint64_t headerSize;
    bool valid;

    std::string nameString() {
        char buf[5];
        buf[0] = static_cast<char>((name >> 24) & 0xff);
        buf[1] = static_cast<char>((name >> 16) & 0xff);
        buf[2] = static_cast<char>((name >>  8) & 0xff);
        buf[3] = static_cast<char>((name      ) & 0xff);
        buf[4] = 0;
        return std::string(buf);
    }
};

class MP4Parser {
  public:
    MP4Parser(const std::string& filename);

    uint8_t readUInt8();
    uint32_t readUInt32();
    uint32_t readUInt32LE();
    uint16_t readUInt16LE();
    int16_t readInt16();
    float readFloat32LE();
    uint64_t readUInt64();
    void close();
    MP4Atom readAtom();
    void skip(const MP4Atom& hdr);
    std::vector<MP4Atom> list(const MP4Atom* atom);
    MP4Atom find(const MP4Atom* atom, uint32_t name);
    void seek(const MP4Atom& atom);
    bool valid();
    float getDuration();
    bool readRDTH(std::vector<Quaternion>& zenithData);
    bool readRDT5(std::vector<Quaternion>& zenithData);
    void readZenithData(std::vector<Quaternion>& zenithData);

  private:
    std::ifstream file;
};


#endif
