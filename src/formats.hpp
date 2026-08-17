#pragma once
#include <iosfwd>
#include <string>
namespace ec2 {
std::wstring normalizeFormat(std::wstring value);
bool isSupportedInputFormat(const std::wstring& value);
bool isSupportedOutputFormat(const std::wstring& value);
void printFormats(std::wostream& output);
}
