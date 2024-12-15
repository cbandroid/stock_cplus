#include "file_util.h"
#include <iostream>

#include <string>
#include <iomanip>
#include <sstream>

std::string FileUtil::getFileName(const std::string& path) {
	// 查找最后一个斜杠的位置
	size_t pos = path.find_last_of("/\\");
	if (pos == std::string::npos) {
		return path; // 没有找到斜杠，说明path中没有目录，只是一个文件名
	}
	else {
		return path.substr(pos + 1); // 提取文件名
	}
}