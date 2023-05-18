#include "Util.h"
#include <iostream>
#include <vector>


void StringToFloat(const std::vector<std::vector <char>>& stream, std::vector <float>& vert)
{
	std::vector<float> res_vec;
	for (int i = 0; i < stream.size(); i++)
	{
		int len = stream[i].size();
		int p_idx = 0;
		float result = 0.0f;
		bool negative = false;
		if (stream[i][0] == 45)
		{
			negative = true;
		}

		while (stream[i][p_idx] != 46)			// dot
		{
			p_idx++;
		}


		for (int j = 0 + negative; j < p_idx; j++)
		{
			int digit = stream[i][j] - 48;
			result += digit* pow(10, p_idx - (1 + j));
		}

		for (int j = p_idx + 1; j < len; j++)
		{
			int digit = stream[i][j] - 48;
			float tmp = digit * pow(10, p_idx - j);
			result += digit * pow(10, p_idx - j);
		}

		if (negative)
			result *= -1.0f;

		res_vec.push_back(result);
	}
	vert = res_vec;
}

void StringToInt(const std::vector<std::vector <char>>& stream, std::vector <unsigned int>& ind)
{
	std::vector<unsigned int> res_vec;
	for (int i = 0; i < stream.size(); i++)
	{
		int len = stream[i].size();
		unsigned int  result = 0;

		for (int j = 0; j < len; j++)
		{
			int digit = stream[i][j] - 48;
			result += digit * pow(10, len - (1 + j));
		}

		res_vec.push_back(result);
	}

	ind = res_vec;
}

void ReadVertexData(const std::string& filepath, std::vector <float>&vert, std::vector<unsigned int>& ind)
{
	std::string line;
	std::ifstream stream(filepath);
	std::vector<std::vector <char>> vertex_vectors;
	std::vector<std::vector <char>> index_vectors;

	while (getline(stream, line))
	{
		if (line.find("v") != std::string::npos)
		{
			for (int i = 1; i < line.size(); i++)
			{
				if ((line[i] >= 48 && line[i] <= 57) || line[i] == 45)		// numbers from 0 to 9 or minus
				{
					std::vector<char> number_char;
					
					while (line[i] != 32 )				// space
					{
						number_char.push_back(line[i]);
						if (i + 1 < line.size())
							i++;
						else
							break;
					}
					vertex_vectors.push_back(number_char);
				}
			}
		}
		else if (line.find("f") != std::string::npos)
		{
			for (int i = 1; i < line.size(); i++)
			{
				if ((line[i] >= 48 && line[i] <= 57) || line[i] == 45)		// numbers from 0 to 9 or minus
				{
					std::vector<char> number_char;

					while (line[i] != 32)				// space
					{
						number_char.push_back(line[i]);
						if (i + 1 < line.size())
							i++;
						else
							break;
					}
					index_vectors.push_back(number_char);
				}
			}
		
		}
		else
		{
		}
		
	}
	// TODO: Convert char array to float 
	StringToFloat(vertex_vectors, vert);
	StringToInt(index_vectors, ind);
}


