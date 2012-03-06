// LineMap.hpp
// Nolan Check
// Created 3/5/2012

#ifndef _LINEMAP_HPP
#define _LINEMAP_HPP

#include <cstdint>

#include "AbsTree.hpp"
#include "LineViewWindow.hpp"
#include "RelTree.hpp"

class LineMapNode;
typedef std::shared_ptr<LineMapNode> LineMapNodePtr;

class LineMapNode
{
public:
	virtual ~LineMapNode() { }
	virtual bool GetSubLine(std::string& line, LineNum num) const = 0;
	virtual bool GetSubLineXRef(uint32_t& addr, LineNum num) const
	{
		addr = 0;
		return false;
	}
	virtual uint32_t GetAddrAtLine(LineNum num) const = 0;
};

typedef RelTree<LineMapNodePtr> LineMap;

class ReverseToolkitFrame;
class GekkoLineMapNode;
typedef AbsTree<std::shared_ptr<GekkoLineMapNode> > GekkoAddressMap;
struct DolSection;

class LineMapLineProvider;
typedef std::shared_ptr<LineMapLineProvider> LineMapLineProviderPtr;

class LineMapLineProvider : public LineProvider
{
public:
	LineMapLineProvider(ReverseToolkitFrame* frame,
		LineViewWindow* lineViewWindow, GekkoAddressMap* gekkoMap, const DolSection* section);
	bool GetLine(std::string& line, LineNum num) const;
	void OnKey(LineNum num, wxKeyEvent& event);

	void GoToAddress(uint32_t addr);
private:
	ReverseToolkitFrame* m_frame;
	const DolSection* m_section;
	GekkoAddressMap* m_gekkoMap;
	LineMap m_lineMap;
};

#endif
