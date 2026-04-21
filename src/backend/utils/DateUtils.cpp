#include "DateUtils.h"

#include <cctype>
#include <ctime>

namespace {
    struct DateParts {
        int year;
        int month;
        int day;
    };

    bool parseInt(const std::string& s, int& out) {
        if (s.empty())
            return false;
        for (char ch : s) {
            if (!std::isdigit(static_cast<unsigned char>(ch)))
                return false;
        }
        try {
            out = std::stoi(s);
            return true;
        }
        catch (...) {
            return false;
        }
    }

    bool isLeapYear(int year) {
        return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    }

    int daysInMonth(int year, int month) {
        static const int days[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        if (month == 2)
            return isLeapYear(year) ? 29 : 28;
        return days[month - 1];
    }

    bool isValidDate(const DateParts& d) {
        if (d.year < 1 || d.month < 1 || d.month > 12)
            return false;
        if (d.day < 1 || d.day > daysInMonth(d.year, d.month))
            return false;
        return true;
    }

    bool parseDate(const std::string& value, DateParts& out) {
        if (value.size() == 10 && value[4] == '-' && value[7] == '-') {
            int y, m, d;
            if (!parseInt(value.substr(0, 4), y) ||
                !parseInt(value.substr(5, 2), m) ||
                !parseInt(value.substr(8, 2), d))
                return false;

            out = { y, m, d };
            return true;
        }

        if (value.size() == 10 &&
            (value[2] == '.' || value[2] == '/') &&
            value[5] == value[2]) {
            int d, m, y;
            if (!parseInt(value.substr(0, 2), d) ||
                !parseInt(value.substr(3, 2), m) ||
                !parseInt(value.substr(6, 4), y))
                return false;

            out = { y, m, d };
            return true;
        }

        return false;
    }

    DateParts todayDate() {
        std::time_t t = std::time(nullptr);
        std::tm local{};
        localtime_s(&local, &t);
        return { local.tm_year + 1900, local.tm_mon + 1, local.tm_mday };
    }

    bool isFutureDate(const DateParts& d) {
        DateParts today = todayDate();
        if (d.year != today.year)
            return d.year > today.year;
        if (d.month != today.month)
            return d.month > today.month;
        return d.day > today.day;
    }
}

namespace date_utils {

void validateDateField(const std::string& value,
                       const std::string& fieldLabel,
                       std::vector<std::string>& errors,
                       bool allowFuture) {
    if (value.empty())
        return;

    DateParts parsed{};
    if (!parseDate(value, parsed) || !isValidDate(parsed)) {
        errors.push_back("Некорректная дата " + fieldLabel + " (значение: " + value + ")");
        return;
    }

    if (!allowFuture && isFutureDate(parsed)) {
        errors.push_back("Дата в будущем " + fieldLabel + " (значение: " + value + ")");
    }
}

}
