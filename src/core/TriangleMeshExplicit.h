/**
 * @file 
 * TriangleMeshExplicit.h
 *
 * @date: Feb 24, 2010
 * @author: sblume
 */

#ifndef TRIANGLEMESHEXPLICIT_H_
#define TRIANGLEMESHEXPLICIT_H_

#include "core/ITriangleMesh.h"
#include "core/Triangle.h"
#include <vector>

namespace BRICS_3D {

class TriangleMeshExplicit : public ITriangleMesh {
public:

	/**
	 * @brief Standard constructor
	 */
	TriangleMeshExplicit();

	/**
	 * @brief Standard destructor
	 */
	virtual ~TriangleMeshExplicit();

	/**
	 * @brief Get the pointer to the triangles
	 * To get access to the vertices of the mesh, prefer getTriangleVertex(),
	 * as that function is more generic and complies to the ITriangleMesh interface.
	 *
	 * @return Pointer to triangles
	 */
    std::vector<Triangle>* getTriangles();

	/**
	 * @brief Set the pointer to the triangles
	 * @param triangles Pointer to new triangle set
	 */
    void setTriangles(std::vector<Triangle>* triangles);

	int getSize();

	Point3D* getTriangleVertex(int triangleIndex, int vertexIndex);

	int addTriangle(Point3D vertex1, Point3D vertex2, Point3D vertex3);

	/**
	 * @brief Add a new triangle to the mesh
	 *
	 * @param triangle The triangle that will be added
	 * @return Returns the triangle index where it is stored
	 *
	 */
	int addTriangle(Triangle triangle);

	void removeTriangle (int triangleIndex);

	void homogeneousTransformation(IHomogeneousMatrix44 *transformation);

protected:

	///Pointer to vector which represents a triangle set
	std::vector<Triangle>* triangles;

private:

	void read(std::istream& inStream);

	void write(std::ostream& outStream);
};

}

#endif /* TRIANGLEMESHEXPLICIT_H_ */

/* EOF */
