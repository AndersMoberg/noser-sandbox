// WorldEditorFrame.hpp
// Nolan Check
// Created 2/12/2012

#ifndef _WORLDEDITORFRAME_HPP
#define _WORLDEDITORFRAME_HPP

#include <wx/wx.h>

#include "Document.hpp"

class WorldEditorFrame : public wxFrame
{

public:

	WorldEditorFrame(Document* doc);

	void OnPaint(wxPaintEvent& event);
	void OnLeftDown(wxMouseEvent& event);

private:

	enum Tool
	{
		TOOL_ADDPOINT,
		TOOL_MOVEPOINT
	};

	void findClosestPoint(const Vector2f& pt);

	Tool m_tool;

	Document* m_doc;

};

#endif
