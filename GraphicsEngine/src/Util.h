#pragma once
// for file reading
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

void ReadVertexData(const std::string& filepath, std::vector <float>& vert, std::vector<unsigned int>& ind);