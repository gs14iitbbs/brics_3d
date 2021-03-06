/******************************************************************************
* BRICS_3D - 3D Perception and Modeling Library
* Copyright (c) 2011, GPS GmbH
*
* Author: Sebastian Blumenthal
*
*
* This software is published under a dual-license: GNU Lesser General Public
* License LGPL 2.1 and Modified BSD license. The dual-license implies that
* users of this code may choose which terms they prefer.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License LGPL and the BSD license for
* more details.
*
******************************************************************************/

#ifndef RSG_ATTRIBUTEFINDER_H_
#define RSG_ATTRIBUTEFINDER_H_

#include "INodeVisitor.h"
#include "Node.h"
#include "Group.h"
#include "Transform.h"
#include "GeometricNode.h"
#include "Connection.h"
#include "Attribute.h"

using std::vector;

namespace brics_3d {

namespace rsg {

/**
 * Special graph traversal to find those nodes and connections that contain a certain subset of Attributes.
 * @ingroup sceneGraph
 */
class AttributeFinder: public INodeVisitor {
public:
	AttributeFinder();
	virtual ~AttributeFinder();

	virtual void visit(Node* node);
	virtual void visit(Group* node);
	virtual void visit(Transform* node);
	virtual void visit(GeometricNode* node);
	virtual void visit(Connection* connection);


	virtual void reset();

    vector<Node*> getMatchingNodes() const
    {
        return matchingNodes;
    }

    vector<Attribute> getQueryAttributes() const
    {
        return queryAttributes;
    }

    /**
     * Logical concatenation of multiple attributes is (for now) is AND
     * @param queryAttributes
     */
    void setQueryAttributes(vector<Attribute> queryAttributes)
    {
        this->queryAttributes = queryAttributes;
    }

protected:
	vector<Attribute> queryAttributes;
	vector<Node*> matchingNodes;
};

}

}

#endif /* RSG_ATTRIBUTEFINDER_H_ */

/* EOF */
