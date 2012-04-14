// Application.hpp
// Nolan Check
// Created 2/12/2012

#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP

#include <memory>

#include <wx/wx.h>

#include "Document.hpp"

class Application : public wxApp
{

public:

	virtual bool OnInit();

private:

	std::unique_ptr<Document> m_doc;

};

wxDECLARE_APP(Application);

#endif
