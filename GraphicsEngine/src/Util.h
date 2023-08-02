#pragma once
// for file reading
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include <glm/glm.hpp>

void ReadVertexData(const std::string& filepath, std::vector <float>& vert, std::vector<unsigned int>& ind);
glm::vec4 GetColorShade(float dot, glm::vec4 baseColor);
int CalculateNumberOfLines(float size, float distance);