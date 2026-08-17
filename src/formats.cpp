#include "formats.hpp"
#include <algorithm>
#include <cwctype>
#include <ostream>
#include <set>
namespace ec2 {
namespace {
const std::set<std::wstring> inputFormats = {
 L"3fr",L"arw",L"avif",L"bmp",L"cr2",L"cr3",L"crw",L"dcr",L"dng",L"eps",
 L"erf",L"gif",L"heic",L"heif",L"icns",L"ico",L"jfif",L"jpeg",L"jpg",L"mos",
 L"mrw",L"nef",L"odd",L"odg",L"orf",L"pef",L"png",L"ppm",L"ps",L"psb",L"psd",
 L"pub",L"raf",L"raw",L"rw2",L"tga",L"tif",L"tiff",L"webp",L"x3f",L"xcf",L"xps"};

const std::set<std::wstring> outputFormats = {
 L"avif",L"bmp",L"eps",L"gif",L"ico",L"jpg",L"odd",L"pdf",L"png",L"ps",L"psd",L"tiff",L"webp"};
}
std::wstring normalizeFormat(std::wstring value) {
 std::transform(value.begin(),value.end(),value.begin(),[](wchar_t c){return std::towlower(c);});
 if (!value.empty() && value.front()==L'.') value.erase(0,1);
 return value;
}
bool isSupportedInputFormat(const std::wstring& value){return inputFormats.count(normalizeFormat(value))!=0;}
bool isSupportedOutputFormat(const std::wstring& value){return outputFormats.count(normalizeFormat(value))!=0;}
void printFormats(std::wostream& out){
 out<<L"Formats de sortie acceptes :\n"; int col=0;
 for(const auto& f:outputFormats) out<<f<<(++col%6==0?L"\n":L"  ");
 if(col%6) out<<L'\n';
}
}
