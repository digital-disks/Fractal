#pragma once

#include <vector>
#include <string>
#include <array>
#include <cstdint>

#include "BinaryWriter.hpp"
#include "GeometryEngine.hpp"

struct TableRecord {
	std::string Tag;
	uint32_t Checksum = 0;
	uint32_t Offset = 0;
	uint32_t Length = 0;
	std::vector<uint8_t> Data;
};

class TrueTypeGenerator
{
public:
	TrueTypeGenerator();
	~TrueTypeGenerator() = default;

	bool CompileFont(const std::string& OutputFilePath, const std::vector<GlyphData>& AllGlyphs);

private:

	std::vector<TableRecord> Tables;

	uint32_t CalculateChecksum(const std::vector<uint8_t>& TableData) const;

	void WriteMasterHeader(BinaryWriter& BinaryWriter) const;
	void WriteTableDirectory(BinaryWriter& BinaryWriter) const;
};