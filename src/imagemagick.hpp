#pragma once
#include <filesystem>
#include <string>
#include "conversion_options.hpp"
namespace ec2 {
bool findImageMagick(std::filesystem::path& executable);
bool installImageMagick();
unsigned long convertImage(const std::filesystem::path&,const std::filesystem::path&,const std::filesystem::path&,const std::wstring&,const ConversionOptions&);
std::wstring imageMagickInstallHint();
}
