#include "stdafx.h"
#include "NanoIO.h"
#include "NanoLog.h"
//=============================================================================
template <typename T>
[[nodiscard]] bool contains(const std::vector<T>& vec, const T& obj) noexcept
{
	return std::find(vec.begin(), vec.end(), obj) != vec.end();
}
//=============================================================================
bool io::Exists(const std::string& filePath)
{
	return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
}
//=============================================================================
std::filesystem::path io::CurrentPath()
{
	return std::filesystem::current_path();
}
//=============================================================================
std::string io::GetFileExtension(const std::string& filePath)
{
	std::filesystem::path path(filePath);
	return path.extension().string();
}
//=============================================================================
std::string io::GetFileName(const std::string& filePath)
{
	std::filesystem::path path(filePath);
	return path.filename().string();
}
//=============================================================================
std::string io::GetFileNameWithoutExtension(const std::string& filePath)
{
	std::filesystem::path path(filePath);
	std::string fileName = GetFileName(filePath);
	std::string extension = GetFileExtension(filePath);

	if (!extension.empty())
		return fileName.substr(0, fileName.size() - extension.size());

	return fileName;
}
//=============================================================================
std::string io::GetFileDirectory(const std::string& filePath)
{
	std::filesystem::path path(filePath);
	return path.parent_path().string() + "/";
}
//=============================================================================
// see https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
std::string io::LoadFile(const std::filesystem::path& path)
{
	std::ifstream asciiFile(path, std::ios::in);
	if (!asciiFile.is_open())
	{
		Error("Fail to open file: " + path.string());
		return {};
	}

	asciiFile.seekg(0, std::ios::end);
	auto size = asciiFile.tellg();
	if (size < 0)
	{
		Error("Cannot determine file size: " + path.string());
		return {};
	}

	asciiFile.seekg(0, std::ios::beg);

	std::string content;
	content.resize(static_cast<size_t>(size));
	asciiFile.read(content.data(), size);
	if (content.empty())
	{
		Error("Error reading file: " + path.string());
		return {};
	}

	asciiFile.close();
	return content;
}
//=============================================================================
// see http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::vector<char> io::LoadBinaryFile(const std::filesystem::path& path)
{
	std::ifstream binaryFile(path, std::ios::in | std::ios::binary);
	if (!binaryFile.is_open())
	{
		Error("Fail to open file: " + path.string());
		return {};
	}

	// Create a buffer with the right size
	std::size_t fileSize = std::filesystem::file_size(path);
	std::vector<char> buffer(static_cast<uint64_t>(fileSize));
	binaryFile.read(&buffer[0], static_cast<std::streamsize>(buffer.size()));

	// Close the file and return
	binaryFile.close();
	return buffer;
}
//=============================================================================