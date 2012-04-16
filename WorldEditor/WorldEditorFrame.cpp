// WorldEditorFrame.cpp
// Nolan Check
// Created 2/12/2012

#include "WorldEditorFrame.hpp"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

WorldEditorFrame::WorldEditorFrame(Document* doc)
	: wxFrame(NULL, wxID_ANY, wxT("World Editor")),
	m_tool(TOOL_ADDPOINT),
	m_doc(doc)
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetBackgroundColour(wxColour(255, 255, 255));

	Connect(wxEVT_PAINT, wxPaintEventHandler(WorldEditorFrame::OnPaint));
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(WorldEditorFrame::OnLeftDown));
}

void WorldEditorFrame::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);

	dc.Clear();

	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

	const std::vector<Vector2f>& points = m_doc->getPoints();
	for (std::vector<Vector2f>::const_iterator it = points.begin();
		it != points.end(); ++it)
	{
		gc->SetPen(*wxBLACK_PEN);
		gc->DrawEllipse(it->x - 10.0f, it->y - 10.0f, 20.0f, 20.0f);
	}
}

void WorldEditorFrame::OnLeftDown(wxMouseEvent& event)
{
	switch (m_tool)
	{
	case TOOL_ADDPOINT:
		m_doc->addPoint(Vector2f(event.GetPosition().x, event.GetPosition().y));
		Refresh();
		break;
	case TOOL_MOVEPOINT:
		break;
	}
}
