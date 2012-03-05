// RelTree.hpp
// Nolan Check
// Created 6/4/2011

#ifndef _RELTREE_HPP
#define _RELTREE_HPP

template<class T>
class RelTree
{

public:

	typedef long long Key;

private:

	struct Node
	{
		Node(Key _rel, Node* _parent, const T& _data);
		~Node();

		Node* GetLink(bool dir);
		void SetLink(bool dir, Node* newLink);

		Key rel;
		bool red; // true is red, false is black
		Node* parent;
		Node* link[2]; // left, right
		T data;
	};

public:

	class NodeRef
	{
	friend class RelTree<T>;
	public:
		bool IsValid() const { return !!m_node; }
		T& Get() const { return m_node->data; }
		Key GetKey() const;
	private:
		NodeRef(Node* node)
			: m_node(node)
		{ }
		Node* m_node;
	};
	
	class ConstNodeRef
	{
	friend class RelTree<T>;
	public:
		ConstNodeRef()
			: m_node(NULL)
		{ }
		ConstNodeRef(NodeRef nr)
			: m_node(nr.m_node)
		{ }
		bool IsValid() const { return !!m_node; }
		const T& Get() const { return m_node->data; }
		Key GetKey() const;
	private:
		ConstNodeRef(const Node* node)
			: m_node(node)
		{ }
		const Node* m_node;
	};

	typedef std::pair<Key, NodeRef> KeyNodeRefPair;
	typedef std::pair<Key, ConstNodeRef> KeyConstNodeRefPair;

	RelTree();
	~RelTree();

	void Clear();
	NodeRef Insert(Key k, const T& data);
	void PushRight(Key k, Key s);
	KeyNodeRefPair FindFloor(Key k);
	KeyConstNodeRefPair FindFloor(Key k) const;
	bool Validate() const;
	void SetDistanceToNext(NodeRef nr, Key s);

private:

	static Node* RotateSingle(Node* node, bool dir);
	static Node* RotateDouble(Node* node, bool dir);
	static bool IsRed(const Node* node);

	Node* InsertVisit(Key parentKey, Node* parent, Node* node, Key k, const T& data, Node*& found);
	void PushRightVisit(Key parentKey, Node* node, Key k, Key s);
	std::pair<Key, Node*> FindFloorVisit(Key parentKey, Node* parent, Node* node, Key k);
	std::pair<Key, const Node*> FindFloorVisit(Key parentKey, const Node* parent, const Node* node, Key k) const;
	std::pair<Key, Node*> FindMinVisit(Key parentKey, Node* node);
	int ValidateVisit(const Node* node) const;

	Node* m_root;

};

template<class T>
RelTree<T>::Node::Node(Key _rel, Node* _parent, const T& _data)
	: rel(_rel), red(true), parent(_parent), data(_data)
{
	link[0] = NULL;
	link[1] = NULL;
}

template<class T>
RelTree<T>::Node::~Node()
{
	delete link[0];
	link[0] = NULL;
	delete link[1];
	link[1] = NULL;
}

template<class T>
typename RelTree<T>::Node*
RelTree<T>::Node::GetLink(bool dir)
{
	return link[dir ? 1 : 0];
}

template<class T>
void RelTree<T>::Node::SetLink(bool dir, Node* newLink)
{
	link[dir ? 1 : 0] = newLink;
	if (newLink)
		newLink->parent = this;
}

template<class T>
typename RelTree<T>::Key
RelTree<T>::NodeRef::GetKey() const
{
	// Just use the const version
	return ConstNodeRef(m_node).GetKey();
}

template<class T>
typename RelTree<T>::Key
RelTree<T>::ConstNodeRef::GetKey() const
{
	// Work our way up to the root to find the absolute key
	const Node* trace = m_node;
	Key traceRel = 0;
	while (trace)
	{
		traceRel += trace->rel;
		trace = trace->parent;
	}
	return traceRel;
}

template<class T>
RelTree<T>::RelTree()
	: m_root(NULL)
{ }

template<class T>
RelTree<T>::~RelTree()
{
	Clear();
}

template<class T>
void RelTree<T>::Clear()
{
	delete m_root;
	m_root = NULL;
}

template<class T>
typename RelTree<T>::NodeRef
RelTree<T>::Insert(Key k, const T& data)
{
	Node* found = NULL;
	m_root = InsertVisit(0, NULL, m_root, k, data, found);
	assert(found);
	m_root->red = false;
	m_root->parent = NULL;
	return NodeRef(found);
}

template<class T>
typename RelTree<T>::Node*
RelTree<T>::RotateSingle(Node* node, bool dir)
{
	assert(node);
	Node* save = node->GetLink(!dir);
	assert(save);

	Node* oldParent = node->parent;

	node->SetLink(!dir, save->GetLink(dir));
	save->SetLink(dir, node);

	// Adjust relative keys
	Key oldSaveRel = save->rel;
	save->rel += node->rel;
	node->rel = -oldSaveRel;
	if (save->GetLink(dir)->GetLink(!dir))
		save->GetLink(dir)->GetLink(!dir)->rel += oldSaveRel;

	save->parent = oldParent;

	node->red = true;
	save->red = false;

	return save;
}

template<class T>
typename RelTree<T>::Node*
RelTree<T>::RotateDouble(Node* node, bool dir)
{
	node->SetLink(!dir, RotateSingle(node->GetLink(!dir), !dir));
	return RotateSingle(node, dir);
}

template<class T>
bool RelTree<T>::IsRed(const Node* node)
{
	return node && node->red;
}

// Ref: <http://eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx>
template<class T>
typename RelTree<T>::Node*
RelTree<T>::InsertVisit(Key parentKey, Node* parent, Node* node, Key k, const T& data, Node*& found)
{
	if (!node)
	{
		node = new Node(k - parentKey, parent, data);
		found = node;
	}
	else
	{
		Key nodeKey = parentKey + node->rel;
		if (nodeKey != k)
		{
			bool dir = k > nodeKey;
			node->SetLink( dir, InsertVisit(nodeKey, node, node->GetLink(dir), k, data, found) );

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
		else // nodeKey == k
			found = node;
	}

	return node;
}

// NOTE: This pushes right INCLUDING k!
template<class T>
void RelTree<T>::PushRight(Key k, Key s)
{
	if (m_root)
	{
		Key rootKey = m_root->rel;
		if (k < rootKey)
		{
			m_root->rel += s;
			if (m_root->link[0])
				PushRightVisit(rootKey, m_root->link[0], k, s);
		}
		else if (k == rootKey)
		{
			// Node exists
			// Push this node away from its left children
			m_root->rel += s;
			if (m_root->link[0])
				m_root->link[0]->rel -= s;
		}
		else // k > rootKey
		{
			if (m_root->link[1])
				PushRightVisit(rootKey, m_root->link[1], k, s);
		}
	}
}

template<class T>
void RelTree<T>::PushRightVisit(Key parentKey, Node* node, Key k, Key s)
{
	// Do NOT call this on the root node
	assert(node->parent);

	Key nodeKey = parentKey + node->rel;
	assert(parentKey != nodeKey);
	if (k < nodeKey)
	{
		if (parentKey < k)
			node->rel += s;
		if (node->link[0])
			PushRightVisit(nodeKey, node->link[0], k, s);
	}
	else if (k == nodeKey)
	{
		// Node exists
		// Push this node away from its left children
		if (parentKey < k)
			node->rel += s;
		if (node->link[0])
			node->link[0]->rel -= s;
	}
	else // k > nodeKey
	{
		if (parentKey > k)
			node->rel -= s;
		if (node->link[1])
			PushRightVisit(nodeKey, node->link[1], k, s);
	}
}

template<class T>
typename RelTree<T>::KeyNodeRefPair
RelTree<T>::FindFloor(Key k)
{
	if (!m_root)
		return KeyNodeRefPair(0, NodeRef(NULL));
	else
	{
		std::pair<Key, Node*> result = FindFloorVisit(0, NULL, m_root, k);
		return KeyNodeRefPair(result.first, NodeRef(result.second));
	}
}

template<class T>
std::pair<typename RelTree<T>::Key, typename RelTree<T>::Node*>
RelTree<T>::FindFloorVisit(Key parentKey, Node* parent, Node* node, Key k)
{
	Key nodeKey = parentKey + node->rel;
	if (k < nodeKey)
	{
		if (!node->link[0])
			return std::make_pair<Key, Node*>(0, NULL);
		else
			return FindFloorVisit(nodeKey, node, node->link[0], k);
	}
	else if (k == nodeKey)
		return std::make_pair(nodeKey, node);
	else // k > nodeKey
	{
		if (!node->link[1])
			return std::make_pair(nodeKey, node);
		else
		{
			std::pair<Key, Node*> rightFloor = FindFloorVisit(nodeKey, node, node->link[1], k);
			if (!rightFloor.second)
				return std::make_pair(nodeKey, node);
			else
				return rightFloor;
		}
	}
}

template<class T>
typename RelTree<T>::KeyConstNodeRefPair
RelTree<T>::FindFloor(Key k) const
{
	if (!m_root)
		return KeyConstNodeRefPair(0, ConstNodeRef(NULL));
	else
	{
		std::pair<Key, const Node*> result = FindFloorVisit(0, NULL, m_root, k);
		return KeyConstNodeRefPair(result.first, ConstNodeRef(result.second));
	}
}

template<class T>
std::pair<typename RelTree<T>::Key, typename const RelTree<T>::Node*>
RelTree<T>::FindFloorVisit(Key parentKey, const Node* parent, const Node* node, Key k) const
{
	Key nodeKey = parentKey + node->rel;
	if (k < nodeKey)
	{
		if (!node->link[0])
			return std::make_pair<Key, const Node*>(0, NULL);
		else
			return FindFloorVisit(nodeKey, node, node->link[0], k);
	}
	else if (k == nodeKey)
		return std::make_pair(nodeKey, node);
	else // k > nodeKey
	{
		if (!node->link[1])
			return std::make_pair(nodeKey, node);
		else
		{
			std::pair<Key, const Node*> rightFloor = FindFloorVisit(nodeKey, node, node->link[1], k);
			if (!rightFloor.second)
				return std::make_pair(nodeKey, node);
			else
				return rightFloor;
		}
	}
}

template<class T>
bool RelTree<T>::Validate() const
{
	if (m_root)
		assert(m_root->parent == NULL);
	return ValidateVisit(m_root) > 0;
}

template<class T>
int RelTree<T>::ValidateVisit(const Node* node) const
{
	int lh, rh;

	if (!node)
		return 1;
	else
	{
		const Node* ln = node->link[0];
		const Node* rn = node->link[1];

		if (IsRed(node))
		{
			// Red violation
			assert(!(IsRed(ln) || IsRed(rn)));
		}

		lh = ValidateVisit(ln);
		rh = ValidateVisit(rn);

		// Invalid binary search tree
		assert(!( (ln && ln->rel >= 0) || (rn && rn->rel <= 0) ));

		// Invalid parenthood
		assert(!( (ln && ln->parent != node) || (rn && rn->parent != node) ));

		// Black height mismatch
		assert(!( lh != 0 && rh != 0 && lh != rh ));

		// Only count black links
		if (lh != 0 && rh != 0)
			return IsRed(node) ? lh : lh + 1;
		else
			return 0;
	}
}

template<class T>
std::pair<typename RelTree<T>::Key, typename RelTree<T>::Node*>
RelTree<T>::FindMinVisit(Key parentKey, Node* node)
{
	assert(node);

	Key nodeKey = parentKey + node->rel;
	if (node->link[0])
		return FindMinVisit(nodeKey, node->link[0]);
	else
		return std::make_pair(nodeKey, node);
}

template<class T>
void RelTree<T>::SetDistanceToNext(NodeRef nr, Key s)
{
	assert(nr.IsValid());
	assert(s > 0);

	// Find next neighbor and common ancestor
	Node* commonAncestor = NULL;
	std::pair<Key, Node*> next(0, (Node*)NULL);
	if (nr.m_node->link[1])
	{
		// Next neighbor is among the right children.
		commonAncestor = nr.m_node;
		next = FindMinVisit(0, nr.m_node->link[1]);
	}
	else
	{
		// Go up through parents until we find one going right. That will be
		// the next.
		Node* cur = nr.m_node;
		while (!commonAncestor)
		{
			if (!cur->parent)
				return; // We've reached root. There was no common ancestor, so bail.

			next.first -= cur->rel;

			if (cur->rel < 0)
			{
				// Parent is on the right!
				commonAncestor = cur->parent;
				next.second = commonAncestor;
			}
			else
				cur = cur->parent;
		}
	}

	assert(next.first > 0);
	assert(next.second);

	Key curOffset = next.first;
	Key pushVal = s - curOffset;

	// TODO: More efficient would be to go through parents and adjust keys.
	Key nrKey = nr.GetKey();
	PushRight(nrKey, pushVal); // This might actually pull left!
}

#endif
