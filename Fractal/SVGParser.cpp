#include "SVGParser.hpp"

std::vector<PathCommand> SVGParser::Parse(const std::string& SVGContent, float ScaleX, float ScaleY, float TransX, float TransY) {

	const char* Pointer = SVGContent.c_str();

	while (*Pointer != '\0') {
		if (std::isspace(*Pointer) || *Pointer == ',') {
			Pointer++;
			continue;
		}

		if (std::isalpha(*Pointer)) {
			Commands.push_back({ *Pointer, {} });
			Pointer++;
			continue;
		}

		if (Commands.empty()) {
			Commands.push_back({ 'M', {} });
		}

		char Command = Commands.back().type;
		size_t ArgIndex = Commands.back().args.size();

		char* NextPointer = nullptr;
		float Value = 0.0f;

		if ((Command == 'A' || Command == 'a') && (ArgIndex % 7 == 3 || ArgIndex % 7 == 4)) {

			if (*Pointer == '0') {
				Value = 0.0f;
				NextPointer = (char*)(Pointer + 1);
			}

			else if (*Pointer == '1') {
				Value = 1.0f;
				NextPointer = (char*)(Pointer + 1);
			}

			else {
				Value = std::strtof(Pointer, &NextPointer);
			}
		}

		else {
			Value = std::strtof(Pointer, &NextPointer);
		}

		float AppliedValue = Value;
		if (ScaleX != 1.0f || ScaleY != 1.0f || TransX != 0.0f || TransY != 0.0f) {
			char UpperCMD = std::toupper(Command);
			bool IsRelative = std::islower(Command);

			if (UpperCMD == 'A') {
				if (ArgIndex % 7 == 0) AppliedValue *= ScaleX; // rx
				else if (ArgIndex % 7 == 1) AppliedValue *= ScaleY; // ry
				else if (ArgIndex % 7 == 5) { AppliedValue *= ScaleX; if (!IsRelative) AppliedValue += TransX; } // x
				else if (ArgIndex % 7 == 6) { AppliedValue *= ScaleY; if (!IsRelative) AppliedValue += TransY; } // y
			}

			else if (UpperCMD == 'H') {
				AppliedValue *= ScaleX;
				if (!IsRelative) AppliedValue += TransX;
			}

			else if (UpperCMD == 'V') {
				AppliedValue *= ScaleY;
				if (!IsRelative) AppliedValue += TransY;
			}

			else {
				if (ArgIndex % 2 == 0) { // X Coordinate
					AppliedValue *= ScaleX;
					if (!IsRelative) AppliedValue += TransX;
				}

				else { // Y Coordinate
					AppliedValue *= ScaleY;
					if (!IsRelative) AppliedValue += TransY;
				}
			}
		}

		Commands.back().args.push_back(AppliedValue);

		if (Pointer == NextPointer) Pointer++;
		else Pointer = NextPointer;
	}

	return Commands;
}

std::string SVGParser::ExtractAttribute(const std::string& Content, const std::string& AttributeName) {

	std::string SearchTarget = AttributeName + "=\"";
	size_t StartPosition = Content.find(SearchTarget);
	char QuoteCharacter = '\"';

	if (StartPosition == std::string::npos) {
		SearchTarget = AttributeName + "='";
		StartPosition = Content.find(SearchTarget);
		QuoteCharacter = '\'';
		if (StartPosition == std::string::npos) return "";
	}

	StartPosition += SearchTarget.length();
	size_t EndPosition = Content.find(QuoteCharacter, StartPosition);

	if (EndPosition == std::string::npos) return "";
	return Content.substr(StartPosition, EndPosition - StartPosition);
}

void SVGParser::ParseViewBox(const std::string& ViewBoxString) {

	if (ViewBoxString.empty()) return;

	const char* Pointer = ViewBoxString.c_str();
	char* NextPointer = nullptr;

	float MinimumXCoord = std::strtof(Pointer, &NextPointer); Pointer = NextPointer;
	float MinimumYCoord = std::strtof(Pointer, &NextPointer); Pointer = NextPointer;

	CanvasWidth = std::strtof(Pointer, &NextPointer); Pointer = NextPointer;
	CanvasHeight = std::strtof(Pointer, &NextPointer);
}

bool SVGParser::ParseFullSVG(const std::string& RawFileContent) {

	Commands.clear();

	std::string ViewBoxString = ExtractAttribute(RawFileContent, "viewBox");
	ParseViewBox(ViewBoxString);

	size_t CurrentPosition = 0;

	while ((CurrentPosition = RawFileContent.find("<path", CurrentPosition)) != std::string::npos) {

		size_t PathEnd = RawFileContent.find(">", CurrentPosition);
		if (PathEnd == std::string::npos) break;

		std::string PathTag = RawFileContent.substr(CurrentPosition, PathEnd - CurrentPosition);

		std::string PathDataString = ExtractAttribute(PathTag, "d");
		std::string TransformString = ExtractAttribute(PathTag, "transform");

		float ScaleX = 1.0f, ScaleY = 1.0f;
		float TransformX = 0.0f, TransformY = 0.0f;

		if (!TransformString.empty()) {

			size_t ScalePos = TransformString.find("scale(");

			if (ScalePos != std::string::npos) {

				size_t BracketEnd = TransformString.find(")", ScalePos);

				if (BracketEnd != std::string::npos) {

					std::string Values = TransformString.substr(ScalePos + 6, BracketEnd - (ScalePos + 6));

					size_t Comma = Values.find(",");

					if (Comma != std::string::npos) {
						ScaleX = std::stof(Values.substr(0, Comma));
						ScaleY = std::stof(Values.substr(Comma + 1));
					}
					else {
						ScaleX = std::stof(Values);
						ScaleY = ScaleX;
					}
				}
			}

			size_t TransformPosition = TransformString.find("translate(");

			if (TransformPosition != std::string::npos) {

				size_t BracketEnd = TransformString.find(")", TransformPosition);

				if (BracketEnd != std::string::npos) {

					std::string Vals = TransformString.substr(TransformPosition + 10, BracketEnd - (TransformPosition + 10));

					size_t Comma = Vals.find(",");

					if (Comma != std::string::npos) {
						TransformX = std::stof(Vals.substr(0, Comma));
						TransformY = std::stof(Vals.substr(Comma + 1));
					}
					else {
						TransformX = std::stof(Vals);
					}
				}
			}
		}

		if (!PathDataString.empty()) {
			Parse(PathDataString, ScaleX, ScaleY, TransformX, TransformY);
		}

		CurrentPosition = PathEnd;
	}

	return !Commands.empty();
}