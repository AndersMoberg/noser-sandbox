// ReverseToolkitFrame.cpp
// Nolan Check
// Created 6/3/2011

#include "ReverseToolkitFrame.hpp"

#include <iomanip>
#include <sstream>
#include <wx/notebook.h>

#include "GekkoDisassembly.hpp"
#include "LineMap.hpp"
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

GekkoLineMapNode::GekkoLineMapNode(ReverseToolkitFrame* frame,
	LineViewWindow* lineViewWindow, const DolSection* section, uint32_t addr)
	: m_frame(frame), m_lineViewWindow(lineViewWindow), m_section(section), m_addr(addr)
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
	
	long long lineAddr = m_addr + num * 4;
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

bool GekkoLineMapNode::GetSubLineXRef(uint32_t& addr, LineNum num) const
{
	addr = 0;

	long long lineAddr = m_addr + num * 4;
	if (lineAddr < m_section->address + m_section->data.size())
	{
		uint32_t instr = FromBE32(*(const uint32_t*)&m_section->data[m_addr+num*4 - m_section->address]);

		GekkoLabelProvider labelProv(m_frame);

		std::stringstream ss;
		ss << std::hex << std::uppercase << std::setw(8) << lineAddr << "h: "
			<< DisassembleGekko(instr, lineAddr, addr, &labelProv);

		if (addr != 0) {
			return true;
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

void ReverseToolkitFrame::LoadDol(const wxString& path)
{
	m_dolDoc.Load(path.c_str().AsChar());

	size_t page = 0;
	for (std::list<DolSection>::const_iterator it = m_dolDoc.GetSections().begin();
		it != m_dolDoc.GetSections().end(); ++it)
	{
		LineViewWindow* newView = new LineViewWindow(m_notebook, it->title, page);
		LineMapLineProviderPtr newProvider(new LineMapLineProvider(this, newView, &m_gekkoMap, &*it));
		newView->SetLineProvider(newProvider);
		
		m_notebook->AddPage(newView, newView->GetName());
		++page;
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
				std::shared_ptr<GekkoLineMapNode> newLineNode(new GekkoLineMapNode(this,
					mapNode.Get()->GetLineViewWindow(), mapNode.Get()->GetSection(), addr));
				LineMap::ConstNodeRef newLineNodeRef = mapNode.Get()->GetTree()->Insert(lineNum, newLineNode);
				newLineNode->SetNodeRef(mapNode.Get()->GetTree(), newLineNodeRef);
				m_gekkoMap.Insert(addr, newLineNode);
				// Push new node right
				mapNode.Get()->GetTree()->PushRightInclusive(lineNum, 3);
				// Insert new label node
				mapNode.Get()->GetTree()->Insert(lineNum, std::shared_ptr<LabelLineMapNode>(new LabelLineMapNode(this, addr)));

				mapNode.Get()->GetTree()->Validate();
			}
			else
			{
				// Push line node right
				mapNode.Get()->GetTree()->PushRightInclusive(lineNum, 3);
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

void ReverseToolkitFrame::OpenLineViewWindow(LineViewWindow* win)
{
	m_notebook->SetSelection(win->GetPage());
}

void ReverseToolkitFrame::GoToAddress(uint32_t addr)
{
	// XXX: Note that this often doesn't go to the expected line because new
	// labels may be created when the new lines come into view.

	// Find the GekkoLineMapNode containing addr
	GekkoAddressMap::ConstNodeRef mapNode = m_gekkoMap.FindFloor(addr);
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
			OpenLineViewWindow(lineViewWin);
		}
	}
}
