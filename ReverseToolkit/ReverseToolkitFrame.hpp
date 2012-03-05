// ReverseToolkitFrame.hpp
// Nolan Check
// Created 6/3/2011

#ifndef _REVERSETOOLKITFRAME_HPP
#define _REVERSETOOLKITFRAME_HPP

#include <map>
#include <wx/wx.h>

#include "AbsTree.hpp"
#include "DolDocument.hpp"

class wxNotebook;

class BinaryDocument;
class BinaryView;

class ReverseToolkitFrame : public wxFrame
{

public:

	ReverseToolkitFrame(const wxString& title);

	void LoadDol(const wxString& path);
	void LoadBinary(const wxString& path);

private:

	wxNotebook* m_notebook;

	DolDocument m_dolDoc;
	std::shared_ptr<BinaryDocument> m_binDoc;
	std::shared_ptr<BinaryView> m_binView;

};

#endif
