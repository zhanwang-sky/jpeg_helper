//
//  ppm_helper.cpp
//  jpeg_helper
//
//  Created by jichen on 2023/5/7.
//

#include <fstream>
#include <regex>
#include <string>
#include "ppm_helper.hpp"

int ppm_helper::write_ppm(const char* filename, int width, int height,
                          const unsigned char* buf) {
  std::ofstream ofs(filename, std::ofstream::binary | std::ofstream::trunc);
  if (!ofs) {
    return -1;
  }

  ofs << "P6\n" << width << " " << height << "\n255\n";
  if (!ofs) {
    return -2;
  }

  int realsz = width * height * 3;
  if (!ofs.write((const char*) buf, realsz)) {
    return -3;
  }

  return realsz;
}

int ppm_helper::read_ppm(const char* filename, int* width, int* height,
                         unsigned char* buf, int bufsz) {
  std::ifstream ifs(filename, std::ifstream::binary);
  if (!ifs) {
    return -1;
  }

  std::string line;
  std::smatch sm;
  std::regex exp("^(\\d+) (\\d+)$");

  if (!std::getline(ifs, line) || line != "P6") {
    return -2;
  }

  if (!std::getline(ifs, line) || !std::regex_match(line, sm, exp)) {
    return -3;
  }

  *width = std::stoi(sm[1].str());
  *height = std::stoi(sm[2].str());

  if (!std::getline(ifs, line) || line != "255") {
    return -4;
  }

  int realsz = *width * *height * 3;
  if (bufsz < realsz) {
    return -5;
  }

  if (!ifs.read((char*) buf, realsz)) {
    return -6;
  }

  return realsz;
}
