// BinaryDocument.cpp
// Nolan Check
// Created 6/23/2011

#include "BinaryDocument.hpp"

#include <wx/ffile.h>

void BinaryDocument::Load(const std::string& path)
{
	wxFFile file(path);

	m_data.resize(file.Length());
	file.Read(&m_data[0], file.Length());
}

void BinaryDocument::AddStringAnnot(long long addr, size_t len)
{
	m_stringAnnots.push_back(StringAnnotation(addr, len));
}