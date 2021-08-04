#pragma once

#include <vector>
#include <iostream>
#include <fstream>

class FileUtil
{

	inline static std::string root = "";

public:

	static void setRoot(std::string);
	static std::vector<char> readFile(const std::string& filename);
	

};

