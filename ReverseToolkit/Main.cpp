// Main.cpp
// Nolan Check
// Created 6/3/2011

#include "ReverseToolkitFrame.hpp"

class ReverseToolkitApp : public wxApp
{

public:

	bool OnInit();

};

bool ReverseToolkitApp::OnInit()
{
	wxFileDialog fileDlg(NULL);
	if (fileDlg.ShowModal() == wxID_OK)
	{
		ReverseToolkitFrame* frame = new ReverseToolkitFrame("Reverse Toolkit");
		frame->LoadDol(fileDlg.GetPath());
		frame->Show();
		SetTopWindow(frame);

		return true;
	}
	else
		return false;
}

wxIMPLEMENT_APP(ReverseToolkitApp);
