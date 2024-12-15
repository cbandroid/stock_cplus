#pragma once
#include <string>
#include <ctime>


class TimeUtil {
public:
	static std::string timestampToString(std::time_t timestamp);
    static time_t parseDateToTime(const std::string& dateStr);
};
