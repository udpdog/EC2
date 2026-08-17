#pragma once
#include <filesystem>
#include <string>
#include "conversion_options.hpp"
namespace ec2 {
bool findImageMagick(std::wstring& executable);
bool installImageMagick();
unsigned long convertImage(const std::wstring&,const std::filesystem::path&,const std::filesystem::path&,const std::wstring&,const ConversionOptions&);
}
