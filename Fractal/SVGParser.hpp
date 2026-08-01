#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cctype>

#include "PathCommand.hpp"

class SVGParser {
public:
	SVGParser() = default;
	~SVGParser() = default;

	bool ParseFullSVG(const std::string& RawFileContent);

	std::vector<PathCommand> Parse(const std::string& SVGContent, float ScaleX = 1.0f, float ScaleY = 1.0f, float TransX = 0.0f, float TransY = 0.0f);

	const std::vector<PathCommand>& GetCommands() const;

	float GetCanvasWidth() const { return CanvasWidth; }
	float GetCanvasHeight() const { return CanvasHeight; }

private:
	std::vector<PathCommand> Commands;

	float CanvasWidth = 100.0f;
	float CanvasHeight = 100.0f;

	std::string ExtractAttribute(const std::string& Content, const std::string& AttributeName);

	void ParseViewBox(const std::string& ViewBoxString);
};