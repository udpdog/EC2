#pragma once
#include <filesystem>
#include <string>
namespace ec2 {
bool findImageMagick(std::wstring& executable);
bool installImageMagick();
unsigned long convertImage(const std::wstring&,const std::filesystem::path&,const std::filesystem::path&,const std::wstring&);
}
