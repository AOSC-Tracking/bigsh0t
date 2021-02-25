#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "../../main/cpp/Math.hpp"
#include "../../main/cpp/Matrix.hpp"
#include "../../main/cpp/MP4.hpp"


int main(int argc, char* argv[]) {
    MP4Parser parser(std::string("c:\\temp\\test.mp4"));
    std::vector<Quaternion> qs;
    
    parser.readRDT5(qs);
    
    parser.close();
    std::cout << qs.size() << std::endl;
    
    return 0;
}