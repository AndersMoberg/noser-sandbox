// File.hpp
// Nolan Check
// Created 11/30/2011

#ifndef _FILE_HPP
#define _FILE_HPP

#include <Windows.h>

#include <cstdint>
#include <memory>

class File;
typedef std::shared_ptr<File> FilePtr;

class File
{

public:

	static FilePtr Create(LPCTSTR lpFileName);
	~File();

	bool Read(long long offset, void* dst, size_t size);
	bool Read8(long long offset, uint8_t* dst);
	bool Read16BE(long long offset, uint16_t* dst);
	bool Read32(long long offset, uint32_t* dst);
	bool Read32BE(long long offset, uint32_t* dst);

private:

	File();
	bool CreateInternal(LPCTSTR lpFileName);

	HANDLE m_hFile;

};

#endif
