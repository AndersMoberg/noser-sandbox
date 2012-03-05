// BinaryView.hpp
// Nolan Check
// Created 6/29/2011

#ifndef _BINARYVIEW_HPP
#define _BINARYVIEW_HPP

#include "LineViewWindow.hpp"
#include "RelTree.hpp"

class BinaryDocument;

class LineMapNode;

typedef RelTree<std::shared_ptr<LineMapNode> > LineMap;

class BinaryView : public LineProvider
{

public:

	BinaryView(std::shared_ptr<BinaryDocument> doc);

	// Inherited from LineProvider
	bool GetLine(std::string& line, LineNum num) const;
	void OnKey(LineNum num, wxKeyEvent& event);

private:

	void ConstructLineMap();

	std::shared_ptr<BinaryDocument> m_doc;
	LineMap m_lineMap;

};

#endif
