#pragma once
#include <string>
#include <optional>

struct Employee {
    int id;
    std::string institute;
    std::string department;
    std::string lastName;
    std::string initials;
    std::string position;

    std::string phone;
    std::string email;
    std::string employmentDate;

    std::string status;

    std::optional<int> computerId;
};
