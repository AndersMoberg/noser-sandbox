// WorldEditorFrame.cpp
// Nolan Check
// Created 2/12/2012

#include "WorldEditorFrame.hpp"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

WorldEditorFrame::WorldEditorFrame(Document* doc)
	: wxFrame(NULL, wxID_ANY, wxT("World Editor")),
	m_tool(TOOL_ADDPOINT),
	m_draggingPoint(NULL),
	m_doc(doc)
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetBackgroundColour(wxColour(255, 255, 255));

	Connect(wxEVT_PAINT, wxPaintEventHandler(WorldEditorFrame::OnPaint));
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(WorldEditorFrame::OnLeftDown));
	Connect(wxEVT_MOTION, wxMouseEventHandler(WorldEditorFrame::OnMouseMove));
	Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(WorldEditorFrame::OnKeyDown));
}

void WorldEditorFrame::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);

	dc.Clear();

	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
	
	gc->SetPen(*wxBLACK_PEN);

	const Document::WallList& walls = m_doc->getWalls();
	for (Document::WallList::const_iterator it = walls.begin();
		it != walls.end(); ++it)
	{
		wxPoint2DDouble pts[2] = { it->getPoint(false)->getPosition(),
			it->getPoint(true)->getPosition() };
		gc->DrawLines(2, pts);
	}

	const Document::PointList& points = m_doc->getPoints();
	for (Document::PointList::const_iterator it = points.begin();
		it != points.end(); ++it)
	{
		const Vector2f& pos = it->getPosition();
		gc->DrawEllipse(pos.x - 4.0f, pos.y - 4.0f, 8.0f, 8.0f);
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
		m_draggingPoint = m_doc->findPoint(
			Vector2f(event.GetPosition().x, event.GetPosition().y),
			10.0f);
		break;
	case TOOL_WALL:
		Point* pt = m_doc->findPoint(
			Vector2f(event.GetPosition().x, event.GetPosition().y),
			10.0f);
		if (pt != NULL) {
			m_wallTool.onClickPoint(pt);
			Refresh();
		}
		break;
	}
}

void WorldEditorFrame::OnMouseMove(wxMouseEvent& event)
{
	if (m_tool == TOOL_MOVEPOINT && m_draggingPoint != NULL)
	{
		Vector2f newPos(event.GetPosition().x, event.GetPosition().y);
		m_draggingPoint->setPosition(newPos);
		Refresh();
	}
}

void WorldEditorFrame::OnKeyDown(wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
	case 'A':
		m_tool = TOOL_ADDPOINT;
		break;
	case 'M':
		m_tool = TOOL_MOVEPOINT;
		break;
	case 'W':
		m_tool = TOOL_WALL;
		m_wallTool.start(m_doc);
		break;
	}
}
