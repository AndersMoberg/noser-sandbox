// ReverseToolkitFrame.hpp
// Nolan Check
// Created 6/3/2011

#ifndef _REVERSETOOLKITFRAME_HPP
#define _REVERSETOOLKITFRAME_HPP

#include <map>
#include <wx/wx.h>

#include "AbsTree.hpp"
#include "DolDocument.hpp"

class GekkoLineMapNode;
class wxNotebook;

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

private:

	wxNotebook* m_notebook;
	DolDocument m_dolDoc;
	GekkoAddressMap m_gekkoMap;
	LabelMap m_labelMap;

};

#endif
