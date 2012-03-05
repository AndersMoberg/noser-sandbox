// BinaryDocument.hpp
// Nolan Check
// Created 6/23/2011

#ifndef _BINARYDOCUMENT_HPP
#define _BINARYDOCUMENT_HPP

#include <cstdint>
#include <list>
#include <vector>

struct StringAnnotation
{
	StringAnnotation(long long _addr, size_t _len)
		: addr(_addr), len(_len)
	{ }

	long long addr;
	size_t len;
};

typedef std::list<StringAnnotation> StringAnnotList;

class BinaryDocument
{

public:

	void Load(const std::string& path);
	const std::vector<uint8_t>& GetData() const { return m_data; }
	const StringAnnotList& GetStringAnnots() const { return m_stringAnnots; }

	void AddStringAnnot(long long addr, size_t len);

private:

	std::vector<uint8_t> m_data;
	StringAnnotList m_stringAnnots;

};

#endif
