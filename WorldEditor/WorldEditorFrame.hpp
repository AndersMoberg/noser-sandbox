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

	virtual void OnPaint(wxPaintEvent& event);

private:

	Document* m_doc;

};

#endif
