/**
 * @file 
 * SceneGraphNodesTest.cpp
 *
 * @date: Oct 20, 2011
 * @author: sblume
 */

#include "SceneGraphNodesTest.h"
#include <stdexcept>

namespace unitTests {

CPPUNIT_TEST_SUITE_REGISTRATION( SceneGraphNodesTest );

void SceneGraphNodesTest::setUp() {

}

void SceneGraphNodesTest::tearDown() {

}

void SceneGraphNodesTest::testNode() {

	Node testNode1;
	Node* testNode2 = new Node();

	unsigned int defaultId = 0;
	unsigned int testId = 42;

	CPPUNIT_ASSERT_EQUAL(defaultId, testNode1.getId());
	CPPUNIT_ASSERT_EQUAL(defaultId, testNode2->getId());

	testNode1.setId(testId);
	CPPUNIT_ASSERT_EQUAL(testId, testNode1.getId());

	CPPUNIT_ASSERT_EQUAL(0u, static_cast<unsigned int>(testNode1.getAttributes().size())); // Attributes should be empty...
	vector<Attribute> testAttributes;
	testAttributes.push_back(Attribute("myName","testNode1"));
	testNode1.setAttributes(testAttributes);
	CPPUNIT_ASSERT_EQUAL(1u, static_cast<unsigned int>(testNode1.getAttributes().size())); // Now there should be one

//	testNode2->setParents()


	delete testNode2;
}

void SceneGraphNodesTest::testGroup() {
	Group root;
	Node::NodePtr child1(new Node());
	Node::NodePtr child2(new Node());

	unsigned const int rootId = 1;
	unsigned const int child1testId = 42;
	unsigned const int child2testId = 123;

	root.setId(rootId);
	child1->setId(child1testId);
	child2->setId(child2testId);


	CPPUNIT_ASSERT_EQUAL(rootId, root.getId()); // preconditions
	CPPUNIT_ASSERT_EQUAL(child1testId, child1->getId());
	CPPUNIT_ASSERT_EQUAL(child2testId, child2->getId());
	CPPUNIT_ASSERT_EQUAL(0u, root.getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(0u, root.getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(0u, child1->getNumberOfParents());

	/* create parent-child relationship */
	root.addChild(child1);

	CPPUNIT_ASSERT_EQUAL(1u, root.getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(0u, root.getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(1u, child1->getNumberOfParents());

	CPPUNIT_ASSERT_EQUAL(rootId, child1->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(child1testId, root.getChild(0)->getId());

	/* create another parent-child relationship */
	root.insertChild(child2, 0u); //insert on first place

	CPPUNIT_ASSERT_EQUAL(2u, root.getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(0u, root.getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(1u, child1->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(1u, child2->getNumberOfParents());

	CPPUNIT_ASSERT_EQUAL(rootId, child1->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(rootId, child2->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(child1testId, root.getChild(1)->getId());
	CPPUNIT_ASSERT_EQUAL(child2testId, root.getChild(0)->getId());

	/* delete the children */
	root.removeChild(child1);
	CPPUNIT_ASSERT_EQUAL(0u, child1->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(1u, root.getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(child2testId, root.getChild(0)->getId());

	root.removeChild(child2);
	CPPUNIT_ASSERT_EQUAL(0u, child2->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(0u, root.getNumberOfChildren());

//	CPPUNIT_ASSERT_THROW(child1->getParent(1000)->getId(), out_of_range);
//	CPPUNIT_ASSERT_THROW(root.getChild(1000)->getId(), out_of_range);

}

void SceneGraphNodesTest::testTransform() {

	Group::GroupPtr root(new Group);
	RSG::Transform::TransformPtr transform1 (new RSG::Transform);

	unsigned const int rootId = 1;
	unsigned const int transform1Id = 2;

	root->setId(rootId);
	transform1->setId(transform1Id);

	CPPUNIT_ASSERT_EQUAL(rootId, root->getId()); // preconditions
	CPPUNIT_ASSERT_EQUAL(transform1Id, transform1->getId());

	root->addChild(transform1);

	CPPUNIT_ASSERT_EQUAL(1u, root->getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(0u, root->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(1u, transform1->getNumberOfParents());

	CPPUNIT_ASSERT_EQUAL(rootId, transform1->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(transform1Id, root->getChild(0)->getId());

	/* set transform */
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr transform123(new HomogeneousMatrix44(1,0,0,  //Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             1,2,3)); //Translation coefficients
	transform1->insertTransform(transform123, TimeStamp(1.0));

	/* getTransform */
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr resultTransform;
	CPPUNIT_ASSERT(resultTransform == 0);
	resultTransform = transform1->getTransform(TimeStamp(1.0));
	CPPUNIT_ASSERT(resultTransform != 0);
	CPPUNIT_ASSERT(resultTransform = transform123);


	matrixPtr = resultTransform->getRawData();

	/* rotation in column-major order*/
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[0], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[4], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[8], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[1], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[5], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[9], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[2], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[6], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[10], maxTolerance);

	/* translation */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[12], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, matrixPtr[14], maxTolerance);

	/* stuffing coefficients */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[3], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[7], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[11], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[15], maxTolerance);



	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr transform654(new HomogeneousMatrix44(1,0,0,  //Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             6,5,4)); //Translation coefficients
	transform1->insertTransform(transform654, TimeStamp(1.0));

	/* getTransform */
	resultTransform.reset();
	CPPUNIT_ASSERT(resultTransform == 0);
	resultTransform = transform1->getTransform(TimeStamp(1.0));
	CPPUNIT_ASSERT(resultTransform != 0);
	CPPUNIT_ASSERT(resultTransform = transform654);


	matrixPtr = resultTransform->getRawData();

	/* rotation in column-major order*/
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[0], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[4], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[8], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[1], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[5], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[9], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[2], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[6], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[10], maxTolerance);

	/* translation */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, matrixPtr[12], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, matrixPtr[14], maxTolerance);

	/* stuffing coefficients */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[3], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[7], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[11], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[15], maxTolerance);

}

void SceneGraphNodesTest::testOwnership() {
	/* Graph structure: (remember: nodes can only serve as are leaves)
	 *             root
	 *              |
	 *        ------+-----
	 *        |          |
	 *      group1     group2
	 *        |          |
	 *        +----  ----+
	 *            |  |
	 *            group3
	 */

	/*
	 * here we use an inner scope to completely pass by the ownership
	 * to the scenegraph. "Normal" shared pointers would "hold" the reference for the
	 * duration of the test...
	 */
	Group::GroupPtr root(new Group());
	boost::weak_ptr<Group> testWeakPtr();

	unsigned const int rootId = 0;
	unsigned const int group1Id = 1;
	unsigned const int group2Id = 2;
	unsigned const int group3Id = 3;

	{
		Group::GroupPtr group1(new Group());
		group1->setId(group1Id);
		Group::GroupPtr group2(new Group());
		group2->setId(group2Id);
		Group::GroupPtr group3(new Group());
		group3->setId(group3Id);

		root->addChild(group1);
		root->addChild(group2);
		group1->addChild(group3);
		group2->addChild(group3);

		Group::GroupPtr test(new Group());
//		testWeakPtr = test;
//		CPPUNIT_ASSERT(testWeakPtr.lock() != 0);

	} // let the local shared pointer go out of scope...
//	CPPUNIT_ASSERT(testWeakPtr.lock() == 0);

	CPPUNIT_ASSERT_EQUAL(2u, root->getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(2u, boost::dynamic_pointer_cast<Group>(root->getChild(0))->getChild(0)->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(group1Id, root->getChild(0)->getId());
	CPPUNIT_ASSERT_EQUAL(group2Id, root->getChild(1)->getId());
	CPPUNIT_ASSERT_EQUAL(group3Id, boost::dynamic_pointer_cast<Group>(root->getChild(0))->getChild(0)->getId());
	CPPUNIT_ASSERT_EQUAL(group3Id, boost::dynamic_pointer_cast<Group>(root->getChild(1))->getChild(0)->getId());

	root->removeChildren(0);

	/* now it should be:
	 *             root
	 *              |
	 *              +-----
	 *                   |
	 *                 group2
	 *                   |
	 *               ----+
	 *               |
	 *            group3
	 */

	CPPUNIT_ASSERT_EQUAL(1u, root->getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(1u, boost::dynamic_pointer_cast<Group>(root->getChild(0))->getChild(0)->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(group2Id, root->getChild(0)->getId());
	CPPUNIT_ASSERT_EQUAL(group3Id, boost::dynamic_pointer_cast<Group>(root->getChild(0))->getChild(0)->getId());

}

void SceneGraphNodesTest::testSimpleGraph() {
	/* Graph structure: (remember: nodes can only serve as are leaves)
	 *             root
	 *              |
	 *        ------+-------
	 *        |             |
	 *      group1        group2
	 *        |             |
	 *    ----+----  -------+-------
	 *    |       |  |      |      |
	 *   node3    node4  node5   group6
	 */

	unsigned const int rootId = 0;
	unsigned const int group1Id = 1;
	unsigned const int group2Id = 2;
	unsigned const int node3Id = 3;
	unsigned const int node4Id = 4;
	unsigned const int node5Id = 5;
	unsigned const int group6Id = 6;

	Group::GroupPtr root(new Group());
	root->setId(rootId);
	Group::GroupPtr group1(new Group());
	group1->setId(group1Id);
	Group::GroupPtr group2(new Group());
	group2->setId(group2Id);

	Node::NodePtr node3(new Node());
	node3->setId(node3Id);
	Node::NodePtr node4(new Node());
	node4->setId(node4Id);
	Node::NodePtr node5(new Node());
	node5->setId(node5Id);

	Group::GroupPtr group6(new Group());
	group6->setId(group6Id);

	CPPUNIT_ASSERT_EQUAL(rootId, root->getId()); // preconditions:
	CPPUNIT_ASSERT_EQUAL(group1Id, group1->getId());
	CPPUNIT_ASSERT_EQUAL(group2Id, group2->getId());
	CPPUNIT_ASSERT_EQUAL(node3Id, node3->getId());
	CPPUNIT_ASSERT_EQUAL(node4Id, node4->getId());
	CPPUNIT_ASSERT_EQUAL(node5Id, node5->getId());
	CPPUNIT_ASSERT_EQUAL(group6Id, group6->getId());

	/* set up graph */
	root->addChild(group1);
	root->addChild(group2);

	group1->addChild(node3);
	group1->addChild(node4);

	group2->addChild(node4); //here we brake the "tree" structure to create a real "graph"
	group2->addChild(node5);
	group2->addChild(group6); //just to test heterogeneous nodes.

	/*
	 * now check if every node(group) has its correct place in the graph
	 */
	/* root */
	CPPUNIT_ASSERT_EQUAL(0u, root->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(2u, root->getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(group1Id, root->getChild(0)->getId());
	CPPUNIT_ASSERT_EQUAL(group2Id, root->getChild(1)->getId());

	/* group1 */
	CPPUNIT_ASSERT_EQUAL(1u, group1->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(rootId, group1->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(2u, group1->getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(node3Id, group1->getChild(0)->getId());
	CPPUNIT_ASSERT_EQUAL(node4Id, group1->getChild(1)->getId());

	/* group2 */
	CPPUNIT_ASSERT_EQUAL(1u, group2->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(rootId, group2->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(3u, group2->getNumberOfChildren());
	CPPUNIT_ASSERT_EQUAL(node4Id, group2->getChild(0)->getId());
	CPPUNIT_ASSERT_EQUAL(node5Id, group2->getChild(1)->getId());
	CPPUNIT_ASSERT_EQUAL(group6Id, group2->getChild(2)->getId());

	/* node3 */
	CPPUNIT_ASSERT_EQUAL(1u, node3->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(group1Id, node3->getParent(0)->getId());

	/* node4 */
	CPPUNIT_ASSERT_EQUAL(2u, node4->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(group1Id, node4->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(group2Id, node4->getParent(1)->getId());

	/* node5 */
	CPPUNIT_ASSERT_EQUAL(1u, node5->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(group2Id, node5->getParent(0)->getId());

	/* group6 */
	CPPUNIT_ASSERT_EQUAL(1u, group6->getNumberOfParents());
	CPPUNIT_ASSERT_EQUAL(group2Id, group6->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(0u, group6->getNumberOfChildren());

	/*
	 * check if we can get some leave-to-root paths
	 */
	CPPUNIT_ASSERT_EQUAL(rootId, node3->getParent(0)->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(rootId, node4->getParent(0)->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(rootId, node4->getParent(1)->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(rootId, node5->getParent(0)->getParent(0)->getId());
	CPPUNIT_ASSERT_EQUAL(rootId, group6->getParent(0)->getParent(0)->getId());

	/*
	 * check if we can get some root-to-leave paths
	 */
	CPPUNIT_ASSERT_EQUAL(node3Id, boost::dynamic_pointer_cast<Group>(root->getChild(0))->getChild(0)->getId());
	CPPUNIT_ASSERT_EQUAL(node4Id, boost::dynamic_pointer_cast<Group>(root->getChild(0))->getChild(1)->getId());

	CPPUNIT_ASSERT_EQUAL(node4Id, boost::dynamic_pointer_cast<Group>(root->getChild(1))->getChild(0)->getId());
	CPPUNIT_ASSERT_EQUAL(node5Id, boost::dynamic_pointer_cast<Group>(root->getChild(1))->getChild(1)->getId());
	CPPUNIT_ASSERT_EQUAL(group6Id, boost::dynamic_pointer_cast<Group>(root->getChild(1))->getChild(2)->getId());
	CPPUNIT_ASSERT_EQUAL(0u, boost::dynamic_pointer_cast<Group>(boost::dynamic_pointer_cast<Group>(root->getChild(1))->getChild(2))->getNumberOfChildren());


}

void SceneGraphNodesTest::testSimpleVisitor() {
	/* Graph structure: (remember: nodes can only serve as are leaves)
	 *                 root
	 *                   |
	 *        -----------+----------
	 *        |          |         |
	 *      group1     group2    node3
	 *        |          |
	 *        +----  ----+
	 *            |  |
	 *            node4
	 */
	unsigned const int rootId = 0;
	unsigned const int group1Id = 1;
	unsigned const int group2Id = 2;
	unsigned const int node3Id = 3;
	unsigned const int node4Id = 4;

	Group::GroupPtr root(new Group());
	root->setId(rootId);
	Group::GroupPtr group1(new Group());
	group1->setId(group1Id);
	Group::GroupPtr group2(new Group());
	group2->setId(group2Id);

	Node::NodePtr node3(new Node());
	node3->setId(node3Id);
	Node::NodePtr node4(new Node());
	node4->setId(node4Id);

	CPPUNIT_ASSERT_EQUAL(rootId, root->getId()); // preconditions:
	CPPUNIT_ASSERT_EQUAL(group1Id, group1->getId());
	CPPUNIT_ASSERT_EQUAL(group2Id, group2->getId());
	CPPUNIT_ASSERT_EQUAL(node3Id, node3->getId());
	CPPUNIT_ASSERT_EQUAL(node4Id, node4->getId());

	/* setup scenegraph */
	root->addChild(group1);
	root->addChild(group2);
	root->addChild(node3);
	group1->addChild(node4);
	group2->addChild(node4);

	IdCollector* idCollector = new IdCollector();

	/* traverse from root */
	CPPUNIT_ASSERT_EQUAL(0u, static_cast<unsigned int>(idCollector->collectedIDs.size()));
	CPPUNIT_ASSERT(idCollector->getDirection() == INodeVisitor::downwards);

	/* traverse from root */
	root->accept(idCollector); // traverse the graph downwards from root with the visitor

	CPPUNIT_ASSERT_EQUAL(6u, static_cast<unsigned int>(idCollector->collectedIDs.size()));
	CPPUNIT_ASSERT_EQUAL(rootId, idCollector->collectedIDs[0]); //Remember: we have depth-first-search
	CPPUNIT_ASSERT_EQUAL(group1Id, idCollector->collectedIDs[1]);
	CPPUNIT_ASSERT_EQUAL(node4Id, idCollector->collectedIDs[2]);
	CPPUNIT_ASSERT_EQUAL(group2Id, idCollector->collectedIDs[3]);
	CPPUNIT_ASSERT_EQUAL(node4Id, idCollector->collectedIDs[4]);
	CPPUNIT_ASSERT_EQUAL(node3Id, idCollector->collectedIDs[5]);

	/* traverse from group2 */
	idCollector->collectedIDs.clear();
	CPPUNIT_ASSERT_EQUAL(0u, static_cast<unsigned int>(idCollector->collectedIDs.size()));
	CPPUNIT_ASSERT(idCollector->getDirection() == INodeVisitor::downwards);

	group2->accept(idCollector);

	CPPUNIT_ASSERT_EQUAL(2u, static_cast<unsigned int>(idCollector->collectedIDs.size()));
	CPPUNIT_ASSERT_EQUAL(group2Id, idCollector->collectedIDs[3]);
	CPPUNIT_ASSERT_EQUAL(node4Id, idCollector->collectedIDs[4]);

	/* traverse from upwards from group2 */
	idCollector->collectedIDs.clear();
	CPPUNIT_ASSERT_EQUAL(0u, static_cast<unsigned int>(idCollector->collectedIDs.size()));
	idCollector->setDirection(INodeVisitor::upwards);
	CPPUNIT_ASSERT(idCollector->getDirection() == INodeVisitor::upwards);

	group2->accept(idCollector);

	CPPUNIT_ASSERT_EQUAL(2u, static_cast<unsigned int>(idCollector->collectedIDs.size()));
	CPPUNIT_ASSERT_EQUAL(group2Id, idCollector->collectedIDs[0]);
	CPPUNIT_ASSERT_EQUAL(rootId, idCollector->collectedIDs[1]);

	/* traverse from upwards from node4 */
	idCollector->collectedIDs.clear();
	CPPUNIT_ASSERT_EQUAL(0u, static_cast<unsigned int>(idCollector->collectedIDs.size()));
	idCollector->setDirection(INodeVisitor::upwards);
	CPPUNIT_ASSERT(idCollector->getDirection() == INodeVisitor::upwards);

	node4->accept(idCollector);

	CPPUNIT_ASSERT_EQUAL(5u, static_cast<unsigned int>(idCollector->collectedIDs.size()));
	CPPUNIT_ASSERT_EQUAL(node4Id, idCollector->collectedIDs[0]);
	CPPUNIT_ASSERT_EQUAL(group1Id, idCollector->collectedIDs[1]);
	CPPUNIT_ASSERT_EQUAL(rootId, idCollector->collectedIDs[2]);
	CPPUNIT_ASSERT_EQUAL(group2Id, idCollector->collectedIDs[3]);
	CPPUNIT_ASSERT_EQUAL(rootId, idCollector->collectedIDs[4]);

	delete idCollector;
}

void SceneGraphNodesTest::testPathCollectorVisitor() {
	/* Graph structure: (remember: nodes can only serve as are leaves)
	 *                 root
	 *                   |
	 *        -----------+----------
	 *        |          |         |
	 *      group1     group2    node3
	 *        |          |
	 *        +----  ----+
	 *            |  |
	 *            node4
	 */
	unsigned const int rootId = 0;
	unsigned const int group1Id = 1;
	unsigned const int group2Id = 2;
	unsigned const int node3Id = 3;
	unsigned const int node4Id = 4;

	Group::GroupPtr root(new Group());
	root->setId(rootId);
	Group::GroupPtr group1(new Group());
	group1->setId(group1Id);
	Group::GroupPtr group2(new Group());
	group2->setId(group2Id);

	Node::NodePtr node3(new Node());
	node3->setId(node3Id);
	Node::NodePtr node4(new Node());
	node4->setId(node4Id);

	CPPUNIT_ASSERT_EQUAL(rootId, root->getId()); // preconditions:
	CPPUNIT_ASSERT_EQUAL(group1Id, group1->getId());
	CPPUNIT_ASSERT_EQUAL(group2Id, group2->getId());
	CPPUNIT_ASSERT_EQUAL(node3Id, node3->getId());
	CPPUNIT_ASSERT_EQUAL(node4Id, node4->getId());

	/* setup scenegraph */
	root->addChild(group1);
	root->addChild(group2);
	root->addChild(node3);
	group1->addChild(node4);
	group2->addChild(node4);

	PathCollector* pathCollector = new PathCollector();

	/* root */
	CPPUNIT_ASSERT_EQUAL(0u, static_cast<unsigned int>(pathCollector->getNodePaths().size()));

	root->accept(pathCollector);

	CPPUNIT_ASSERT_EQUAL(1u, static_cast<unsigned int>(pathCollector->getNodePaths().size()));
	CPPUNIT_ASSERT_EQUAL(0u, static_cast<unsigned int>(pathCollector->getNodePaths()[0].size()));

	/* group2 */
	pathCollector->reset();
	CPPUNIT_ASSERT_EQUAL(0u, static_cast<unsigned int>(pathCollector->getNodePaths().size()));

	group2->accept(pathCollector);

	CPPUNIT_ASSERT_EQUAL(1u, static_cast<unsigned int>(pathCollector->getNodePaths().size()));
	CPPUNIT_ASSERT_EQUAL(1u, static_cast<unsigned int>(pathCollector->getNodePaths()[0].size()));
	CPPUNIT_ASSERT_EQUAL(rootId, (*pathCollector->getNodePaths()[0][0]).getId());

	/* node4 */
	pathCollector->reset();
	CPPUNIT_ASSERT_EQUAL(0u, static_cast<unsigned int>(pathCollector->getNodePaths().size()));

	node4->accept(pathCollector);

	CPPUNIT_ASSERT_EQUAL(2u, static_cast<unsigned int>(pathCollector->getNodePaths().size()));
	CPPUNIT_ASSERT_EQUAL(2u, static_cast<unsigned int>(pathCollector->getNodePaths()[0].size()));
	CPPUNIT_ASSERT_EQUAL(2u, static_cast<unsigned int>(pathCollector->getNodePaths()[1].size()));

	CPPUNIT_ASSERT_EQUAL(rootId, (*pathCollector->getNodePaths()[0][0]).getId()); // 1. path
	CPPUNIT_ASSERT_EQUAL(group1Id, (*pathCollector->getNodePaths()[0][1]).getId());

	CPPUNIT_ASSERT_EQUAL(rootId, (*pathCollector->getNodePaths()[1][0]).getId()); // 2. path
	CPPUNIT_ASSERT_EQUAL(group2Id, (*pathCollector->getNodePaths()[1][1]).getId());

	delete pathCollector;
}

void SceneGraphNodesTest::testTransformVisitor() {
	/* Graph structure: (remember: nodes can only serve as are leaves)
	 *            root
	 *              |
	 *        ------+-----
	 *        |          |
	 *       tf1        tf2
	 *        |          |
	 *       tf3        group4
	 *        |          |
	 *        +----  ----+
	 *            |  |
	 *            node5
	 */
	unsigned const int rootId = 0;
	unsigned const int tf1Id = 1;
	unsigned const int tf2Id = 2;
	unsigned const int tf3Id = 3;
	unsigned const int group4Id = 4;
	unsigned const int node5Id = 5;

	Group::GroupPtr root(new Group());
	root->setId(rootId);
	RSG::Transform::TransformPtr tf1(new RSG::Transform());
	tf1->setId(tf1Id);
	RSG::Transform::TransformPtr tf2(new RSG::Transform());
	tf2->setId(tf2Id);
	RSG::Transform::TransformPtr tf3(new RSG::Transform());
	tf3->setId(tf3Id);
	Group::GroupPtr group4(new Group());
	group4->setId(group4Id);
	Node::NodePtr node5(new Node());
	node5->setId(node5Id);

	TimeStamp dummyTime(1.0);
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr transform123(new HomogeneousMatrix44(1,0,0,  	//Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             1,2,3)); 						//Translation coefficients
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr transform654(new HomogeneousMatrix44(1,0,0,  	//Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             6,5,4)); 						//Translation coefficients

	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr transform789(new HomogeneousMatrix44(1,0,0,  	//Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             7,8,9)); 						//Translation coefficients
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr resultTransform;

	tf1->insertTransform(transform123, dummyTime);
	tf2->insertTransform(transform654, dummyTime);
	tf3->insertTransform(transform789, dummyTime);

	CPPUNIT_ASSERT_EQUAL(rootId, root->getId()); // preconditions:
	CPPUNIT_ASSERT_EQUAL(tf1Id, tf1->getId());
	CPPUNIT_ASSERT_EQUAL(tf2Id, tf2->getId());
	CPPUNIT_ASSERT_EQUAL(tf3Id, tf3->getId());
	CPPUNIT_ASSERT_EQUAL(group4Id, group4->getId());
	CPPUNIT_ASSERT_EQUAL(node5Id, node5->getId());

	/* setup scenegraph */
	root->addChild(tf1);
	root->addChild(tf2);
	tf1->addChild(tf3);
	tf3->addChild(node5);
	tf2->addChild(group4);
	group4->addChild(node5);


	IdCollector* idCollector = new IdCollector();

	cout << "Graph with transforms" << endl;
	root->accept(idCollector);

	CPPUNIT_ASSERT_EQUAL(7u, static_cast<unsigned int>(idCollector->collectedIDs.size()));
	CPPUNIT_ASSERT_EQUAL(rootId, idCollector->collectedIDs[0]); //Remember: we have depth-first-search
	CPPUNIT_ASSERT_EQUAL(tf1Id, idCollector->collectedIDs[1]);
	CPPUNIT_ASSERT_EQUAL(tf3Id, idCollector->collectedIDs[2]);
	CPPUNIT_ASSERT_EQUAL(node5Id, idCollector->collectedIDs[3]);
	CPPUNIT_ASSERT_EQUAL(tf2Id, idCollector->collectedIDs[4]);
	CPPUNIT_ASSERT_EQUAL(group4Id, idCollector->collectedIDs[5]);
	CPPUNIT_ASSERT_EQUAL(node5Id, idCollector->collectedIDs[6]);

	PathCollector* pathCollector = new PathCollector();
	node5->accept(pathCollector);

	CPPUNIT_ASSERT_EQUAL(2u, static_cast<unsigned int>(pathCollector->getNodePaths().size()));
	CPPUNIT_ASSERT_EQUAL(3u, static_cast<unsigned int>(pathCollector->getNodePaths()[0].size()));
	CPPUNIT_ASSERT_EQUAL(3u, static_cast<unsigned int>(pathCollector->getNodePaths()[1].size()));

	CPPUNIT_ASSERT_EQUAL(rootId, (*pathCollector->getNodePaths()[0][0]).getId()); // 1. path
	CPPUNIT_ASSERT_EQUAL(tf1Id, (*pathCollector->getNodePaths()[0][1]).getId());
	CPPUNIT_ASSERT_EQUAL(tf3Id, (*pathCollector->getNodePaths()[0][2]).getId());

	CPPUNIT_ASSERT_EQUAL(rootId, (*pathCollector->getNodePaths()[1][0]).getId()); // 2. path
	CPPUNIT_ASSERT_EQUAL(tf2Id, (*pathCollector->getNodePaths()[1][1]).getId());
	CPPUNIT_ASSERT_EQUAL(group4Id, (*pathCollector->getNodePaths()[1][2]).getId());

	resultTransform = getGlobalTransformAlongPath(pathCollector->getNodePaths()[0]);
//	cout << (*resultTransform) << endl;

	matrixPtr = resultTransform->getRawData();

	/* rotation in column-major order*/
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[0], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[4], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[8], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[1], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[5], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[9], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[2], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[6], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[10], maxTolerance);

	/* translation */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, matrixPtr[12], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(12.0, matrixPtr[14], maxTolerance);

	/* stuffing coefficients */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[3], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[7], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[11], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[15], maxTolerance);

	resultTransform = getGlobalTransformAlongPath(pathCollector->getNodePaths()[1]);
//	cout << (*resultTransform) << endl;

	matrixPtr = resultTransform->getRawData();

	/* rotation in column-major order*/
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[0], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[4], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[8], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[1], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[5], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[9], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[2], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[6], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[10], maxTolerance);

	/* translation */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, matrixPtr[12], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, matrixPtr[14], maxTolerance);

	/* stuffing coefficients */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[3], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[7], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[11], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[15], maxTolerance);


	/*
	 * Here we update one transform and look what will happen...
	 */
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr someUpdate(new HomogeneousMatrix44(1,0,0,  	//Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             -100,-100,-100)); 						//Translation coefficients
	tf1->insertTransform(someUpdate, dummyTime);
	pathCollector->reset();
	node5->accept(pathCollector);


	CPPUNIT_ASSERT_EQUAL(2u, static_cast<unsigned int>(pathCollector->getNodePaths().size()));
	CPPUNIT_ASSERT_EQUAL(3u, static_cast<unsigned int>(pathCollector->getNodePaths()[0].size()));
	CPPUNIT_ASSERT_EQUAL(3u, static_cast<unsigned int>(pathCollector->getNodePaths()[1].size()));

	CPPUNIT_ASSERT_EQUAL(rootId, (*pathCollector->getNodePaths()[0][0]).getId()); // 1. path
	CPPUNIT_ASSERT_EQUAL(tf1Id, (*pathCollector->getNodePaths()[0][1]).getId());
	CPPUNIT_ASSERT_EQUAL(tf3Id, (*pathCollector->getNodePaths()[0][2]).getId());

	CPPUNIT_ASSERT_EQUAL(rootId, (*pathCollector->getNodePaths()[1][0]).getId()); // 2. path
	CPPUNIT_ASSERT_EQUAL(tf2Id, (*pathCollector->getNodePaths()[1][1]).getId());
	CPPUNIT_ASSERT_EQUAL(group4Id, (*pathCollector->getNodePaths()[1][2]).getId());

	resultTransform = getGlobalTransformAlongPath(pathCollector->getNodePaths()[0]);
//	cout << (*resultTransform) << endl;

	matrixPtr = resultTransform->getRawData();

	/* rotation in column-major order*/
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[0], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[4], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[8], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[1], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[5], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[9], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[2], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[6], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[10], maxTolerance);

	/* translation */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-93.0, matrixPtr[12], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-92.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-91.0, matrixPtr[14], maxTolerance);

	/* stuffing coefficients */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[3], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[7], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[11], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[15], maxTolerance);

	resultTransform = getGlobalTransformAlongPath(pathCollector->getNodePaths()[1]);
//	cout << (*resultTransform) << endl;

	matrixPtr = resultTransform->getRawData();

	/* rotation in column-major order*/
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[0], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[4], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[8], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[1], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[5], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[9], maxTolerance);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[2], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[6], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[10], maxTolerance);

	/* translation */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, matrixPtr[12], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, matrixPtr[14], maxTolerance);

	/* stuffing coefficients */
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[3], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[7], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[11], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[15], maxTolerance);

	delete pathCollector;
	delete idCollector;
}

void SceneGraphNodesTest::testGlobalTransformCalculation() {
	/* Graph structure: (remember: nodes can only serve as are leaves)
	 *            root(tf)
	 *              |
	 *        ------+-----
	 *        |          |
	 *       tf1        tf2
	 *        |          |
	 *       tf3        group4
	 *        |          |
	 *        +----  ----+
	 *            |  |
	 *            node5
	 */
	unsigned const int rootId = 0;
	unsigned const int tf1Id = 1;
	unsigned const int tf2Id = 2;
	unsigned const int tf3Id = 3;
	unsigned const int group4Id = 4;
	unsigned const int node5Id = 5;

	RSG::Transform::TransformPtr root(new RSG::Transform());
	root->setId(rootId);
	RSG::Transform::TransformPtr tf1(new RSG::Transform());
	tf1->setId(tf1Id);
	RSG::Transform::TransformPtr tf2(new RSG::Transform());
	tf2->setId(tf2Id);
	RSG::Transform::TransformPtr tf3(new RSG::Transform());
	tf3->setId(tf3Id);
	Group::GroupPtr group4(new Group());
	group4->setId(group4Id);
	Node::NodePtr node5(new Node());
	node5->setId(node5Id);

	TimeStamp dummyTime(1.0);
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr transform001(new HomogeneousMatrix44(1,0,0,  	//Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             0,0,-1));
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr transform123(new HomogeneousMatrix44(1,0,0,  	//Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             1,2,3)); 						//Translation coefficients
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr transform654(new HomogeneousMatrix44(1,0,0,  	//Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             6,5,4)); 						//Translation coefficients

	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr transform789(new HomogeneousMatrix44(1,0,0,  	//Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             7,8,9)); 						//Translation coefficients
	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr resultTransform;

	root->insertTransform(transform001, dummyTime);
	tf1->insertTransform(transform123, dummyTime);
	tf2->insertTransform(transform654, dummyTime);
	tf3->insertTransform(transform789, dummyTime);

	CPPUNIT_ASSERT_EQUAL(rootId, root->getId()); // preconditions:
	CPPUNIT_ASSERT_EQUAL(tf1Id, tf1->getId());
	CPPUNIT_ASSERT_EQUAL(tf2Id, tf2->getId());
	CPPUNIT_ASSERT_EQUAL(tf3Id, tf3->getId());
	CPPUNIT_ASSERT_EQUAL(group4Id, group4->getId());
	CPPUNIT_ASSERT_EQUAL(node5Id, node5->getId());

	/* setup scenegraph */
	root->addChild(tf1);
	root->addChild(tf2);
	tf1->addChild(tf3);
	tf3->addChild(node5);
	tf2->addChild(group4);
	group4->addChild(node5);


	resultTransform = getGlobalTransform(root);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(tf1);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(tf3);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(11.0, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(node5);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(11.0, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(tf2);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(group4);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, matrixPtr[14], maxTolerance);

	IHomogeneousMatrix44::IHomogeneousMatrix44Ptr someUpdate(new HomogeneousMatrix44(1,0,0,  	//Rotation coefficients
	                                                             0,1,0,
	                                                             0,0,1,
	                                                             100,100,99));
	root->insertTransform(someUpdate, dummyTime);

	resultTransform = getGlobalTransform(root);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(99, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(tf1);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(101.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(102.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(102.0, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(tf3);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(108.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(110.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(111.0, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(node5);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(108.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(110.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(111.0, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(tf2);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(106.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(105.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(103.0, matrixPtr[14], maxTolerance);

	resultTransform = getGlobalTransform(group4);
	matrixPtr = resultTransform->getRawData();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(106.0, matrixPtr[12], maxTolerance); //check (just) translation
	CPPUNIT_ASSERT_DOUBLES_EQUAL(105.0, matrixPtr[13], maxTolerance);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(103.0, matrixPtr[14], maxTolerance);


}

}  // namespace unitTests

/* EOF */
