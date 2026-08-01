#include "TrueTypeGenerator.hpp"
#include "StaticTableGenerator.hpp"
#include "DynamicTableGenerator.hpp"
#include "GeometryEngine.hpp"

TrueTypeGenerator::TrueTypeGenerator() {

	Tables.push_back({ "OS/2", 0, 0, 0, {} });
	Tables.push_back({ "cmap", 0, 0, 0, {} });
	Tables.push_back({ "glyf", 0, 0, 0, {} });
	Tables.push_back({ "head", 0, 0, 0, {} });
	Tables.push_back({ "hhea", 0, 0, 0, {} });
	Tables.push_back({ "hmtx", 0, 0, 0, {} });
	Tables.push_back({ "loca", 0, 0, 0, {} });
	Tables.push_back({ "maxp", 0, 0, 0, {} });
	Tables.push_back({ "name", 0, 0, 0, {} });
	Tables.push_back({ "post", 0, 0, 0, {} });
}

uint32_t TrueTypeGenerator::CalculateChecksum(const std::vector<uint8_t>& TableData) const {

	uint32_t Sum = 0;

	size_t CheckCount = TableData.size() / 4;

	for (size_t Index = 0; Index < CheckCount; Index++) {
		
		size_t ByteOffset = Index * 4;

		uint32_t Chunk = (static_cast<uint32_t>(TableData[ByteOffset]) << 24) |
			(static_cast<uint32_t>(TableData[ByteOffset + 1]) << 16) |
			(static_cast<uint32_t>(TableData[ByteOffset + 2]) << 8) |
			(static_cast<uint32_t>(TableData[ByteOffset + 3]));

		Sum += Chunk;
	}

	size_t Remainder = TableData.size() % 4;

	if (Remainder > 0) {

		uint32_t LastChunk = 0;

		for (size_t Index = 0; Index < Remainder; Index++) {
			LastChunk |= (static_cast<uint32_t>(TableData[TableData.size() - Remainder + Index]) << (24 - (Index * 8)));
		}

		Sum += LastChunk;
	}

	return Sum;
}

void TrueTypeGenerator::WriteMasterHeader(BinaryWriter& BinaryWriter) const {

	uint16_t TableCount = static_cast<uint16_t>(Tables.size());

	uint16_t MaximumPowerOf2 = 1;
	uint16_t EntrySelector = 0;

	while ((MaximumPowerOf2 * 2) <= TableCount) {
		MaximumPowerOf2 *= 2;
		EntrySelector++;
	}

	uint16_t SearchRange = MaximumPowerOf2 * 16;
	uint16_t RangeShift = (TableCount * 16) - SearchRange;

	constexpr uint32_t TTFMagicNumber = 0x00010000;

	BinaryWriter.WriteUnsignedInteger32Bit(TTFMagicNumber);
	BinaryWriter.WriteUnsignedInteger16Bit(TableCount);
	BinaryWriter.WriteUnsignedInteger16Bit(SearchRange);
	BinaryWriter.WriteUnsignedInteger16Bit(EntrySelector);
	BinaryWriter.WriteUnsignedInteger16Bit(RangeShift);
}

void TrueTypeGenerator::WriteTableDirectory(BinaryWriter& BinaryWriter) const {

	for (const auto& Table : Tables) {

		BinaryWriter.WriteTag(Table.Tag);
		BinaryWriter.WriteUnsignedInteger32Bit(Table.Checksum);
		BinaryWriter.WriteUnsignedInteger32Bit(Table.Offset);
		BinaryWriter.WriteUnsignedInteger32Bit(Table.Length);
	}
}

bool TrueTypeGenerator::CompileFont(const std::string& OutputFilePath, const std::vector<GlyphData>& AllGlyphs) {

	BinaryWriter BinaryWriter;
	StaticTableGenerator StaticTableGenerator;
	DynamicTableGenerator DynamicTableGenerator;

	uint16_t TotalIcons = static_cast<uint16_t>(AllGlyphs.size());
	uint16_t TotalGlyphs = TotalIcons + 1;

	Tables[0].Data = StaticTableGenerator.GenerateOS2Table();
	Tables[3].Data = StaticTableGenerator.GenerateHeadTable();
	Tables[4].Data = StaticTableGenerator.GenerateHheaTable(TotalGlyphs); 
	Tables[7].Data = StaticTableGenerator.GenerateMaxpTable(TotalGlyphs); 
	Tables[8].Data = StaticTableGenerator.GenerateNameTable();
	Tables[9].Data = StaticTableGenerator.GeneratePostTable();

	Tables[1].Data = DynamicTableGenerator.GenerateCmapTable(TotalIcons); 
	Tables[2].Data = DynamicTableGenerator.GenerateGlyfTable(AllGlyphs);
	Tables[5].Data = DynamicTableGenerator.GenerateHmtxTable(TotalGlyphs);
	Tables[6].Data = DynamicTableGenerator.GenerateLocaTable();

	for (auto& Table : Tables) {
		Table.Length = static_cast<uint32_t>(Table.Data.size());
	}

	uint32_t CurrentOffset = 172;

	for (auto& Table : Tables) {

		if (Table.Length > 0) {
			Table.Checksum = CalculateChecksum(Table.Data);

			Table.Offset = CurrentOffset;

			uint32_t Padding = (4 - (Table.Length % 4)) % 4;

			CurrentOffset += Table.Length + Padding;
		}
	}

	WriteMasterHeader(BinaryWriter);
	WriteTableDirectory(BinaryWriter);


	uint32_t GlobalSum = CalculateChecksum(BinaryWriter.GetBuffer());

	for (const auto& Table : Tables) {
		GlobalSum += Table.Checksum;
	}

	uint32_t CheckSumAdjustment = 0xB1B0AFBA - GlobalSum;

	Tables[3].Data[8] = (CheckSumAdjustment >> 24) & 0xFF;
	Tables[3].Data[9] = (CheckSumAdjustment >> 16) & 0xFF;
	Tables[3].Data[10] = (CheckSumAdjustment >> 8) & 0xFF;
	Tables[3].Data[11] = CheckSumAdjustment & 0xFF;

	for (auto& Table : Tables) {
		if (Table.Length > 0) {

			for (uint8_t Byte : Table.Data) {
				BinaryWriter.WriteUnsignedInteger8Bit(Byte);
			}

			size_t Remainder = Table.Length % 4;

			if (Remainder > 0) {
				for (size_t Index = 0; Index < (4 - Remainder); Index++) {
					BinaryWriter.WriteUnsignedInteger8Bit(0);
				}
			}
		}
	}

	return BinaryWriter.SaveToFile(OutputFilePath);
}