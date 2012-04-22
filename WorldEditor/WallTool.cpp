// WallTool.cpp
// Nolan Check
// Created 4/21/2012

#include "WallTool.hpp"

void WallTool::start(Document* doc)
{
	m_doc = doc;
	m_pt1 = NULL;
	m_pt2 = NULL;
	m_state = STATE_SELECT1;
}

void WallTool::onClickPoint(Point* pt)
{
	if (m_state == STATE_SELECT1) {
		m_pt1 = pt;
		m_state = STATE_SELECT2;
	} else if (m_state == STATE_SELECT2) {
		m_pt2 = pt;
		m_doc->addWall(m_pt1, m_pt2);
		m_state = STATE_SELECT1;
	}
}
