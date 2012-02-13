// Application.hpp
// Nolan Check
// Created 2/12/2012

#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP

#include <wx/wx.h>

class Application : public wxApp
{
public:
	virtual bool OnInit();
};

wxDECLARE_APP(Application);

#endif
