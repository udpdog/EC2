#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace ec2 {

bool findExecutable(const std::vector<std::string>& names,
                    std::filesystem::path& executable);

unsigned long runProcess(const std::filesystem::path& executable,
                         const std::vector<std::filesystem::path>& arguments);

} // namespace ec2
