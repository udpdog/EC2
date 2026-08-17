#pragma once
#include <filesystem>
#include <string>
namespace ec2 {
enum class Action { Convert, Help, Formats, Error };
struct Options { Action action=Action::Error; std::filesystem::path input,output; std::wstring format,error; bool force=false; };
Options parseArguments(int argc,wchar_t* argv[]);
void printHelp();
}
