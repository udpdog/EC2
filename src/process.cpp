#include "process.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#endif

#include <filesystem>
#include <vector>

#ifndef _WIN32
extern char** environ;
#endif

namespace fs = std::filesystem;

namespace ec2 {
namespace {

#ifdef _WIN32

std::wstring quoteWindowsArgument(const std::wstring& value) {
    std::wstring result = L"\"";
    std::size_t slashes = 0;
    for (const wchar_t character : value) {
        if (character == L'\\') {
            ++slashes;
        } else if (character == L'\"') {
            result.append(slashes * 2 + 1, L'\\');
            result += character;
            slashes = 0;
        } else {
            result.append(slashes, L'\\');
            slashes = 0;
            result += character;
        }
    }
    result.append(slashes * 2, L'\\');
    return result + L'\"';
}

bool searchWindowsPath(const std::string& name, fs::path& result) {
    const std::wstring wideName(name.begin(), name.end());
    const DWORD needed = SearchPathW(nullptr, wideName.c_str(), nullptr, 0, nullptr, nullptr);
    if (!needed) return false;
    std::vector<wchar_t> buffer(needed + 1);
    const DWORD written = SearchPathW(nullptr, wideName.c_str(), nullptr,
                                      static_cast<DWORD>(buffer.size()), buffer.data(), nullptr);
    if (!written || written >= buffer.size()) return false;
    result = fs::path(std::wstring(buffer.data(), written));
    return true;
}

#else

bool isExecutable(const fs::path& path) {
    return ::access(path.c_str(), X_OK) == 0;
}

bool searchPosixPath(const std::string& name, fs::path& result) {
    if (name.find('/') != std::string::npos) {
        const fs::path candidate(name);
        if (isExecutable(candidate)) {
            result = candidate;
            return true;
        }
        return false;
    }

    const char* pathValue = std::getenv("PATH");
    if (!pathValue) return false;
    std::string pathList(pathValue);
    std::size_t start = 0;
    while (start <= pathList.size()) {
        const std::size_t end = pathList.find(':', start);
        const std::string directory = pathList.substr(
            start, end == std::string::npos ? std::string::npos : end - start);
        const fs::path candidate = fs::path(directory.empty() ? "." : directory) / name;
        if (isExecutable(candidate)) {
            result = candidate;
            return true;
        }
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return false;
}

#endif

} // namespace

bool findExecutable(const std::vector<std::string>& names, fs::path& executable) {
    for (const std::string& name : names) {
#ifdef _WIN32
        if (searchWindowsPath(name, executable)) return true;
#else
        if (searchPosixPath(name, executable)) return true;
#endif
    }
    return false;
}

unsigned long runProcess(const fs::path& executable,
                         const std::vector<fs::path>& arguments) {
#ifdef _WIN32
    std::wstring command = quoteWindowsArgument(executable.wstring());
    for (const fs::path& argument : arguments) {
        command += L" " + quoteWindowsArgument(argument.wstring());
    }
    std::vector<wchar_t> mutableCommand(command.begin(), command.end());
    mutableCommand.push_back(L'\0');

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(executable.c_str(), mutableCommand.data(), nullptr, nullptr, TRUE,
                        0, nullptr, nullptr, &startup, &process)) {
        return GetLastError();
    }
    CloseHandle(process.hThread);
    WaitForSingleObject(process.hProcess, INFINITE);
    DWORD exitCode = 1;
    GetExitCodeProcess(process.hProcess, &exitCode);
    CloseHandle(process.hProcess);
    return exitCode;
#else
    std::vector<std::string> storage;
    storage.reserve(arguments.size() + 1);
    storage.push_back(executable.string());
    for (const fs::path& argument : arguments) storage.push_back(argument.string());

    std::vector<char*> argv;
    argv.reserve(storage.size() + 1);
    for (std::string& argument : storage) argv.push_back(argument.data());
    argv.push_back(nullptr);

    pid_t process = 0;
    const int spawnResult = posix_spawn(
        &process, executable.c_str(), nullptr, nullptr, argv.data(), environ);
    if (spawnResult != 0) return static_cast<unsigned long>(spawnResult);

    int status = 0;
    while (waitpid(process, &status, 0) == -1) {
        if (errno != EINTR) return static_cast<unsigned long>(errno);
    }
    if (WIFEXITED(status)) return static_cast<unsigned long>(WEXITSTATUS(status));
    if (WIFSIGNALED(status)) return static_cast<unsigned long>(128 + WTERMSIG(status));
    return 1;
#endif
}

} // namespace ec2
