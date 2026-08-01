#pragma once

#include <vector>
#include <string>
#include <cstdint>

class BinaryWriter
{
public:
	BinaryWriter() = default;
	~BinaryWriter() = default;

	void WriteUnsignedInteger8Bit(uint8_t Value);
	void WriteInteger8Bit(int8_t Value);

	void WriteUnsignedInteger16Bit(uint16_t Value);
	void WriteInteger16Bit(int16_t Value);

	void WriteUnsignedInteger32Bit(uint32_t Value);
	void WriteInteger32Bit(int32_t Value);

	void WriteTag(const std::string& Tag);

	bool SaveToFile(const std::string& FilePath) const;

	const std::vector<uint8_t>& GetBuffer() const { return Buffer; }
	size_t GetSize() const { return Buffer.size(); }


private:
	std::vector<uint8_t> Buffer;
};