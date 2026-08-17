#include "formats.hpp"
#include <algorithm>
#include <cwctype>
#include <ostream>
#include <set>
namespace ec2 {
namespace {
const std::set<std::wstring> formats = {
 L"3fr",L"arw",L"avif",L"bmp",L"cr2",L"cr3",L"crw",L"dcr",L"dng",L"eps",
 L"erf",L"gif",L"heic",L"heif",L"icns",L"ico",L"jfif",L"jpeg",L"jpg",L"mos",
 L"mrw",L"nef",L"odd",L"odg",L"orf",L"pef",L"png",L"ppm",L"ps",L"psb",L"psd",
 L"pub",L"raf",L"raw",L"rw2",L"tga",L"tif",L"tiff",L"webp",L"x3f",L"xcf",L"xps"};
}
std::wstring normalizeFormat(std::wstring value) {
 std::transform(value.begin(),value.end(),value.begin(),[](wchar_t c){return std::towlower(c);});
 if (!value.empty() && value.front()==L'.') value.erase(0,1);
 return value;
}
bool isSupportedFormat(const std::wstring& value){return formats.count(normalizeFormat(value))!=0;}
void printFormats(std::wostream& out){
 out<<L"Formats acceptes :\n"; int col=0;
 for(const auto& f:formats) out<<f<<(++col%9==0?L"\n":L"  ");
 if(col%9) out<<L'\n';
}
}
