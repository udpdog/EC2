#pragma once

#include <iosfwd>
#include <string>

namespace ec2 {

enum class FitMode { Max, Crop, Scale };

struct ResizeOptions {
    unsigned width = 0;
    unsigned height = 0;
    FitMode fit = FitMode::Max;
};

struct ConversionOptions {
    ResizeOptions resize;
    bool stripMetadata = false;
    bool autoOrient = true;
    bool customized = false;
    bool enabled = false;
};

enum class OptionParseResult { NotRecognized, Parsed, Error };

OptionParseResult parseConversionOption(const std::wstring& argument,
                                        int argc,
                                        wchar_t* argv[],
                                        int& index,
                                        ConversionOptions& options,
                                        std::wstring& error);

bool validateConversionOptions(const std::wstring& outputFormat,
                               ConversionOptions& options,
                               std::wstring& error);

std::wstring fitModeName(FitMode mode);
void printConversionOptions(const ConversionOptions& options, std::wostream& output);
bool printConversionOptionsHelp(const std::wstring& format, std::wostream& output);

} // namespace ec2
