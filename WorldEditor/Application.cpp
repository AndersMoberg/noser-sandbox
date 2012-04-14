// Application.cpp
// Nolan Check
// Created 2/12/2012

#include "Application.hpp"

#include "WorldEditorFrame.hpp"

bool Application::OnInit()
{
	if (!wxApp::OnInit()) {
		return false;
	}

	m_doc.reset(new Document);

	m_doc->addPoint(Vector2f(50.0f, 50.0f));

	WorldEditorFrame* frame = new WorldEditorFrame(m_doc.get());

	frame->Show();

	return true;
}
