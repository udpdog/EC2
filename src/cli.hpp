#pragma once
#include <filesystem>
#include <string>
#include "conversion_options.hpp"
namespace ec2 {
enum class Action { Convert, Help, FormatHelp, Formats, Error };
struct Options {
 Action action=Action::Error;
 std::filesystem::path input,output;
 std::wstring format,error,helpFormat;
 ConversionOptions conversion;
 bool force=false;
};
Options parseArguments(int argc,wchar_t* argv[]);
void printHelp();
}
