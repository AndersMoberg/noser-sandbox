// WallTool.hpp
// Nolan Check
// Created 4/21/2012

#ifndef _WALLTOOL_HPP
#define _WALLTOOL_HPP

#include "Document.hpp"
#include "Geometry.hpp"

class WallTool
{

public:

	void start(Document* doc);

	void onClickPoint(Point* pt);

private:

	Document* m_doc;

	Point* m_pt1;
	Point* m_pt2;

	enum State
	{
		STATE_SELECT1,
		STATE_SELECT2
	};
	State m_state;

};

#endif
