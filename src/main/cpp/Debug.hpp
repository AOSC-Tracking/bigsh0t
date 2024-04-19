#ifndef DEBUG__HPP
#define DEBUG__HPP
#include <string>
#include <fstream>

void logInfo(const std::string& message);
void logInfoDouble(const std::string& message, double v);

template<typename T>
void logInfoValue(const std::string& message, T v) {
  std::ofstream outfile;

  outfile.open("c:\\temp\\bigsh0tlog.txt", std::ios_base::app); // append instead of overwrite
  outfile << message.c_str() << ":" << v << std::endl;
  outfile.close();
}

#endif