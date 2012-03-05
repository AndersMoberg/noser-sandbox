// ReverseToolkitFrame.cpp
// Nolan Check
// Created 6/3/2011

#include "ReverseToolkitFrame.hpp"

#include <iomanip>
#include <sstream>
#include <wx/notebook.h>

#include "BinaryDocument.hpp"
#include "BinaryView.hpp"
#include "GekkoDisassembly.hpp"
#include "LineViewWindow.hpp"
#include "RelTree.hpp"

ReverseToolkitFrame::ReverseToolkitFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title),
	m_notebook(NULL)
{
	wxPanel* mainPanel = new wxPanel(this, 0, 0, 400, 300);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	m_notebook = new wxNotebook(mainPanel, wxID_ANY);
	sizer->Add(m_notebook, 1, wxEXPAND | wxALL);
	mainPanel->SetSizer(sizer);
}

void ReverseToolkitFrame::LoadDol(const wxString& path)
{
	m_dolDoc.Load(path.c_str().AsChar());
	// TODO: Reimplement
}

void ReverseToolkitFrame::LoadBinary(const wxString& path)
{
	m_binDoc.reset(new BinaryDocument());
	m_binDoc->Load(path.c_str().AsChar());
	m_binView.reset(new BinaryView(m_binDoc));

	LineViewWindow* newView = new LineViewWindow(m_notebook, "Binary", m_binView);
	m_notebook->AddPage(newView, newView->GetName());
}
