#include "time_util.h"
#include <iostream>
#include <string>
#include <sstream>

std::string TimeUtil::timestampToString(std::time_t timestamp) {
	// 将时间戳转换为 tm 结构

	std::tm tm;
	localtime_s(&tm, &timestamp);

	const std::string format = "%Y-%m-%d %H:%M:%S";

	char buffer[32];  // 假设日期时间字符串不会超过32个字符
	std::strftime(buffer, sizeof(buffer), format.c_str(), &tm);

	// 返回格式化的字符串
	return std::string(buffer);
}

time_t TimeUtil::parseDateToTime(const std::string& dateStr) {
	std::tm tm = {};
	std::istringstream ss(dateStr);
	char slash;
	int day, month, year;
	ss >> month >> slash >> day >> slash >> year;  // m/d/Y 格式

    struct tm timeinfo = { 0,0,0,0,0,0,0,0,0 };
	timeinfo.tm_year = year + 2000-1900; // 年份为2021，需要减去1900
	timeinfo.tm_mon = month-1; // 月份为1月，需减去1
	timeinfo.tm_mday = day; // 日期为1号
	timeinfo.tm_hour = 0; // 小时为0
	timeinfo.tm_min = 0; // 分钟为0
	timeinfo.tm_sec = 0; // 秒为0

	time_t time = mktime(&timeinfo);
	
    return time;
}
