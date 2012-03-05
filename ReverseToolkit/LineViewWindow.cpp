// LineViewWindow.cpp
// Nolan Check
// Created 6/3/2011

#include "LineViewWindow.hpp"

#include <sstream>
#include <wx/dcbuffer.h>

LineViewWindow::LineViewWindow(wxWindow* parent, const wxString& name, std::shared_ptr<LineProvider> provider)
	: wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
	wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS, name),
	m_font(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas"),
	m_scrollPos(0.0), m_selectedLine(0),
	m_provider(provider)
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetBackgroundColour(wxColour(255, 255, 255));

	Connect(wxEVT_PAINT, wxPaintEventHandler(LineViewWindow::OnPaint));
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(LineViewWindow::OnLeftClick));
	Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(LineViewWindow::OnMouseWheel));
	Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(LineViewWindow::OnKey));
}

void LineViewWindow::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);

	bool complete = false;
	while (!complete)
	{
		dc.Clear();

		dc.SetFont(m_font);

		int fontHeight = m_font.GetPixelSize().GetY();
		int numLines = GetSize().GetHeight() / fontHeight;
		complete = true;
		for (LineNum i = floor(m_scrollPos); i < ceil(m_scrollPos + numLines) + 1; ++i)
		{
			std::string line;
			if (m_provider->GetLine(line, i))
			{
				complete = false;
				break;
			}

			if (i == m_selectedLine)
				dc.DrawRectangle(0, fontHeight * (i - m_scrollPos), GetSize().GetWidth(), fontHeight);

			dc.DrawText(line, 0, fontHeight * (i - m_scrollPos));
		}
	}
}

void LineViewWindow::OnLeftClick(wxMouseEvent& event)
{
	int fontHeight = m_font.GetPixelSize().GetY();
	long clickedLine = double(event.GetY()) / fontHeight + m_scrollPos;
	m_selectedLine = clickedLine;
	Refresh();

	event.Skip();
}

void LineViewWindow::OnMouseWheel(wxMouseEvent& event)
{
	m_scrollPos -= double(event.GetWheelRotation()) / event.GetWheelDelta();
	Refresh();
}

void LineViewWindow::OnKey(wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
	case WXK_PAGEUP: // Go up a bunch of lines
		// TODO: Scroll up one page's worth of lines
		if (event.ShiftDown())
			m_scrollPos -= 1024.0;
		else
			m_scrollPos -= 128.0;
		Refresh();
		break;
	case WXK_PAGEDOWN: // Go down a bunch of lines
		// TODO: Scroll down one page's worth of lines
		if (event.ShiftDown())
			m_scrollPos += 1024.0;
		else
			m_scrollPos += 128.0;
		Refresh();
		break;
	case WXK_UP: // Move cursor up one line
		--m_selectedLine;
		// TODO: Adjust scrollPos intelligently
		m_scrollPos = m_selectedLine - 8.0;
		Refresh();
		break;
	case WXK_DOWN: // Move cursor down one line
		++m_selectedLine;
		// TODO: Adjust scrollPos intelligently
		m_scrollPos = m_selectedLine - 8.0;
		Refresh();
		break;
	default:
		m_provider->OnKey(m_selectedLine, event);
		Refresh();
		break;
	}
}
