// ReverseToolkitFrame.cpp
// Nolan Check
// Created 6/3/2011

#include "ReverseToolkitFrame.hpp"

#include <iomanip>
#include <sstream>
#include <wx/notebook.h>

#include "GekkoDisassembly.hpp"
#include "LineViewWindow.hpp"
#include "RelTree.hpp"

ReverseToolkitFrame::ReverseToolkitFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title),
	m_notebook(NULL)
{
	wxPanel* mainPanel = new wxPanel(this);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	m_notebook = new wxNotebook(mainPanel, wxID_ANY);
	sizer->Add(m_notebook, 1, wxEXPAND | wxALL);
	mainPanel->SetSizer(sizer);
}

class LineMapNode
{
public:
	virtual ~LineMapNode() { }
	virtual bool GetSubLine(std::string& line, LineNum num) const = 0;
	virtual uint32_t GetAddrAtLine(LineNum num) const = 0;
};

typedef RelTree<std::shared_ptr<LineMapNode> > LineMap;

class BytesLineMapNode : public LineMapNode
{
public:
	BytesLineMapNode(const DolSection* section, uint32_t addr);
	bool GetSubLine(std::string& line, LineNum num) const;
	uint32_t GetAddrAtLine(LineNum num) const;
private:
	const DolSection* m_section;
	uint32_t m_addr;
};

BytesLineMapNode::BytesLineMapNode(const DolSection* section, uint32_t addr)
	: m_section(section), m_addr(addr)
{ }

bool BytesLineMapNode::GetSubLine(std::string& line, LineNum num) const
{
	line.clear();
	uint32_t lineAddr = m_addr + num;
	if (lineAddr < m_section->address + m_section->data.size())
	{
		uint8_t b = m_section->data[lineAddr - m_section->address];

		std::stringstream ss;
		ss << std::hex << std::uppercase << std::setw(8) << lineAddr << "h:  db ";
		if (b > 9)
		{
			ss << std::hex << std::uppercase << std::setw(3) << int(b) << "h  ; "
				<< char(b);
		}
		else
		{
			ss << std::setw(4) << int(b) << "  ; " << char(b);
		}

		line = ss.str();
	}

	return false;
}

uint32_t BytesLineMapNode::GetAddrAtLine(LineNum num) const
{
	return m_addr + num;
}

class GekkoLineMapNode : public LineMapNode
{
public:
	GekkoLineMapNode(ReverseToolkitFrame* frame, const DolSection* section, uint32_t addr);
	bool GetSubLine(std::string& line, LineNum num) const;
	uint32_t GetAddrAtLine(LineNum num) const;
	// TODO: CLEANUP!!!
	const DolSection* GetSection() const { return m_section; }
	void SetNodeRef(LineMap* lineMap, LineMap::ConstNodeRef nodeRef)
	{
		m_lineMap = lineMap;
		m_nodeRef = nodeRef;
	}
	LineMap* GetTree() const { return m_lineMap; }
	LineMap::ConstNodeRef GetNodeRef() const { return m_nodeRef; }
	LineNum GetLineAtAddr(uint32_t addr) const;
private:
	ReverseToolkitFrame* m_frame;
	const DolSection* m_section;
	uint32_t m_addr;
	LineMap* m_lineMap;
	LineMap::ConstNodeRef m_nodeRef;
};

GekkoLineMapNode::GekkoLineMapNode(ReverseToolkitFrame* frame, const DolSection* section, uint32_t addr)
	: m_frame(frame), m_section(section), m_addr(addr)
{ }

inline uint32_t FromBE32(uint32_t v) { return _byteswap_ulong(v); }

class GekkoLabelProvider : public LabelProvider
{
public:
	GekkoLabelProvider(ReverseToolkitFrame* frame);
	std::string GetLabel(uint32_t addr) const;
private:
	ReverseToolkitFrame* m_frame;
};

GekkoLabelProvider::GekkoLabelProvider(ReverseToolkitFrame* frame)
	: m_frame(frame)
{ }

std::string GekkoLabelProvider::GetLabel(uint32_t addr) const
{
	return m_frame->GetLabel(addr);
}

bool GekkoLineMapNode::GetSubLine(std::string& line, LineNum num) const
{
	line.clear();
	
	uint32_t lineAddr = m_addr + num * 4;
	if (lineAddr < m_section->address + m_section->data.size())
	{
		uint32_t instr = FromBE32(*(const uint32_t*)&m_section->data[m_addr+num*4 - m_section->address]);

		GekkoLabelProvider labelProv(m_frame);

		uint32_t addrRef = 0;
		std::stringstream ss;
		ss << std::hex << std::uppercase << std::setw(8) << lineAddr << "h: "
			<< DisassembleGekko(instr, lineAddr, addrRef, &labelProv);

		line = ss.str();

		// TODO: Add code labels in some other pass
		if (addrRef != 0)
		{
			if (!m_frame->HasLabel(addrRef))
			{
				std::stringstream ss;
				ss << "loc_" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << addrRef << 'h';
				m_frame->AddLabel(addrRef, ss.str());
				return true;
			}
		}
	}

	return false;
}

uint32_t GekkoLineMapNode::GetAddrAtLine(LineNum num) const
{
	return m_addr + num * 4;
}

LineNum GekkoLineMapNode::GetLineAtAddr(uint32_t addr) const
{
	return (addr - m_addr) / 4;
}

class StringAnnotMapNode : public LineMapNode
{
public:
	StringAnnotMapNode(const DolSection* section, uint32_t addr, const wxString& str);
	bool GetSubLine(std::string& line, LineNum num) const;
	uint32_t GetAddrAtLine(LineNum num) const;
private:
	const DolSection* m_section;
	uint32_t m_addr;
	wxString m_str;
};

StringAnnotMapNode::StringAnnotMapNode(const DolSection* section, uint32_t addr, const wxString& str)
	: m_section(section), m_addr(addr), m_str(str)
{ }

bool StringAnnotMapNode::GetSubLine(std::string& line, LineNum num) const
{
	// TODO: Display address on this line
	line.clear();
	if (num == 0)
		line = m_str.c_str().AsChar();
	return false;
}

uint32_t StringAnnotMapNode::GetAddrAtLine(LineNum num) const
{
	return m_addr;
}

class LabelLineMapNode : public LineMapNode
{
public:
	LabelLineMapNode(ReverseToolkitFrame* frame, uint32_t addr);
	bool GetSubLine(std::string& line, LineNum num) const;
	uint32_t GetAddrAtLine(LineNum num) const;
private:
	ReverseToolkitFrame* m_frame;
	uint32_t m_addr;
};

LabelLineMapNode::LabelLineMapNode(ReverseToolkitFrame* frame, uint32_t addr)
	: m_frame(frame), m_addr(addr)
{ }

bool LabelLineMapNode::GetSubLine(std::string& line, LineNum num) const
{
	line.clear();
	if (num == 1)
		line = m_frame->GetLabel(m_addr) + ':';
	return false;
}

uint32_t LabelLineMapNode::GetAddrAtLine(LineNum num) const
{
	return m_addr;
}

class LineMapLineProvider : public LineProvider
{
public:
	LineMapLineProvider(ReverseToolkitFrame* frame, GekkoAddressMap* gekkoMap, const DolSection* section);
	bool GetLine(std::string& line, LineNum num) const;
	void OnKey(LineNum num, wxKeyEvent& event);
private:
	ReverseToolkitFrame* m_frame;
	const DolSection* m_section;
	GekkoAddressMap* m_gekkoMap;
	LineMap m_lineMap;
};

LineMapLineProvider::LineMapLineProvider(ReverseToolkitFrame* frame, GekkoAddressMap* gekkoMap, const DolSection* section)
	: m_frame(frame), m_gekkoMap(gekkoMap), m_section(section)
{
	if (section->type == DolSection::Text)
	{
		std::shared_ptr<GekkoLineMapNode> newNode(new GekkoLineMapNode(frame, section, section->address));
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
	}
}

void ReverseToolkitFrame::LoadDol(const wxString& path)
{
	m_dolDoc.Load(path.c_str().AsChar());

	for (std::list<DolSection>::const_iterator it = m_dolDoc.GetSections().begin();
		it != m_dolDoc.GetSections().end(); ++it)
	{
		LineViewWindow* newView = new LineViewWindow(m_notebook, it->title,
			std::shared_ptr<LineMapLineProvider>(new LineMapLineProvider(this, &m_gekkoMap, &*it)));
		m_notebook->AddPage(newView, newView->GetName());
	}
}

void ReverseToolkitFrame::AddLabel(uint32_t addr, const std::string& name)
{
	LabelMap::iterator labelIt = m_labelMap.find(addr);
	if (labelIt != m_labelMap.end())
	{
		// Give existing label a new name
		labelIt->second = name;
	}
	else
	{
		// Add the label
		m_labelMap[addr] = name;

		// Find the GekkoLineMapNode containing addr
		GekkoAddressMap::ConstNodeRef mapNode = m_gekkoMap.FindFloor(addr);
		if (mapNode.IsValid())
		{
			LineMap::ConstNodeRef lineNode = mapNode.Get()->GetNodeRef();
			LineMap::Key lineNodeKey = lineNode.GetKey();
			LineNum lineNum = lineNodeKey + mapNode.Get()->GetLineAtAddr(addr);
			if (lineNum != lineNodeKey)
			{
				// Create new GekkoLineMapNode
				// FIXME: Create new bytes node if we put a label in bytes.
				std::shared_ptr<GekkoLineMapNode> newLineNode(new GekkoLineMapNode(this, mapNode.Get()->GetSection(), addr));
				LineMap::ConstNodeRef newLineNodeRef = mapNode.Get()->GetTree()->Insert(lineNum, newLineNode);
				newLineNode->SetNodeRef(mapNode.Get()->GetTree(), newLineNodeRef);
				m_gekkoMap.Insert(addr, newLineNode);
				// Push new node right
				mapNode.Get()->GetTree()->PushRight(lineNum-1, 3);
				// Insert new label node
				mapNode.Get()->GetTree()->Insert(lineNum, std::shared_ptr<LabelLineMapNode>(new LabelLineMapNode(this, addr)));

				mapNode.Get()->GetTree()->Validate();
			}
			else
			{
				// Push line node right
				mapNode.Get()->GetTree()->PushRight(lineNum-1, 3);
				// Insert new label node
				mapNode.Get()->GetTree()->Insert(lineNum, std::shared_ptr<LabelLineMapNode>(new LabelLineMapNode(this, addr)));

				mapNode.Get()->GetTree()->Validate();
			}
		}
	}
}

bool ReverseToolkitFrame::HasLabel(uint32_t addr) const
{
	LabelMap::const_iterator it = m_labelMap.find(addr);
	return it != m_labelMap.end();
}

std::string ReverseToolkitFrame::GetLabel(uint32_t addr) const
{
	LabelMap::const_iterator it = m_labelMap.find(addr);
	if (it != m_labelMap.end())
		return it->second;
	else
		return "UNKNOWN_LABEL";
}
