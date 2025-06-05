#include "ReadJson.h"

std::wstring utf8_to_wstring(const std::string& str){
	if (str.empty()) return L"";

	// 计算需要的宽字符缓冲区大小
	int size_needed = MultiByteToWideChar(
		CP_UTF8, 0,
		str.c_str(), (int)str.size(),
		NULL, 0
	);

	if (size_needed <= 0) {
		return L"转换失败";
	}

	// 分配缓冲区
	std::wstring result(size_needed, 0);

	// 执行转换
	MultiByteToWideChar(
		CP_UTF8, 0,
		str.c_str(), (int)str.size(),
		&result[0], size_needed
	);
	return result;
}