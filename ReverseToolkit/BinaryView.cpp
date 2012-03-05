// BinaryView.cpp
// Nolan Check
// Created 6/29/2011

#include "BinaryView.hpp"

#include <iomanip>
#include <map>
#include <sstream>

#include "AbsTree.hpp"
#include "BinaryDocument.hpp"

class LineMapNode
{
public:
	virtual ~LineMapNode() { }
	virtual bool GetSubLine(std::string& line, LineNum num) const = 0;
	virtual long long GetAddrAtLine(LineNum num) const = 0;
};

class BytesLineMapNode : public LineMapNode
{
public:
	BytesLineMapNode(std::shared_ptr<BinaryDocument> doc, long long addr);
	bool GetSubLine(std::string& line, LineNum num) const;
	long long GetAddrAtLine(LineNum num) const;
private:
	std::shared_ptr<BinaryDocument> m_doc;
	long long m_addr;
};

BytesLineMapNode::BytesLineMapNode(std::shared_ptr<BinaryDocument> doc, long long addr)
	: m_doc(doc), m_addr(addr)
{ }

bool BytesLineMapNode::GetSubLine(std::string& line, LineNum num) const
{
	line.clear();
	long long lineAddr = m_addr + num;
	if (lineAddr < m_doc->GetData().size())
	{
		uint8_t b = m_doc->GetData()[lineAddr];

		std::stringstream ss;
		ss << std::hex << std::uppercase << std::setw(8) << lineAddr << "h:  db ";
		if (b > 9)
		{
			ss << std::hex << std::uppercase << std::setw(3) << int(b) << "h  ; "
				<< char(b);
		}
		else
		{
			ss << std::setw(4) << int(b) << "  ; " << char(b);
		}

		line = ss.str();
	}

	return false;
}

long long BytesLineMapNode::GetAddrAtLine(LineNum num) const
{
	return m_addr + num;
}

class StringLineMapNode : public LineMapNode
{
public:
	StringLineMapNode(std::shared_ptr<BinaryDocument> doc, long long addr, size_t len);
	bool GetSubLine(std::string& line, LineNum num) const;
	long long GetAddrAtLine(LineNum num) const;
private:
	std::shared_ptr<BinaryDocument> m_doc;
	long long m_addr;
	size_t m_len;
};

StringLineMapNode::StringLineMapNode(std::shared_ptr<BinaryDocument> doc, long long addr, size_t len)
	: m_doc(doc), m_addr(addr), m_len(len)
{ }

bool StringLineMapNode::GetSubLine(std::string& line, LineNum num) const
{
	line.clear();
	if (num == 0)
	{
		const char* data = (const char*)&m_doc->GetData()[0];
		line = std::string(&data[m_addr], m_len);
	}

	return false;
}

long long StringLineMapNode::GetAddrAtLine(LineNum num) const
{
	return m_addr;
}

BinaryView::BinaryView(std::shared_ptr<BinaryDocument> doc)
	: m_doc(doc)
{
	ConstructLineMap();
}

bool BinaryView::GetLine(std::string& line, LineNum num) const
{
	line.clear();

	LineMap::KeyConstNodeRefPair keyAndNode = m_lineMap.FindFloor(num);

	if (keyAndNode.second.IsValid())
		return keyAndNode.second.Get()->GetSubLine(line, num - keyAndNode.first);
	else
		return false;
}

void BinaryView::OnKey(LineNum num, wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
	case 'A':
	{
		LineMap::KeyNodeRefPair found = m_lineMap.FindFloor(num);
		if (found.second.IsValid())
		{
			LineNum line = found.first;
			std::shared_ptr<LineMapNode> node = found.second.Get();
			LineNum subLine = num - line;
			long long addr = node->GetAddrAtLine(subLine);
			const uint8_t* src = &m_doc->GetData()[addr];

			// Find string length
			size_t len = 0;
			while (src[len])
				++len;

			m_doc->AddStringAnnot(addr, len);
			ConstructLineMap();
		}
		break;
	}
	}
}

void BinaryView::ConstructLineMap()
{
	m_lineMap.Clear();

	// Make a map of every string annotation keyed on address.
	std::map<long long, StringAnnotation> stringAnnotMap;
	const StringAnnotList& sal = m_doc->GetStringAnnots();
	for (StringAnnotList::const_iterator it = sal.begin(); it != sal.end(); ++it)
	{
		stringAnnotMap.insert(std::make_pair(it->addr, *it));
	}

	// Construct line node list.
	long long curLine = 0;
	long long curAddr = 0;
	std::map<long long, StringAnnotation>::const_iterator annotIt = stringAnnotMap.begin();
	while (curAddr < m_doc->GetData().size())
	{
		long long nextAddr = m_doc->GetData().size();
		long long toAddr = nextAddr;
		if (annotIt != stringAnnotMap.end())
		{
			toAddr = annotIt->first;
			assert(toAddr >= curAddr);
			nextAddr = toAddr + annotIt->second.len;
		}
		
		// First, create the bytes node if there is any.
		long long bytesLen = toAddr - curAddr;
		assert(bytesLen >= 0);
		if (bytesLen > 0)
		{
			std::shared_ptr<BytesLineMapNode> newNode(new BytesLineMapNode(m_doc, curAddr));
			m_lineMap.Insert(curLine, newNode);
			// Bytes nodes take up a number of lines equal to the number of bytes.
			curLine += bytesLen;
		}

		// Then, create the annotation node if there is any.
		if (annotIt != stringAnnotMap.end())
		{
			std::shared_ptr<StringLineMapNode> newNode(new StringLineMapNode(m_doc,
				annotIt->second.addr, annotIt->second.len));
			m_lineMap.Insert(curLine, newNode);

			// String annots take up one line.
			++curLine;

			++annotIt;
		}

		curAddr = nextAddr;
	}
}
