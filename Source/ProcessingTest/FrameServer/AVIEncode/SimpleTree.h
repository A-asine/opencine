// Copyright (c) 2019 apertus° Association & contributors
// Project: OpenCine / ProcessingTest
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#ifndef SIMPLETREE_H
#define SIMPLETREE_H

#include <vector>
#include <functional>
#include <cstring>
#include <iostream>

// Reference: https://github.com/BAndiT1983/OC_FrameServer

typedef std::function<unsigned int(void*, unsigned int)> AVIFunc;

class Node
{
	std::vector<Node*> _nodeList;

	AVIFunc _func;
	bool _requiresSize;

public:
	Node(AVIFunc func, bool requiresSize) :
		_nodeList({}),
		_func(func),
		_requiresSize(requiresSize)
	{
	}

	Node* AddChild(AVIFunc func, bool requiresSize)
	{
		Node* node = new Node(func, requiresSize);
		_nodeList.push_back(node);

		return node;
	}

	unsigned int Execute(void* dataBuffer, unsigned int offset, bool calculateSize)	// calculateSize -> gathers size of nested chunks and writes it to Size attribute of AVIList or AVICHunk
	{
		unsigned int size = _func(dataBuffer, offset);
		unsigned int childSize = 0;

		for (Node* child : _nodeList)
		{
			childSize += child->Execute(dataBuffer, offset + size + childSize, calculateSize);
		}

		if (calculateSize)
		{
			uint32_t headerSize = 0;
			memcpy(&headerSize, (uint8_t*)dataBuffer + offset + sizeof(uint32_t), sizeof(uint32_t));
			headerSize += childSize;
			memcpy((uint8_t*)dataBuffer + offset + sizeof(uint32_t), &headerSize, sizeof(uint32_t));
			std::cout << "Written bytes: " << headerSize << std::endl;
		}

		return size + childSize;
	}

	bool GetRequiresSize() const
	{
		return _requiresSize;
	}
};


#endif // SIMPLETREE_H
