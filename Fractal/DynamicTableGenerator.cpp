#include "DynamicTableGenerator.hpp"

std::vector<uint8_t> DynamicTableGenerator::GenerateGlyfTable(const std::vector<GlyphData>& SVGs) {

	BinaryWriter TableWriter;

	GlyphOffsets.clear();

	// .notdef Glyph
	GlyphOffsets.push_back(0);

	TableWriter.WriteInteger16Bit(1); // NumberOfContours
	TableWriter.WriteInteger16Bit(500); // XMin
	TableWriter.WriteInteger16Bit(500); // YMin
	TableWriter.WriteInteger16Bit(1500); // XMax
	TableWriter.WriteInteger16Bit(1500); // YMax
	TableWriter.WriteUnsignedInteger16Bit(3); // End points of contours
	TableWriter.WriteUnsignedInteger16Bit(0); // Instruction Length
	TableWriter.WriteUnsignedInteger8Bit(0x01); // Point 0
	TableWriter.WriteUnsignedInteger8Bit(0x11); // Point 1
	TableWriter.WriteUnsignedInteger8Bit(0x21); // Point 2
	TableWriter.WriteUnsignedInteger8Bit(0x11); // Point 3
	TableWriter.WriteInteger16Bit(500);
	TableWriter.WriteInteger16Bit(1000);
	TableWriter.WriteInteger16Bit(500);
	TableWriter.WriteInteger16Bit(1000);
	TableWriter.WriteInteger16Bit(-1000);

	GlyphOffsets.push_back(static_cast<uint32_t>(TableWriter.GetBuffer().size()));

	for (const auto& SVGData : SVGs) {
		if (SVGData.Points.empty()) {
			GlyphOffsets.push_back(static_cast<uint32_t>(TableWriter.GetBuffer().size()));
			continue;
		}

		int xMin = 32767, yMin = 32767, xMax = -32768, yMax = -32768;
		for (const auto& Point : SVGData.Points) {
			if (Point.X < xMin) xMin = Point.X;
			if (Point.Y < yMin) yMin = Point.Y;
			if (Point.X > xMax) xMax = Point.X;
			if (Point.Y > yMax) yMax = Point.Y;
		}

		TableWriter.WriteInteger16Bit(static_cast<int16_t>(SVGData.ContourEndIndices.size()));
		TableWriter.WriteInteger16Bit(xMin);
		TableWriter.WriteInteger16Bit(yMin);
		TableWriter.WriteInteger16Bit(xMax);
		TableWriter.WriteInteger16Bit(yMax);

		for (uint16_t EndIndex : SVGData.ContourEndIndices) {
			TableWriter.WriteUnsignedInteger16Bit(EndIndex);
		}
		TableWriter.WriteUnsignedInteger16Bit(0); // Instructions

		std::vector<uint8_t> Flags, XDeltas, YDeltas;
		int PreviousX = 0, PreviousY = 0;

		for (const auto& Point : SVGData.Points) {
			uint8_t Flag = Point.OnCurve ? 0x01 : 0x00;
			int dx = Point.X - PreviousX;
			int dy = Point.Y - PreviousY;

			// X Coordinates Compression
			if (dx == 0) { Flag |= 0x10; }
			else if (dx >= -255 && dx <= 255) { Flag |= 0x02; if (dx > 0) Flag |= 0x10; XDeltas.push_back(static_cast<uint8_t>(std::abs(dx))); }
			else { XDeltas.push_back(static_cast<uint8_t>((dx >> 8) & 0xFF)); XDeltas.push_back(static_cast<uint8_t>(dx & 0xFF)); }

			// Y Coordinates Compression
			if (dy == 0) { Flag |= 0x20; }
			else if (dy >= -255 && dy <= 255) { Flag |= 0x04; if (dy > 0) Flag |= 0x20; YDeltas.push_back(static_cast<uint8_t>(std::abs(dy))); }
			else { YDeltas.push_back(static_cast<uint8_t>((dy >> 8) & 0xFF)); YDeltas.push_back(static_cast<uint8_t>(dy & 0xFF)); }

			Flags.push_back(Flag);
			PreviousX = Point.X;
			PreviousY = Point.Y;
		}

		for (uint8_t Flag : Flags) TableWriter.WriteUnsignedInteger8Bit(Flag);
		for (uint8_t XData : XDeltas) TableWriter.WriteUnsignedInteger8Bit(XData);
		for (uint8_t YData : YDeltas) TableWriter.WriteUnsignedInteger8Bit(YData);

		if (TableWriter.GetBuffer().size() % 2 != 0) {
			TableWriter.WriteUnsignedInteger8Bit(0);
		}

		GlyphOffsets.push_back(static_cast<uint32_t>(TableWriter.GetBuffer().size()));
	}

	return TableWriter.GetBuffer();
}

std::vector<uint8_t> DynamicTableGenerator::GenerateCmapTable(uint16_t TotalIcons) const {
	BinaryWriter TableWriter;

	TableWriter.WriteUnsignedInteger16Bit(0); // Table Version
	TableWriter.WriteUnsignedInteger16Bit(1); // Number of encoding records

	TableWriter.WriteUnsignedInteger16Bit(3); // Platform ID (Windows)
	TableWriter.WriteUnsignedInteger16Bit(1); // Encoding ID (Unicode)
	TableWriter.WriteUnsignedInteger32Bit(12); // Offset to Format 4

	TableWriter.WriteUnsignedInteger16Bit(4);  // Format 4
	TableWriter.WriteUnsignedInteger16Bit(40); // Length (Now 40 bytes instead of 32!)
	TableWriter.WriteUnsignedInteger16Bit(0);  // Language

	TableWriter.WriteUnsignedInteger16Bit(6); // segCountX2 (3 * 2)
	TableWriter.WriteUnsignedInteger16Bit(4); // searchRange (2 * (largest power of 2 <= 3))
	TableWriter.WriteUnsignedInteger16Bit(1); // entrySelector (log2(2))
	TableWriter.WriteUnsignedInteger16Bit(2); // rangeShift (segCountX2 - searchRange)

	TableWriter.WriteUnsignedInteger16Bit(0x0020); // Space

	uint16_t LastIconUnicode = 0xE000 + (TotalIcons > 0 ? TotalIcons - 1 : 0);
	TableWriter.WriteUnsignedInteger16Bit(LastIconUnicode); // Icon
	TableWriter.WriteUnsignedInteger16Bit(0xFFFF); // Terminator

	TableWriter.WriteUnsignedInteger16Bit(0);

	TableWriter.WriteUnsignedInteger16Bit(0x0020); // Space
	TableWriter.WriteUnsignedInteger16Bit(0xE000); // Icon
	TableWriter.WriteUnsignedInteger16Bit(0xFFFF); // Terminator

	TableWriter.WriteUnsignedInteger16Bit(0xFFE0); // Space Math: 1 - 0x0020 = -31 (0xFFE1)
	TableWriter.WriteUnsignedInteger16Bit(0x2001); // Icon Math: 1 - 0xE000 = 8193 (0x2001)
	TableWriter.WriteUnsignedInteger16Bit(1);      // Terminator

	TableWriter.WriteUnsignedInteger16Bit(0);
	TableWriter.WriteUnsignedInteger16Bit(0);
	TableWriter.WriteUnsignedInteger16Bit(0);

	return TableWriter.GetBuffer();
}

std::vector<uint8_t> DynamicTableGenerator::GenerateHmtxTable(uint16_t TotalGlyphs) const {

	BinaryWriter TableWriter;

	for (uint16_t Index = 0; Index < TotalGlyphs; Index++) {
		TableWriter.WriteUnsignedInteger16Bit(2048); // Advance Width
		TableWriter.WriteInteger16Bit(0); // Left Side Bearing
	}

	return TableWriter.GetBuffer();
}

std::vector<uint8_t> DynamicTableGenerator::GenerateLocaTable() const {

	BinaryWriter TableWriter;

	for (uint32_t Offset : GlyphOffsets) {
		TableWriter.WriteUnsignedInteger16Bit(static_cast<uint16_t>(Offset / 2));
	}

	return TableWriter.GetBuffer();
}