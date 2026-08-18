#include "conversion_options.hpp"

#include "formats.hpp"

#include <iostream>
#include <map>

namespace ec2 {
namespace {

struct FormatOptionsProfile {
    unsigned defaultWidth = 0;
    unsigned defaultHeight = 0;
    bool supportsQuality = false;
    unsigned defaultQuality = 0;
};

const std::map<std::wstring, FormatOptionsProfile> kFormatProfiles = {
    {L"avif", {}},
    {L"bmp", {}},
    {L"eps", {}},
    {L"gif", {}},
    {L"ico", {32, 32}},
    {L"jpg", {0, 0, true}},
    {L"pdf", {}},
    {L"png", {0, 0, true, 75}},
    {L"ps", {}}
};

bool readValue(int argc, wchar_t* argv[], int& index,
               const std::wstring& argument, std::wstring& value,
               std::wstring& error) {
	if (++index >= argc) {
		error = L"valeur manquante apres " + argument + L".";
		return false;
	}
	value = argv[index];
	return true;
}

bool parseDimension(const std::wstring& text, unsigned& value) {
	try {
		std::size_t used = 0;
		const unsigned long parsed = std::stoul(text, &used);
		if (used != text.size() || parsed == 0 || parsed > 100000) return false;
		value = static_cast<unsigned>(parsed);
		return true;
	} catch (...) {
		return false;
	}
}

bool parseBoolean(const std::wstring& text, bool& value) {
	const std::wstring normalized = normalizeFormat(text);
	if (normalized == L"yes" || normalized == L"true" || normalized == L"1") {
		value = true;
		return true;
	}
	if (normalized == L"no" || normalized == L"false" || normalized == L"0") {
		value = false;
		return true;
	}
	return false;
}

bool parseFitMode(const std::wstring& text, FitMode& mode) {
	const std::wstring normalized = normalizeFormat(text);
	if (normalized == L"max") mode = FitMode::Max;
	else if (normalized == L"crop") mode = FitMode::Crop;
	else if (normalized == L"scale") mode = FitMode::Scale;
	else return false;
	return true;
}

} // namespace

OptionParseResult parseConversionOption(const std::wstring& argument,
                                        int argc, wchar_t* argv[], int& index,
                                        ConversionOptions& options,
                                        std::wstring& error) {
	std::wstring value;
    if (argument == L"--width" || argument == L"--height" || argument == L"--quality") {
        if (!readValue(argc, argv, index, argument, value, error)) return OptionParseResult::Error;
        unsigned dimension = 0;
        if (!parseDimension(value, dimension)) {
            error = argument == L"--quality"
                ? L"--quality exige une valeur de 1 a 100."
                : L"dimension invalide : " + value + L".";
            return OptionParseResult::Error;
        }
        if (argument == L"--quality") {
            if (dimension > 100) {
                error = L"--quality exige une valeur de 1 a 100.";
                return OptionParseResult::Error;
            }
            options.quality = dimension;
        } else if (argument == L"--width") options.resize.width = dimension;
        else options.resize.height = dimension;
	} else if (argument == L"--fit") {
		if (!readValue(argc, argv, index, argument, value, error)) return OptionParseResult::Error;
		if (!parseFitMode(value, options.resize.fit)) {
			error = L"--fit accepte max, crop ou scale.";
			return OptionParseResult::Error;
		}
	} else if (argument == L"--strip" || argument == L"--auto-orient") {
		if (!readValue(argc, argv, index, argument, value, error)) return OptionParseResult::Error;
		bool enabled = false;
		if (!parseBoolean(value, enabled)) {
			error = argument + L" accepte yes ou no.";
			return OptionParseResult::Error;
		}
		if (argument == L"--strip") options.stripMetadata = enabled;
		else options.autoOrient = enabled;
	} else {
		return OptionParseResult::NotRecognized;
	}

	options.customized = true;
	return OptionParseResult::Parsed;
}

bool validateConversionOptions(const std::wstring& outputFormat,
                               ConversionOptions& options,
                               std::wstring& error) {
    const auto profile = kFormatProfiles.find(outputFormat);
    options.enabled = profile != kFormatProfiles.end();
    if (options.customized && !options.enabled) {
		error = L"les options avancees ne sont pas disponibles pour le format "
		        + outputFormat + L".";
        return false;
    }
    if (options.quality && options.enabled && !profile->second.supportsQuality) {
        error = L"--quality n'est pas disponible pour le format " + outputFormat + L".";
        return false;
    }
    if (options.enabled) {
        if (!options.resize.width) options.resize.width = profile->second.defaultWidth;
        if (!options.resize.height) options.resize.height = profile->second.defaultHeight;
        if (!options.quality) options.quality = profile->second.defaultQuality;
    }
	if ((options.resize.fit == FitMode::Crop || options.resize.fit == FitMode::Scale)
	        && (!options.resize.width || !options.resize.height)) {
		error = L"--fit crop et scale exigent --width et --height.";
		return false;
	}
	return true;
}

std::wstring fitModeName(FitMode mode) {
	switch (mode) {
	case FitMode::Max:
		return L"max";
	case FitMode::Crop:
		return L"crop";
	case FitMode::Scale:
		return L"scale";
	}
	return L"inconnu";
}

void printConversionOptions(const ConversionOptions& options, std::wostream& output) {
	if (!options.enabled) return;
	output << L"[EC2] Options : largeur="
	       << (options.resize.width ? std::to_wstring(options.resize.width) : L"auto")
	       << L", hauteur="
	       << (options.resize.height ? std::to_wstring(options.resize.height) : L"auto")
	       << L", fit=" << fitModeName(options.resize.fit)
           << L", strip=" << (options.stripMetadata ? L"yes" : L"no")
           << L", auto-orient=" << (options.autoOrient ? L"yes" : L"no");
    if (options.quality) output << L", quality=" << options.quality;
    output << L'\n';
}

bool printConversionOptionsHelp(const std::wstring& format, std::wostream& output) {
    const std::wstring normalized = normalizeFormat(format);
    const auto profile = kFormatProfiles.find(normalized);
    if (profile == kFormatProfiles.end()) return false;

    output << L"Options avancees pour " << normalized << L" :\n"
           << L"  --width N           Largeur en pixels";
    if (profile->second.defaultWidth) output << L" (defaut : " << profile->second.defaultWidth << L")";
    output << L"\n  --height N          Hauteur en pixels";
    if (profile->second.defaultHeight) output << L" (defaut : " << profile->second.defaultHeight << L")";
    output << L"\n"
           << L"  --fit MODE          max, crop ou scale (defaut : max)\n"
           << L"  --strip BOOL        yes ou no (defaut : no)\n"
           << L"  --auto-orient BOOL  yes ou no (defaut : yes)\n";
    if (profile->second.supportsQuality) {
        output << L"  --quality N         Qualite de 1 a 100 (defaut : ";
        if (profile->second.defaultQuality) output << profile->second.defaultQuality;
        else output << L"automatique";
        output << L")\n";
    }
    output << L"\n"
	       << L"Exemple :\n"
           << L"  ec2.exe image.jpg -format " << normalized;
    if (profile->second.defaultWidth && profile->second.defaultHeight) {
        output << L" --width " << profile->second.defaultWidth
               << L" --height " << profile->second.defaultHeight;
    } else {
        output << L" --width 1920 --height 1080";
    }
    output << L" --fit max\n";
	return true;
}

} // namespace ec2
