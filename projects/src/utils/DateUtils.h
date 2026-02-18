#pragma once

#include <string>
#include <vector>

namespace date_utils {

void validateDateField(const std::string& value,
                       const std::string& fieldLabel,
                       std::vector<std::string>& errors,
                       bool allowFuture = false);

}
