#include "FileUtil.h"

void FileUtil::setRoot(std::string path) {
	if (path.back() != '/') { root = path + '/'; }
	else { root = path; }
}

std::vector<char> FileUtil::readFile(const std::string& filepath) {
	std::ifstream file(root + filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file: " + root + filepath);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
