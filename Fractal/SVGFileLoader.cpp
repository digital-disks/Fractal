#include "SVGFileLoader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <print>
#include <filesystem>

std::string SVGFileLoader::Load(const std::string& FilePath) {

	std::ifstream File(FilePath);

	if (!File.is_open()) {

		std::println("Error: Could not open the SVG file at this directory {}", FilePath);

		return "";
	}

	std::ostringstream Stream;

	Stream << File.rdbuf();

	return Stream.str();
}

std::vector<SVGFile> SVGFileLoader::LoadDirectory(const std::string& DirectoryPath) {

	std::vector<SVGFile> Files;

	if (!std::filesystem::exists(DirectoryPath)) {

		std::println("Error: Directory '{}' does not exist.", DirectoryPath);
		return Files;
	}

	for (const auto& Entry : std::filesystem::directory_iterator(DirectoryPath)) {
		if (Entry.path().extension() == ".svg") {

			std::ifstream File(Entry.path());

			if (File.is_open()) {
				std::ostringstream Stream;
				Stream << File.rdbuf();
				Files.push_back({ Entry.path().filename().string(), Stream.str() });
			}
		}
	}

	return Files;
}
