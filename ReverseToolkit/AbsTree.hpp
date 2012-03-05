// AbsTree.hpp
// Nolan Check
// Created 6/4/2011

#ifndef _ABSTREE_HPP
#define _ABSTREE_HPP

template<class T>
class AbsTree
{

public:

	typedef long long Key;

private:

	struct Node
	{
		Node(Key _key, Node* _parent, const T& _data);
		~Node();

		Node* GetLink(bool dir);
		void SetLink(bool dir, Node* newLink);

		Key key;
		bool red; // true is red, false is black
		Node* parent;
		Node* link[2]; // left, right
		T data;
	};

public:

	class ConstNodeRef
	{
	friend class AbsTree<T>;
	public:
		bool IsValid() const { return !!m_node; }
		const T& Get() const { return m_node->data; }
	private:
		ConstNodeRef(const Node* node)
			: m_node(node)
		{ }
		const Node* m_node;
	};

	AbsTree();
	~AbsTree();

	// TODO: Insert should return a NodeRef to the newly-inserted node.
	void Insert(Key k, const T& data);
	ConstNodeRef FindFloor(Key k) const;

private:

	static Node* RotateSingle(Node* node, bool dir);
	static Node* RotateDouble(Node* node, bool dir);
	static bool IsRed(Node* node);

	Node* InsertVisit(Node* parent, Node* node, Key k, const T& data);
	const Node* FindFloorVisit(const Node* parent, const Node* node, Key k) const;

	Node* m_root;

};

template<class T>
AbsTree<T>::Node::Node(Key _key, Node* _parent, const T& _data)
	: key(_key), red(true), parent(_parent), data(_data)
{
	link[0] = NULL;
	link[1] = NULL;
}

template<class T>
AbsTree<T>::Node::~Node()
{
	delete link[0];
	link[0] = NULL;
	delete link[1];
	link[1] = NULL;
}

template<class T>
typename AbsTree<T>::Node*
AbsTree<T>::Node::GetLink(bool dir)
{
	return link[dir ? 1 : 0];
}

template<class T>
void AbsTree<T>::Node::SetLink(bool dir, Node* newLink)
{
	link[dir ? 1 : 0] = newLink;
	if (newLink)
		newLink->parent = this;
}

template<class T>
AbsTree<T>::AbsTree()
	: m_root(NULL)
{ }

template<class T>
AbsTree<T>::~AbsTree()
{
	delete m_root;
	m_root = NULL;
}

template<class T>
void AbsTree<T>::Insert(Key k, const T& data)
{
	m_root = InsertVisit(NULL, m_root, k, data);
	m_root->red = false;
	m_root->parent = NULL;
}

template<class T>
typename AbsTree<T>::Node*
AbsTree<T>::RotateSingle(Node* node, bool dir)
{
	assert(node);
	Node* save = node->GetLink(!dir);
	assert(save);

	node->SetLink(!dir, save->GetLink(dir));
	save->SetLink(dir, node);

	node->red = true;
	save->red = false;

	return save;
}

template<class T>
typename AbsTree<T>::Node*
AbsTree<T>::RotateDouble(Node* node, bool dir)
{
	node->SetLink(!dir, RotateSingle(node->GetLink(!dir), !dir));
	return RotateSingle(node, dir);
}

template<class T>
bool AbsTree<T>::IsRed(Node* node)
{
	return node && node->red;
}

// Ref: <http://eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx>
template<class T>
typename AbsTree<T>::Node*
AbsTree<T>::InsertVisit(Node* parent, Node* node, Key k, const T& data)
{
	if (!node)
	{
		node = new Node(k, parent, data);
	}
	else
	{
		if (k != node->key)
		{
			bool dir = k > node->key;
			node->SetLink( dir, InsertVisit(node, node->GetLink(dir), k, data) );

			// Rebalance red-black tree
			if ( IsRed(node->GetLink(dir)) )
			{
				if ( IsRed(node->GetLink(!dir)) )
				{
					// Case 1
					node->red = true;
					node->link[0]->red = false;
					node->link[1]->red = false;
				}
				else
				{
					// Cases 2 & 3
					if ( IsRed(node->GetLink(dir)->GetLink(dir)) )
						node = RotateSingle(node, !dir);
					else if ( IsRed(node->GetLink(dir)->GetLink(!dir)) )
						node = RotateDouble(node, !dir);
				}
			}
		}
	}

	return node;
}

template<class T>
typename AbsTree<T>::ConstNodeRef
AbsTree<T>::FindFloor(Key k) const
{
	if (!m_root)
		return ConstNodeRef(NULL);
	else
	{
		const Node* result = FindFloorVisit(NULL, m_root, k);
		return ConstNodeRef(result);
	}
}

template<class T>
typename const AbsTree<T>::Node*
AbsTree<T>::FindFloorVisit(const Node* parent, const Node* node, Key k) const
{
	if (k < node->key)
	{
		if (!node->link[0])
			return NULL;
		else
			return FindFloorVisit(node, node->link[0], k);
	}
	else if (k == node->key)
		return node;
	else // k > nodeKey
	{
		if (!node->link[1])
			return node;
		else
		{
			const Node* rightFloor = FindFloorVisit(node, node->link[1], k);
			if (!rightFloor)
				return node;
			else
				return rightFloor;
		}
	}
}

#endif
