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

	WorldEditorFrame* frame = new WorldEditorFrame;

	frame->Show();

	return true;
}
