#include "imagemagick.hpp"

#include "formats.hpp"
#include "process.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace ec2 {
namespace {

fs::path textArgument(const std::wstring& value) {
#ifdef _WIN32
    return fs::path(value);
#else
    std::string ascii(value.begin(), value.end());
    return fs::path(ascii);
#endif
}

#ifdef _WIN32
bool findWindowsInstallation(fs::path& executable) {
    const wchar_t* roots[] = {L"ProgramFiles", L"ProgramW6432"};
    for (const wchar_t* variable : roots) {
        const DWORD size = GetEnvironmentVariableW(variable, nullptr, 0);
        if (!size) continue;
        std::vector<wchar_t> value(size);
        if (!GetEnvironmentVariableW(variable, value.data(), size)) continue;
        std::error_code error;
        for (const auto& entry : fs::directory_iterator(fs::path(value.data()), error)) {
            if (error || !entry.is_directory(error)) continue;
            if (normalizeFormat(entry.path().filename().wstring()).rfind(L"imagemagick", 0)) continue;
            const fs::path candidate = entry.path() / L"magick.exe";
            if (fs::is_regular_file(candidate, error)) {
                executable = candidate;
                return true;
            }
        }
    }
    return false;
}
#endif

} // namespace

bool findImageMagick(fs::path& executable) {
#ifdef _WIN32
    if (findExecutable({"magick.exe"}, executable)) return true;
    return findWindowsInstallation(executable);
#else
    // ImageMagick 7 fournit `magick`; de nombreuses distributions proposent
    // encore ImageMagick 6 sous le nom `convert`.
    return findExecutable({"magick", "convert"}, executable);
#endif
}

bool installImageMagick() {
    fs::path installer;
#ifdef _WIN32
    if (!findExecutable({"winget.exe"}, installer)) return false;
    return runProcess(installer, {
        L"install", L"--id", L"ImageMagick.ImageMagick", L"--exact",
        L"--accept-package-agreements", L"--accept-source-agreements"
    }) == 0;
#else
    fs::path sudo;
    const bool hasSudo = findExecutable({"sudo"}, sudo);
    const auto install = [&](const std::vector<fs::path>& command) {
        if (command.empty()) return false;
        if (hasSudo) {
            std::vector<fs::path> arguments{command.front()};
            arguments.insert(arguments.end(), command.begin() + 1, command.end());
            return runProcess(sudo, arguments) == 0;
        }
        std::vector<fs::path> arguments(command.begin() + 1, command.end());
        return runProcess(command.front(), arguments) == 0;
    };

    if (findExecutable({"apt-get"}, installer))
        return install({installer, "install", "-y", "imagemagick"});
    if (findExecutable({"dnf"}, installer))
        return install({installer, "install", "-y", "ImageMagick"});
    if (findExecutable({"yum"}, installer))
        return install({installer, "install", "-y", "ImageMagick"});
    if (findExecutable({"pacman"}, installer))
        return install({installer, "-S", "--noconfirm", "imagemagick"});
    if (findExecutable({"zypper"}, installer))
        return install({installer, "--non-interactive", "install", "ImageMagick"});
    if (findExecutable({"apk"}, installer))
        return install({installer, "add", "imagemagick"});
    return false;
#endif
}

std::wstring imageMagickInstallHint() {
#ifdef _WIN32
    return L"Verifiez que winget est installe.";
#else
    return L"Installez ImageMagick avec le gestionnaire de paquets de votre distribution.";
#endif
}

unsigned long convertImage(const fs::path& executable,
                           const fs::path& input,
                           const fs::path& output,
                           const std::wstring& format,
                           const ConversionOptions& options) {
    std::vector<fs::path> arguments{input};
    if (options.enabled && options.autoOrient) arguments.emplace_back("-auto-orient");

    if (options.enabled && (options.resize.width || options.resize.height)) {
        std::wstring geometry = (options.resize.width
            ? std::to_wstring(options.resize.width) : L"") + L"x"
            + (options.resize.height ? std::to_wstring(options.resize.height) : L"");
        arguments.emplace_back("-resize");
        if (options.resize.fit == FitMode::Max) {
            arguments.push_back(textArgument(geometry + L">"));
        } else if (options.resize.fit == FitMode::Crop) {
            arguments.push_back(textArgument(geometry + L"^"));
            arguments.emplace_back("-gravity");
            arguments.emplace_back("center");
            arguments.emplace_back("-extent");
            arguments.push_back(textArgument(geometry));
        } else {
            arguments.push_back(textArgument(geometry + L"!"));
        }
    }
    if (options.enabled && options.stripMetadata) arguments.emplace_back("-strip");
    if (options.enabled && options.quality) {
        arguments.emplace_back("-quality");
        arguments.push_back(textArgument(std::to_wstring(options.quality)));
    }

    fs::path forcedOutput;
#ifdef _WIN32
    forcedOutput = fs::path(normalizeFormat(format) + L":" + output.wstring());
#else
    const std::string outputFormat(format.begin(), format.end());
    forcedOutput = fs::path(outputFormat + ":" + output.string());
#endif
    arguments.push_back(forcedOutput);
    return runProcess(executable, arguments);
}

} // namespace ec2
