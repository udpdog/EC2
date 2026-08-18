#include "imagemagick.hpp"
#include "formats.hpp"
#include <windows.h>
#include <vector>
namespace fs=std::filesystem;
namespace ec2 {
namespace {
std::wstring quote(const std::wstring& v) {
	std::wstring r=L"\"";
	size_t n=0;
	for(wchar_t c:v) {
		if(c==L'\\')++n;
		else if(c==L'\"') {
			r.append(n*2+1,L'\\');
			r+=c;
			n=0;
		}
		else {
			r.append(n,L'\\');
			n=0;
			r+=c;
		}
	}
	r.append(n*2,L'\\');
	return r+L'\"';
}
bool search(const wchar_t* name,std::wstring& result) {
	DWORD n=SearchPathW(nullptr,name,nullptr,0,nullptr,nullptr);
	if(!n)return false;
	std::vector<wchar_t>b(n+1);
	DWORD w=SearchPathW(nullptr,name,nullptr,(DWORD)b.size(),b.data(),nullptr);
	if(!w||w>=b.size())return false;
	result.assign(b.data(),w);
	return true;
}
DWORD run(const std::wstring& exe,const std::wstring& args) {
	std::wstring cmd=quote(exe)+L" "+args;
	std::vector<wchar_t>b(cmd.begin(),cmd.end());
	b.push_back(0);
	STARTUPINFOW si{};
	si.cb=sizeof(si);
	PROCESS_INFORMATION pi{};
	if(!CreateProcessW(exe.c_str(),b.data(),nullptr,nullptr,TRUE,0,nullptr,nullptr,&si,&pi))return GetLastError();
	CloseHandle(pi.hThread);
	WaitForSingleObject(pi.hProcess,INFINITE);
	DWORD code=1;
	GetExitCodeProcess(pi.hProcess,&code);
	CloseHandle(pi.hProcess);
	return code;
}
}
bool findImageMagick(std::wstring& exe) {
	if(search(L"magick.exe",exe))return true;
	for(const wchar_t* var: {
	            L"ProgramFiles",L"ProgramW6432"
	        }) {
		DWORD n=GetEnvironmentVariableW(var,nullptr,0);
		if(!n)continue;
		std::vector<wchar_t>b(n);
		if(!GetEnvironmentVariableW(var,b.data(),n))continue;
		std::error_code e;
		for(const auto& x:fs::directory_iterator(fs::path(b.data()),e)) {
			if(e||!x.is_directory(e))continue;
			if(normalizeFormat(x.path().filename().wstring()).rfind(L"imagemagick",0))continue;
			fs::path p=x.path()/L"magick.exe";
			if(fs::is_regular_file(p,e)) {
				exe=p.wstring();
				return true;
			}
		}
	}
	return false;
}
bool installImageMagick() {
	std::wstring w;
	if(!search(L"winget.exe",w))return false;
	return run(w,L"install --id ImageMagick.ImageMagick --exact --accept-package-agreements --accept-source-agreements")==0;
}
unsigned long convertImage(const std::wstring& exe,const fs::path& in,const fs::path& out,const std::wstring& fmt,const ConversionOptions& options) {
	std::wstring args=quote(in.wstring());
	if(options.enabled&&options.autoOrient)args+=L" -auto-orient";
	if(options.enabled&&(options.resize.width||options.resize.height)) {
		std::wstring geometry=(options.resize.width?std::to_wstring(options.resize.width):L"")+L"x"+(options.resize.height?std::to_wstring(options.resize.height):L"");
		if(options.resize.fit==FitMode::Max)args+=L" -resize "+quote(geometry+L">");
		else if(options.resize.fit==FitMode::Crop)args+=L" -resize "+quote(geometry+L"^")+L" -gravity center -extent "+quote(geometry);
		else args+=L" -resize "+quote(geometry+L"!");
	}
	if(options.enabled&&options.stripMetadata)args+=L" -strip";
	if(options.enabled&&options.quality)args+=L" -quality "+std::to_wstring(options.quality);
	return run(exe,args+L" "+quote(normalizeFormat(fmt)+L":"+out.wstring()));
}
}
