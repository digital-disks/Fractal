#include "BinaryWriter.hpp"

#include <fstream>
#include <iostream>

void BinaryWriter::WriteUnsignedInteger8Bit(uint8_t Value) {

	Buffer.push_back(Value);
}

void BinaryWriter::WriteInteger8Bit(int8_t Value) {

	Buffer.push_back(static_cast<uint8_t>(Value));
}

void BinaryWriter::WriteUnsignedInteger16Bit(uint16_t Value) {

	Buffer.push_back(static_cast<uint8_t>((Value >> 8) & 0xFF));
	Buffer.push_back(static_cast<uint8_t>(Value & 0xFF));
}

void BinaryWriter::WriteInteger16Bit(int16_t Value) {

	Buffer.push_back(static_cast<uint8_t>((Value >> 8) & 0xFF));
	Buffer.push_back(static_cast<uint8_t>(Value & 0xFF));
}

void BinaryWriter::WriteUnsignedInteger32Bit(uint32_t Value) {

	Buffer.push_back(static_cast<uint8_t>((Value >> 24) & 0xFF));
	Buffer.push_back(static_cast<uint8_t>((Value >> 16) & 0xFF));
	Buffer.push_back(static_cast<uint8_t>((Value >> 8) & 0xFF));
	Buffer.push_back(static_cast<uint8_t>(Value & 0xFF));
}

void BinaryWriter::WriteInteger32Bit(int32_t Value) {

	Buffer.push_back(static_cast<uint8_t>((Value >> 24) & 0xFF));
	Buffer.push_back(static_cast<uint8_t>((Value >> 16) & 0xFF));
	Buffer.push_back(static_cast<uint8_t>((Value >> 8) & 0xFF));
	Buffer.push_back(static_cast<uint8_t>(Value & 0xFF));
}

void BinaryWriter::WriteTag(const std::string& Tag) {

	for (size_t Index = 0; Index < 4; Index++) {

		if (Index < Tag.length()) {
			Buffer.push_back(static_cast<uint8_t>(Tag[Index]));
		}

		else {
			Buffer.push_back(0x20);
		}
	}
}

bool BinaryWriter::SaveToFile(const std::string& FilePath) const {

	std::ofstream File(FilePath, std::ios::binary);

	if (!File.is_open()) return false;

	File.write(reinterpret_cast<const char*>(Buffer.data()), Buffer.size());

	return true;
}