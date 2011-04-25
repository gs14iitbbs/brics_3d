/*
 * ObjectModelPlaneFromLineAndPoint.h
 *
 *  Created on: Apr 25, 2011
 *      Author: reon
 */

#ifndef OBJECTMODELPLANEFROMLINEANDPOINT_H_
#define OBJECTMODELPLANEFROMLINEANDPOINT_H_

#include "algorithm/segmentation/objectModels/ObjectModelPlane.h"

namespace BRICS_3D {

class ObjectModelPlaneFromLineAndPoint : public ObjectModelPlane{
protected:
	/** Used to check if a number is small enough to be considered to be 0 */
	static const double geometryEpsilon=1e-3;


public:

	ObjectModelPlaneFromLineAndPoint(){};
	virtual ~ObjectModelPlaneFromLineAndPoint(){};

	struct line{
		Point3D pbase;
		std::vector<double> director;
	};

	/** \brief Checks if a line contains a point or not*/

	inline bool contains(const line &line,Point3D &point){
		double dx=point.getX()-line.pbase.getX();
		double dy=point.getY()-line.pbase.getY();
		double dz=point.getZ()-line.pbase.getZ();
		if (abs(dx)<geometryEpsilon&&abs(dy)<geometryEpsilon&&abs(dz)<geometryEpsilon) return true;

		//       dx          dy          dz
		//if -----------=-----------=-----------, point is inside the line.
		//   director[0] director[1] director[2]

		return (abs(dx*line.director[1]-dy*line.director[0])<geometryEpsilon)&&
				(abs(dx*line.director[2]-dz*line.director[0])<geometryEpsilon)&&
				(abs(dy*line.director[2]-dz*line.director[1])<geometryEpsilon);
	}

	/** \brief Computes the cross product of two 3D vectors, returning a vector normal to both.
	 *  It uses the simple implementation:

	    \f[  v_out = \left(
				\begin{array}{c c c}
				\hat{i} ~ \hat{j} ~ \hat{k} \\
				x0 ~ y0 ~ z0 \\
				x1 ~ y1 ~ z1 \\
				\end{array} \right)
		\f]
	 */

	inline void crossProduct3D(
			const std::vector<double> &v0,
			const std::vector<double> &v1,
			std::vector<double> &v_out){
		//ToDo ASSERT_(v0.size()==3)
		//ToDo ASSERT_(v1.size()==3);
		v_out[0] =  v0[1]*v1[2] - v0[2]*v1[1];
		v_out[1] = -v0[0]*v1[2] + v0[2]*v1[0];
		v_out[2] =  v0[0]*v1[1] - v0[1]*v1[0];
	}

	void getSamples (int &iterations, std::vector<int> &samples);
	bool computeModelCoefficients (const std::vector<int> &samples, Eigen::VectorXf &model_coefficients);

	void computeRandomModel (int &iterations, Eigen::VectorXf &model_coefficients, bool &isDegenerate, bool &modelFound);
	inline int getNumberOfSamplesRequired(){return 3;};


};
}
#endif /* OBJECTMODELPLANEFROMLINEANDPOINT_H_ */
