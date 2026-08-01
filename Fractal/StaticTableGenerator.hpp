#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "BinaryWriter.hpp"

class StaticTableGenerator
{
public:
	StaticTableGenerator() = default;
	~StaticTableGenerator() = default;

	std::vector<uint8_t> GenerateOS2Table() const;
	std::vector<uint8_t> GenerateHeadTable() const;
	std::vector<uint8_t> GenerateHheaTable(uint16_t TotalGlyphs) const;
	std::vector<uint8_t> GenerateMaxpTable(uint16_t TotalGlyphs) const;
	std::vector<uint8_t> GenerateNameTable() const;
	std::vector<uint8_t> GeneratePostTable() const;
};
