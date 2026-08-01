#include <string>
#include <string_view>
#include <print>
#include <format>

#include "PathCommand.hpp"

#include "SVGParser.hpp"
#include "SVGFileLoader.hpp"
#include "GeometryEngine.hpp"
#include "BinaryWriter.hpp"
#include "TrueTypeGenerator.hpp"

int main() {
	std::println("Scanning './SVGs' directory...");

	std::vector<SVGFile> SVGFiles = SVGFileLoader::LoadDirectory("./SVGs");

	if (SVGFiles.empty()) {
		std::println("No SVGs found! Please ensure you have an 'SVGs' folder containing your icons.");
		return -1;
	}

	SVGParser Parser;
	GeometryEngine Engine;
	std::vector<GlyphData> AllParsedGlyphs;

	uint32_t CurrentUnicode = 0xE000; // PUA block starting point

	std::println("Processing {} SVG files...", SVGFiles.size());

	for (const auto& File : SVGFiles) {
		if (Parser.ParseFullSVG(File.Content)) {

			GlyphData FontPoints = Engine.ConvertToTTF(
				Parser.GetCommands(),
				Parser.GetCanvasWidth(),
				Parser.GetCanvasHeight()
			);

			AllParsedGlyphs.push_back(FontPoints);

			std::println(" -> Imported: [{}] Assigned to Unicode: U+{:X}", File.Name, CurrentUnicode);
			CurrentUnicode++;
		}
		else {
			std::println(" -> Error: Failed to parse [{}]", File.Name);
		}
	}

	TrueTypeGenerator TTFGenerator;
	std::println("\nCompiling font...");

	if (TTFGenerator.CompileFont("FractalIconFont.ttf", AllParsedGlyphs)) {
		std::println("Success! {} icons compiled into FractalIconFont.ttf", AllParsedGlyphs.size());
	}
	else {
		std::println("Error: Font compilation failed.");
	}

	return 0;
}

const std::vector<PathCommand>& SVGParser::GetCommands() const {
	return Commands;
}