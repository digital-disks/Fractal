#pragma once

#include <string>
#include <vector>

struct SVGFile
{
	std::string Name;
	std::string Content;
};

class SVGFileLoader
{
public:

	static std::string Load(const std::string& FilePath);
	static std::vector<SVGFile> LoadDirectory(const std::string& DirectoryPath);

private:

};
