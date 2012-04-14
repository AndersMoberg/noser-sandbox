// WorldEditorFrame.cpp
// Nolan Check
// Created 2/12/2012

#include "WorldEditorFrame.hpp"

#include <wx/dcbuffer.h>

WorldEditorFrame::WorldEditorFrame(Document* doc)
	: wxFrame(NULL, wxID_ANY, wxT("World Editor")),
	m_doc(doc)
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetBackgroundColour(wxColour(255, 255, 255));

	Connect(wxEVT_PAINT, wxPaintEventHandler(WorldEditorFrame::OnPaint));
}

void WorldEditorFrame::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);

	dc.Clear();

	const std::vector<Vector2f>& points = m_doc->getPoints();
	for (std::vector<Vector2f>::const_iterator it = points.begin();
		it != points.end(); ++it)
	{
		dc.DrawPoint(it->x, it->y);
	}
}
