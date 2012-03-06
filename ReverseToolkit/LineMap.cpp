// LineMap.cpp
// Nolan Check
// Created 3/5/2012

#include "LineMap.hpp"

#include <iomanip>
#include <sstream>

#include "DolDocument.hpp"
#include "ReverseToolkitFrame.hpp"

LineMapLineProvider::LineMapLineProvider(ReverseToolkitFrame* frame,
	LineViewWindow* lineViewWindow, GekkoAddressMap* gekkoMap, const DolSection* section)
	: m_frame(frame), m_gekkoMap(gekkoMap), m_section(section)
{
	if (section->type == DolSection::Text)
	{
		std::shared_ptr<GekkoLineMapNode> newNode(new GekkoLineMapNode(frame, lineViewWindow,
			section, section->address));
		m_gekkoMap->Insert(section->address, newNode);
		LineMap::ConstNodeRef nodeRef = m_lineMap.Insert(0, newNode);
		m_lineMap.Validate();
		newNode->SetNodeRef(&m_lineMap, nodeRef);
	}
	else if (section->type == DolSection::Data)
	{
		std::shared_ptr<BytesLineMapNode> newNode(new BytesLineMapNode(section, section->address));
		// FIXME: Should BytesLineMapNode go into some address map?
		m_lineMap.Insert(0, newNode);
		m_lineMap.Validate();
	}
}

bool LineMapLineProvider::GetLine(std::string& line, LineNum num) const
{
	line.clear();

	LineMap::KeyConstNodeRefPair keyAndNode = m_lineMap.FindFloor(num);

	if (keyAndNode.second.IsValid())
		return keyAndNode.second.Get()->GetSubLine(line, num - keyAndNode.first);
	else
		return false;
}

void LineMapLineProvider::OnKey(LineNum num, wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
#if 0
	case 'A':
	{
		// Create string annotation in Shift-JIS format for Japanese text
		LineMap::KeyNodeRefPair block = m_lineMap.FindFloor(num);
		if (block.second.IsValid())
		{
			// Find length of the string data
			uint32_t beginAddr = block.second.Get()->GetAddrAtLine(num - block.first);
			const uint8_t* stringPtr = &m_section->data[beginAddr - m_section->address];
			// TODO: Check for max string size depending on section size
			// Search through until we find null byte
			size_t strSize = 0;
			while (stringPtr[strSize])
				++strSize;
			if (strSize > 0)
			{
				uint32_t endAddr = beginAddr + strSize;
				wxString str(stringPtr, wxCSConv(wxFONTENCODING_SHIFT_JIS));
				std::shared_ptr<StringAnnotMapNode> newNode(new StringAnnotMapNode(m_section, beginAddr, str));
				// TODO: Don't use num as the line number. Get line num from beginAddr.
				if (num == block.first)
				{
					block.second.Get() = newNode;
					// TODO: Find correct way to place new block after string. And only place
					// new block if there isn't already a block at that address!
					LineNum nextNum = num + strSize;
					std::shared_ptr<BytesLineMapNode> newBytes(new BytesLineMapNode(m_section, endAddr));
					m_lineMap.Insert(nextNum, newBytes);
					m_lineMap.SetDistanceToNext(block.second, 1);
				}
				else
				{
					LineMap::NodeRef newNodeRef = m_lineMap.Insert(num, newNode);
					// TODO: Find correct way to place new block after string. And only place
					// new block if there isn't already a block at that address!
					LineNum nextNum = num + strSize;
					std::shared_ptr<BytesLineMapNode> newBytes(new BytesLineMapNode(m_section, endAddr));
					m_lineMap.Insert(nextNum, newBytes);
					m_lineMap.SetDistanceToNext(newNodeRef, 1);
				}
			}
		}
		break;
	}
	case 'T':
	{
		// Set size of selected line map node (test SetDistanceToNext function)
		LineMap::KeyNodeRefPair block = m_lineMap.FindFloor(num);
		if (block.second.IsValid())
		{
			m_lineMap.SetDistanceToNext(block.second, 2);
		}
		break;
	}
#endif
	case 'N':
	{
		// Create new label or rename existing label
		LineMap::KeyConstNodeRefPair block = m_lineMap.FindFloor(num);
		if (block.second.IsValid())
		{
			uint32_t labelAddr = block.second.Get()->GetAddrAtLine(num - block.first);

			std::stringstream ss;
			ss << "loc_" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << labelAddr << 'h';
			std::string originalDefault = ss.str();

			std::string defaultName;
			if (m_frame->HasLabel(labelAddr))
				defaultName = m_frame->GetLabel(labelAddr);
			else
				defaultName = originalDefault;

			wxTextEntryDialog textEntry(m_frame, "Enter label:", wxGetTextFromUserPromptStr, defaultName);
			if (textEntry.ShowModal() == wxID_OK)
			{
				if (textEntry.GetValue().IsEmpty())
					m_frame->AddLabel(labelAddr, originalDefault);
				else
					m_frame->AddLabel(labelAddr, textEntry.GetValue().c_str().AsChar());
			}
		}
		break;
	}
	case 'G':
		if (event.ControlDown())
		{
			// Go to address

			wxTextEntryDialog textEntry(m_frame, "Enter address in hex format:", wxGetTextFromUserPromptStr);
			if (textEntry.ShowModal() == wxID_OK)
			{
				unsigned long addr;
				if (!textEntry.GetValue().ToCULong(&addr, 16))
				{
					wxMessageBox("Invalid address.");
				}
				else
				{
					// Find the GekkoLineMapNode containing addr
					GekkoAddressMap::ConstNodeRef mapNode = m_gekkoMap->FindFloor(addr);
					if (mapNode.IsValid())
					{
						// Scroll to correct line number and open its window in the frame
						LineMap::ConstNodeRef lineNode = mapNode.Get()->GetNodeRef();
						LineMap::Key lineNodeKey = lineNode.GetKey();
						LineNum lineNum = lineNodeKey + mapNode.Get()->GetLineAtAddr(addr);
						if (lineNum >= 0)
						{
							LineViewWindow* lineViewWin = mapNode.Get()->GetLineViewWindow();
							lineViewWin->SetSelectedLine(lineNum);
							m_frame->OpenLineViewWindow(lineViewWin);
						}
					}
				}
			}
		}
		break;
	}
}
