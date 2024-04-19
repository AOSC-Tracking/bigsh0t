#include <fstream>

void logInfo(const std::string& message) {
  std::ofstream outfile;

  outfile.open("c:\\temp\\bigsh0tlog.txt", std::ios_base::app); // append instead of overwrite
  outfile << message.c_str() << std::endl;
  outfile.close();
}
