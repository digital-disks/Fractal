#pragma once

#include <vector>
#include <cstdint>

#include "BinaryWriter.hpp"
#include "GeometryEngine.hpp"

class DynamicTableGenerator
{
public:
	DynamicTableGenerator() = default;
	~DynamicTableGenerator() = default;

	std::vector<uint8_t> GenerateCmapTable(uint16_t TotalIcons) const;
	std::vector<uint8_t> GenerateGlyfTable(const std::vector<GlyphData>& SVGs);
	std::vector<uint8_t> GenerateHmtxTable(uint16_t TotalGlyphs) const;
	std::vector<uint8_t> GenerateLocaTable() const;

private:

	std::vector<uint32_t> GlyphOffsets;

};
