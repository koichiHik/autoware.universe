//  Copyright 2021 Tier IV, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "raw_vehicle_cmd_converter/csv_loader.hpp"

#include <string>
#include <vector>

namespace raw_vehicle_cmd_converter
{
CSVLoader::CSVLoader(const std::string & csv_path) { csv_path_ = csv_path; }

bool CSVLoader::readCSV(Table & result, const char delim)
{
  std::ifstream ifs(csv_path_);
  if (!ifs.is_open()) {
    std::cerr << "Cannot open " << csv_path_.c_str() << std::endl;
    return false;
  }

  std::string buf;
  while (std::getline(ifs, buf)) {
    std::vector<std::string> tokens;

    std::istringstream iss(buf);
    std::string token;
    while (std::getline(iss, token, delim)) {
      tokens.push_back(token);
    }

    if (tokens.size() != 0) {
      result.push_back(tokens);
    }
  }
  if (!validateData(result, csv_path_)) {
    return false;
  }
  return true;
}

bool CSVLoader::validateData(const Table & table, const std::string & csv_path)
{
  if (table[0].size() < 2) {
    std::cerr << "Cannot read " << csv_path.c_str() << " CSV file should have at least 2 column"
              << std::endl;
    return false;
  }
  for (size_t i = 1; i < table.size(); i++) {
    if (table[0].size() != table[i].size()) {
      std::cerr << "Cannot read " << csv_path.c_str()
                << ". Each row should have a same number of columns" << std::endl;
      return false;
    }
  }
  return true;
}

Map CSVLoader::getMap(const Table & table)
{
  Map map = {};
  for (size_t i = 1; i < table.size(); i++) {
    std::vector<double> accelerations;
    for (size_t j = 1; j < table[i].size(); j++) {
      accelerations.push_back(std::stod(table[i][j]));
    }
    map.push_back(accelerations);
  }
  return map;
}

std::vector<double> CSVLoader::getRowIndex(const Table & table)
{
  std::vector<double> index = {};
  for (size_t i = 1; i < table[0].size(); i++) {
    index.push_back(std::stod(table[0][i]));
  }
  return index;
}

std::vector<double> CSVLoader::getColumnIndex(const Table & table)
{
  std::vector<double> index = {};
  for (size_t i = 1; i < table.size(); i++) {
    index.push_back(std::stod(table[i][0]));
  }
  return index;
}

double CSVLoader::clampValue(
  const double val, const std::vector<double> & ranges, const std::string & name)
{
  const double max_value = ranges.back();
  const double min_value = ranges.front();
  if (val < min_value || max_value < val) {
    std::cerr << "Input" << name << ": " << val << " is out off range. use closest value."
              << std::endl;
    return std::min(std::max(val, min_value), max_value);
  }
  return val;
}

}  // namespace raw_vehicle_cmd_converter