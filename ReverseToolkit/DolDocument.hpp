// DolDocument.hpp
// Nolan Check
// Created 6/4/2011

#ifndef _DOLDOCUMENT_HPP
#define _DOLDOCUMENT_HPP

#include <cstdint>
#include <list>
#include <vector>

struct DolSection
{
	std::string title;
	enum
	{
		Text,
		Data
	} type;
	uint32_t address;
	std::vector<uint8_t> data;
};

class DolDocument
{

public:

	void Load(const std::string& path);
	const std::list<DolSection>& GetSections() const { return m_sections; }

private:

	std::list<DolSection> m_sections;

};

#endif
