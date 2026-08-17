#include "cli.hpp"

#include "conversion_options.hpp"
#include "formats.hpp"

#include <iostream>

namespace ec2 {

void printHelp() {
	std::wcout
	        << L"EC2 - convertisseur d'images\n\n"
	        << L"Utilisation :\n"
	        << L"  ec2.exe <image> -format <format> [-o <sortie>] [--force]\n\n"
	        << L"Options generales :\n"
	        << L"  -format, -f       Format de destination\n"
	        << L"  -o                Fichier de sortie\n"
	        << L"  --force           Autoriser l'ecrasement\n"
	        << L"  --formats         Lister les formats\n"
	        << L"  -h, --help        Afficher cette aide\n\n"
	        << L"Aide d'un format :\n"
	        << L"  ec2.exe -h -f <format>\n"
	        << L"  ec2.exe --help <format>\n";
}

Options parseArguments(int argc, wchar_t* argv[]) {
	Options options;
	if (argc == 1) {
		options.action = Action::Help;
		return options;
	}

	const std::wstring firstArgument = argv[1];
	if (firstArgument == L"-h" || firstArgument == L"--help") {
		if (argc == 2) {
			options.action = Action::Help;
			return options;
		}
		if (argc == 3) {
			options.helpFormat = normalizeFormat(argv[2]);
		} else if (argc == 4
		           && (std::wstring(argv[2]) == L"-f"
		               || std::wstring(argv[2]) == L"-format"
		               || std::wstring(argv[2]) == L"--format")) {
			options.helpFormat = normalizeFormat(argv[3]);
		} else {
			options.error = L"utilisez -h -f <format> ou --help <format>.";
			return options;
		}
		if (!isSupportedOutputFormat(options.helpFormat)) {
			options.error = L"format inconnu '" + options.helpFormat + L"'.";
			return options;
		}
		options.action = Action::FormatHelp;
		return options;
	}

	for (int index = 1; index < argc; ++index) {
		const std::wstring argument = argv[index];

		if (argument == L"-h" || argument == L"--help") {
			options.action = Action::Help;
			return options;
		}
		if (argument == L"--formats") {
			options.action = Action::Formats;
			return options;
		}
		if (argument == L"--force") {
			options.force = true;
			continue;
		}

		const OptionParseResult advanced = parseConversionOption(
		                                       argument, argc, argv, index, options.conversion, options.error);
		if (advanced == OptionParseResult::Error) return options;
		if (advanced == OptionParseResult::Parsed) continue;

		if (argument == L"-format" || argument == L"--format" || argument == L"-f") {
			if (++index >= argc) {
				options.error = L"format manquant.";
				return options;
			}
			options.format = normalizeFormat(argv[index]);
		} else if (argument == L"-o" || argument == L"--output") {
			if (++index >= argc) {
				options.error = L"chemin de sortie manquant.";
				return options;
			}
			options.output = argv[index];
		} else if (!argument.empty() && argument.front() == L'-') {
			options.error = L"option inconnue : " + argument;
			return options;
		} else if (options.input.empty()) {
			options.input = argument;
		} else {
			options.error = L"un seul fichier d'entree est accepte.";
			return options;
		}
	}

	if (options.input.empty() || options.format.empty()) {
		options.error = L"indiquez une image et -format <format>.";
	} else if (!isSupportedOutputFormat(options.format)) {
		options.error = L"format de sortie non accepte '" + options.format
		                + L"'. Utilisez --formats.";
	} else if (!validateConversionOptions(options.format, options.conversion, options.error)) {
		// Le message est fourni par le profil d'options du format.
	} else {
		options.action = Action::Convert;
	}
	return options;
}

} // namespace ec2
