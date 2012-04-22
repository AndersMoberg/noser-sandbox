// WorldEditorFrame.hpp
// Nolan Check
// Created 2/12/2012

#ifndef _WORLDEDITORFRAME_HPP
#define _WORLDEDITORFRAME_HPP

#include <wx/wx.h>

#include "Document.hpp"
#include "WallTool.hpp"

class WorldEditorFrame : public wxFrame
{

public:

	WorldEditorFrame(Document* doc);

	void OnPaint(wxPaintEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnKeyDown(wxKeyEvent& event);

private:

	enum Tool
	{
		TOOL_ADDPOINT,
		TOOL_MOVEPOINT,
		TOOL_WALL
	};

	void findClosestPoint(const Vector2f& pt);

	Tool m_tool;
	Point* m_draggingPoint;

	Document* m_doc;

	WallTool m_wallTool;

};

#endif
