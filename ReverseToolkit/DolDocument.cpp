// DolDocument.cpp
// Nolan Check
// Created 6/4/2011

#include "DolDocument.hpp"

#include <sstream>
#include <wx/ffile.h>

inline uint32_t FromBE32(uint32_t v) { return _byteswap_ulong(v); }

void DolDocument::Load(const std::string& path)
{
	wxFFile dolFile(path, "rb");

	uint32_t textPos[7];
	uint32_t dataPos[11];
	uint32_t textAddr[7];
	uint32_t dataAddr[11];
	uint32_t textSize[7];
	uint32_t dataSize[11];
	uint32_t bssAddr;
	uint32_t bssSize;
	uint32_t entryPoint;

	dolFile.Read(textPos, sizeof(textPos));
	dolFile.Read(dataPos, sizeof(dataPos));
	dolFile.Read(textAddr, sizeof(textAddr));
	dolFile.Read(dataAddr, sizeof(dataAddr));
	dolFile.Read(textSize, sizeof(textSize));
	dolFile.Read(dataSize, sizeof(dataSize));

	for (int i = 0; i < 7; ++i)
	{
		textPos[i] = FromBE32(textPos[i]);
		textAddr[i] = FromBE32(textAddr[i]);
		textSize[i] = FromBE32(textSize[i]);
	}
	for (int i = 0; i < 11; ++i)
	{
		dataPos[i] = FromBE32(dataPos[i]);
		dataAddr[i] = FromBE32(dataAddr[i]);
		dataSize[i] = FromBE32(dataSize[i]);
	}

	dolFile.Read(&bssAddr, 4);
	bssAddr = FromBE32(bssAddr);
	dolFile.Read(&bssSize, 4);
	bssSize = FromBE32(bssSize);
	dolFile.Read(&entryPoint, 4);
	entryPoint = FromBE32(entryPoint);

	// Make dol sections for text
	for (int i = 0; i < 7; ++i)
	{
		if (textPos[i])
		{
			std::stringstream ss;
			ss << "Text " << i;

			DolSection section;
			section.title = ss.str();
			section.type = DolSection::Text;
			section.address = textAddr[i];
			section.data.resize(textSize[i]);

			dolFile.Seek(textPos[i]);
			dolFile.Read(&section.data[0], textSize[i]);

			m_sections.push_back(section);
		}
	}

	// Make dol section for data
	for (int i = 0; i < 11; ++i)
	{
		if (dataPos[i])
		{
			std::stringstream ss;
			ss << "Data " << i;

			DolSection section;
			section.title = ss.str();
			section.type = DolSection::Data;
			section.address = dataAddr[i];
			section.data.resize(dataSize[i]);

			dolFile.Seek(dataPos[i]);
			dolFile.Read(&section.data[0], dataSize[i]);

			m_sections.push_back(section);
		}
	}
}