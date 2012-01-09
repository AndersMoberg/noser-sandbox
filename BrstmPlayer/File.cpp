// File.cpp
// Nolan Check
// Created 11/30/2011

#include "File.hpp"

FilePtr File::Create(LPCTSTR lpFileName)
{
	FilePtr result(new File);
	if (!result->CreateInternal(lpFileName)) {
		return NULL;
	}
	return result;
}

File::File()
	: m_hFile(INVALID_HANDLE_VALUE)
{ }

File::~File()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

bool File::CreateInternal(LPCTSTR lpFileName)
{
	m_hFile = CreateFile(
		lpFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (m_hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

bool File::Read(long long offset, void* dst, size_t size)
{
	LARGE_INTEGER liOffset;
	liOffset.QuadPart = offset;
	if (!SetFilePointerEx(m_hFile, liOffset, NULL, FILE_BEGIN)) {
		return false;
	}

	DWORD bytesRead;
	if (!ReadFile(m_hFile, dst, size, &bytesRead, NULL)) {
		return false;
	}
	if (bytesRead != size) {
		return false;
	}

	return true;
}

bool File::Read8(long long offset, uint8_t* dst)
{
	return Read(offset, dst, 1);
}

bool File::Read16BE(long long offset, uint16_t* dst)
{
	if (!Read(offset, dst, 2)) {
		return false;
	}
	*dst = _rotl16(*dst, 8);
	return true;
}

bool File::Read32(long long offset, uint32_t* dst)
{
	return Read(offset, dst, 4);
}

bool File::Read32BE(long long offset, uint32_t* dst)
{
	if (!Read(offset, dst, 4)) {
		return false;
	}
	*dst = _byteswap_ulong(*dst);
	return true;
}
