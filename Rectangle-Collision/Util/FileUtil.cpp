#include "FileUtil.h"



std::vector<char> FileUtil::readFile(const std::string& filename) {
    std::ifstream file(root + filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + filename);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


std::string FileUtil::root = "D:/files/VulkanFiles_Summer2021/Rectangle-Collision/";