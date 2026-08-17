#pragma once
#include <iosfwd>
#include <string>
namespace ec2 {
std::wstring normalizeFormat(std::wstring value);
bool isSupportedFormat(const std::wstring& value);
void printFormats(std::wostream& output);
}
