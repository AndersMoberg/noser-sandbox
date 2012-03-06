// ReverseToolkitFrame.hpp
// Nolan Check
// Created 6/3/2011

#ifndef _REVERSETOOLKITFRAME_HPP
#define _REVERSETOOLKITFRAME_HPP

#include <map>
#include <wx/wx.h>

#include "AbsTree.hpp"
#include "DolDocument.hpp"
#include "LineMap.hpp"

class wxNotebook;

class GekkoLineMapNode : public LineMapNode
{
public:
	GekkoLineMapNode(ReverseToolkitFrame* frame,
		LineViewWindow* lineViewWindow,
		const DolSection* section, uint32_t addr);
	bool GetSubLine(std::string& line, LineNum num) const;
	bool GetSubLineXRef(uint32_t& addr, LineNum num) const;
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
	LineViewWindow* GetLineViewWindow() const { return m_lineViewWindow; }
private:
	ReverseToolkitFrame* m_frame;
	LineViewWindow* m_lineViewWindow;
	const DolSection* m_section;
	uint32_t m_addr;
	LineMap* m_lineMap;
	LineMap::ConstNodeRef m_nodeRef;
};

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

typedef AbsTree<std::shared_ptr<GekkoLineMapNode> > GekkoAddressMap;
typedef std::map<uint32_t, std::string> LabelMap;
//typedef AbsTree<std::string> LabelMap;

class ReverseToolkitFrame : public wxFrame
{

public:

	ReverseToolkitFrame(const wxString& title);

	void LoadDol(const wxString& path);
	void AddLabel(uint32_t addr, const std::string& name);
	bool HasLabel(uint32_t addr) const;
	std::string GetLabel(uint32_t addr) const;
	const GekkoAddressMap& GetGekkoMap() const { return m_gekkoMap; }

	void OpenLineViewWindow(LineViewWindow* win);

private:

	wxNotebook* m_notebook;
	DolDocument m_dolDoc;
	GekkoAddressMap m_gekkoMap;
	LabelMap m_labelMap;

};

#endif
