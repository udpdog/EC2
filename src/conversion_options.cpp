#include "conversion_options.hpp"

#include "formats.hpp"

#include <iostream>
#include <set>

namespace ec2 {
namespace {

const std::set<std::wstring> kFormatsWithAdvancedOptions = {
    L"avif", L"bmp", L"eps", L"gif", L"pdf"
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
	if (argument == L"--width" || argument == L"--height") {
		if (!readValue(argc, argv, index, argument, value, error)) return OptionParseResult::Error;
		unsigned dimension = 0;
		if (!parseDimension(value, dimension)) {
			error = L"dimension invalide : " + value + L".";
			return OptionParseResult::Error;
		}
		if (argument == L"--width") options.resize.width = dimension;
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
	options.enabled = kFormatsWithAdvancedOptions.count(outputFormat) != 0;
	if (options.customized && !options.enabled) {
		error = L"les options avancees ne sont pas disponibles pour le format "
		        + outputFormat + L".";
		return false;
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
	       << L", auto-orient=" << (options.autoOrient ? L"yes" : L"no") << L'\n';
}

bool printConversionOptionsHelp(const std::wstring& format, std::wostream& output) {
	if (kFormatsWithAdvancedOptions.count(normalizeFormat(format)) == 0) return false;

	output << L"Options avancees pour " << normalizeFormat(format) << L" :\n"
	       << L"  --width N           Largeur en pixels\n"
	       << L"  --height N          Hauteur en pixels\n"
	       << L"  --fit MODE          max, crop ou scale (defaut : max)\n"
	       << L"  --strip BOOL        yes ou no (defaut : no)\n"
	       << L"  --auto-orient BOOL  yes ou no (defaut : yes)\n\n"
	       << L"Exemple :\n"
	       << L"  ec2.exe image.jpg -format " << normalizeFormat(format)
	       << L" --width 1920 --height 1080 --fit max\n";
	return true;
}

} // namespace ec2
