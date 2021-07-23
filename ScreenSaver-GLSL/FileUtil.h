#pragma once

#include <vector>
#include <iostream>
#include <fstream>

class FileUtil
{

public:
	
	static std::vector<char> readFile(const std::string& filename);
	static std::string root;

};

