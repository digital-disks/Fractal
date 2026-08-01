#include "StaticTableGenerator.hpp"

std::vector<uint8_t> StaticTableGenerator::GenerateOS2Table() const {

	BinaryWriter TableWriter;

	TableWriter.WriteUnsignedInteger16Bit(1); // Version
	TableWriter.WriteInteger16Bit(2048); // xAvgCharWidth
	TableWriter.WriteUnsignedInteger16Bit(400); // usWeightClass
	TableWriter.WriteUnsignedInteger16Bit(5); // usWidthClass
	TableWriter.WriteUnsignedInteger16Bit(0); // fsType

	TableWriter.WriteInteger16Bit(400); // ySubscriptXSize
	TableWriter.WriteInteger16Bit(400); // ySubscriptYSize
	TableWriter.WriteInteger16Bit(0); // ySubscriptXOffset
	TableWriter.WriteInteger16Bit(-400); // ySubscriptYOffset
	TableWriter.WriteInteger16Bit(400); // ySuperscriptXSize
	TableWriter.WriteInteger16Bit(400); // ySuperscriptYSize
	TableWriter.WriteInteger16Bit(0); // ySuperscriptXOffset
	TableWriter.WriteInteger16Bit(400); // ySuperscriptYOffset

	TableWriter.WriteInteger16Bit(50); // yStrikeoutSize
	TableWriter.WriteInteger16Bit(600); // yStrikeoutPosition
	TableWriter.WriteInteger16Bit(0); // sFamilyClass

	for (int Index = 0; Index < 10; Index++) { // PANOSE (10 bytes)
		TableWriter.WriteUnsignedInteger8Bit(0);
	}

	TableWriter.WriteUnsignedInteger32Bit(0); // ulUnicodeRange1
	TableWriter.WriteUnsignedInteger32Bit(0); // ulUnicodeRange2
	TableWriter.WriteUnsignedInteger32Bit(0); // ulUnicodeRange3
	TableWriter.WriteUnsignedInteger32Bit(0); // ulUnicodeRange4

	TableWriter.WriteUnsignedInteger8Bit('D'); // achVendID
	TableWriter.WriteUnsignedInteger8Bit('G');
	TableWriter.WriteUnsignedInteger8Bit('T');
	TableWriter.WriteUnsignedInteger8Bit('L');

	TableWriter.WriteUnsignedInteger16Bit(0x0040); // fsSelection
	TableWriter.WriteUnsignedInteger16Bit(0x0020); // usFirstCharIndex
	TableWriter.WriteUnsignedInteger16Bit(0xE000); // usLastCharIndex

	TableWriter.WriteInteger16Bit(2048); // sTypoAscender
	TableWriter.WriteInteger16Bit(0); // sTypoDescender
	TableWriter.WriteInteger16Bit(0); // sTypoLineGap
	TableWriter.WriteUnsignedInteger16Bit(2048); // usWinAscent
	TableWriter.WriteUnsignedInteger16Bit(0); // usWinDescent

	TableWriter.WriteUnsignedInteger32Bit(1); // ulCodePageRange1
	TableWriter.WriteUnsignedInteger32Bit(0); // ulCodePageRange2

	return TableWriter.GetBuffer();
}

std::vector<uint8_t> StaticTableGenerator::GenerateHeadTable() const {

	BinaryWriter TableWriter;

	TableWriter.WriteUnsignedInteger32Bit(0x00010000); // Table Version
	TableWriter.WriteUnsignedInteger32Bit(0x00010000); // Font Revision

	TableWriter.WriteUnsignedInteger32Bit(0); // Checksum Adjustment (Will come back to calculate it correctly)

	TableWriter.WriteUnsignedInteger32Bit(0x5F0F3CF5); // Magic Number

	TableWriter.WriteUnsignedInteger16Bit(0x000B); // Flags

	TableWriter.WriteUnsignedInteger16Bit(2048); // Units per EM

	TableWriter.WriteUnsignedInteger32Bit(0); // Date Created
	TableWriter.WriteUnsignedInteger32Bit(0); // Another one to make it 64-Bit

	TableWriter.WriteUnsignedInteger32Bit(0); // Date Modified
	TableWriter.WriteUnsignedInteger32Bit(0); // Another one to make it 64-Bit

	TableWriter.WriteInteger16Bit(0); //XMin Bouding Box
	TableWriter.WriteInteger16Bit(0); //YMin Bouding Box
	TableWriter.WriteInteger16Bit(2048); //XMax Bouding Box
	TableWriter.WriteInteger16Bit(2048); //YMax Bouding Box

	TableWriter.WriteUnsignedInteger16Bit(0); // Mac Style Regular

	TableWriter.WriteUnsignedInteger16Bit(8); // Lowest Recommended PPEM

	TableWriter.WriteInteger16Bit(2); // Font Direction Hint (Left to Right)

	TableWriter.WriteInteger16Bit(0); // 0 = Short offsets (will be needed for the loca table)

	TableWriter.WriteInteger16Bit(0); // Glyph Data Format

	return TableWriter.GetBuffer();
}

std::vector<uint8_t> StaticTableGenerator::GenerateHheaTable(uint16_t TotalGlyphs) const {

	BinaryWriter TableWriter;

	TableWriter.WriteUnsignedInteger32Bit(0x00010000); // Table Version

	TableWriter.WriteInteger16Bit(2048); // EMSquare (Top of the font)
	TableWriter.WriteInteger16Bit(-400); // EMSquare (Bottom of the font)
	TableWriter.WriteInteger16Bit(0); // EMSquare (Line Gap of the font)

	TableWriter.WriteUnsignedInteger16Bit(2048); // AdvanceMaxWidth
	TableWriter.WriteInteger16Bit(0); // MinLeftSideBearing
	TableWriter.WriteInteger16Bit(0); // MinRightSideBearing
	TableWriter.WriteInteger16Bit(2048); // XMaxExtent

	TableWriter.WriteInteger16Bit(1); // CaretSlopeRise
	TableWriter.WriteInteger16Bit(0); // CaretSlopeRun
	TableWriter.WriteInteger16Bit(0); // CaretOffset

	TableWriter.WriteInteger16Bit(0); // Padding
	TableWriter.WriteInteger16Bit(0); // Padding
	TableWriter.WriteInteger16Bit(0); // Padding
	TableWriter.WriteInteger16Bit(0); // Padding

	TableWriter.WriteInteger16Bit(0); // Metric Data Format

	TableWriter.WriteUnsignedInteger16Bit(TotalGlyphs); // Number of horizontal metrics

	return TableWriter.GetBuffer();
}

std::vector<uint8_t> StaticTableGenerator::GenerateMaxpTable(uint16_t TotalGlyphs) const {

	BinaryWriter TableWriter;

	TableWriter.WriteUnsignedInteger32Bit(0x00010000); // Table Version

	TableWriter.WriteUnsignedInteger16Bit(TotalGlyphs); // Number of Glyphs minimum

	TableWriter.WriteUnsignedInteger16Bit(200); // MaxPoints
	TableWriter.WriteUnsignedInteger16Bit(10); // MaxContours
	TableWriter.WriteUnsignedInteger16Bit(0); // MaxCompositePoints
	TableWriter.WriteUnsignedInteger16Bit(0); // MaxCompositeContours

	TableWriter.WriteUnsignedInteger16Bit(2); // MaxZones

	TableWriter.WriteUnsignedInteger16Bit(0); // MaxTwilightPoints
	TableWriter.WriteUnsignedInteger16Bit(0); // MaxStorage
	TableWriter.WriteUnsignedInteger16Bit(0); // MaxFunctionDefs
	TableWriter.WriteUnsignedInteger16Bit(0); // MaxInstructionDefs
	TableWriter.WriteUnsignedInteger16Bit(0); // MaxStackElements
	TableWriter.WriteUnsignedInteger16Bit(0); // MaxSizeOfElements
	TableWriter.WriteUnsignedInteger16Bit(0); // MaxComponentElements
	TableWriter.WriteUnsignedInteger16Bit(0); // MaxComponentDepth

	return TableWriter.GetBuffer();
}

std::vector<uint8_t> StaticTableGenerator::GenerateNameTable() const {

	BinaryWriter TableWriter;

	std::vector<std::pair<uint16_t, std::string>> NameRecords = {

		{1, "Fractal"},				// Family
		{2, "Regular"},				//SubFamily
		{3, "Fractal 1.0"},			//Unique ID
		{4, "Fractal Regular"},		// Full Name
		{5, "Version 1.0"},			//Version String
		{6, "Fractal-Regular"}		//PostScript Name
	};

	TableWriter.WriteUnsignedInteger16Bit(0); // Format 0
	TableWriter.WriteUnsignedInteger16Bit(static_cast<uint16_t>(NameRecords.size())); // Number of Name Records
	TableWriter.WriteUnsignedInteger16Bit(6 + (NameRecords.size() * 12)); // Offset to string storage

	uint16_t CurrentStringOffset = 0;

	for (const auto& Record : NameRecords) {

		uint16_t StringLength = static_cast<uint16_t>(Record.second.length() * 2);

		TableWriter.WriteUnsignedInteger16Bit(3);
		TableWriter.WriteUnsignedInteger16Bit(1);
		TableWriter.WriteUnsignedInteger16Bit(0x0409);
		TableWriter.WriteUnsignedInteger16Bit(Record.first);
		TableWriter.WriteUnsignedInteger16Bit(StringLength);
		TableWriter.WriteUnsignedInteger16Bit(CurrentStringOffset);

		CurrentStringOffset += StringLength;
	}

	for (const auto& Record : NameRecords) {
		
		for (char Character : Record.second) {

			TableWriter.WriteUnsignedInteger8Bit(0);
			TableWriter.WriteUnsignedInteger8Bit(Character);
		}
	}

	return TableWriter.GetBuffer();
}

std::vector<uint8_t> StaticTableGenerator::GeneratePostTable() const {

	BinaryWriter TableWriter;

	TableWriter.WriteUnsignedInteger32Bit(0x00030000); // Table Version

	TableWriter.WriteUnsignedInteger32Bit(0); // Italic Angle

	TableWriter.WriteInteger16Bit(-100); // UnderlinePosition
	TableWriter.WriteInteger16Bit(50); // UnderlineThickness

	TableWriter.WriteUnsignedInteger32Bit(1); // IsFixedPitch (Monoscaped)

	TableWriter.WriteUnsignedInteger32Bit(0); // Memory Configuration for legacy printers MinMemType42
	TableWriter.WriteUnsignedInteger32Bit(0); // Memory Configuration for legacy printers MaxMemType42
	TableWriter.WriteUnsignedInteger32Bit(0); // Memory Configuration for legacy printers MinMemType1
	TableWriter.WriteUnsignedInteger32Bit(0); // Memory Configuration for legacy printers MaxMemType1

	return TableWriter.GetBuffer();
}