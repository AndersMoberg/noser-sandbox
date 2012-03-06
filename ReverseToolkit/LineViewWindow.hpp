// LineViewWindow.hpp
// Nolan Check
// Created 6/3/2011

#ifndef _LINEVIEWWINDOW_HPP
#define _LINEVIEWWINDOW_HPP

#include <memory>
#include <wx/wx.h>

typedef long long LineNum;

class LineProvider;
typedef std::shared_ptr<LineProvider> LineProviderPtr;

class LineProvider
{
public:
	virtual ~LineProvider() { };
	virtual bool GetLine(std::string& line, LineNum num) const = 0;
	virtual void OnKey(LineNum num, wxKeyEvent& event) = 0;
};

class LineViewWindow : public wxWindow
{

public:

	LineViewWindow(wxWindow* parent, const wxString& name, LineProviderPtr provider);

	void OnPaint(wxPaintEvent& event);
	void OnLeftClick(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnKey(wxKeyEvent& event);

private:

	wxFont m_font;
	double m_scrollPos;
	LineNum m_selectedLine;
	LineProviderPtr m_provider;

};

#endif
