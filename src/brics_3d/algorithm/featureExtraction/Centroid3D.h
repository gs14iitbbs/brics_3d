/******************************************************************************
* BRICS_3D - 3D Perception and Modeling Library
* Copyright (c) 2011, GPS GmbH
*
* Author: Pinaki Sunil Banerjee
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

#ifndef BRICS_3D_CENTROID3DESTIMATION_H_
#define BRICS_3D_CENTROID3DESTIMATION_H_

#include "brics_3d/core/PointCloud3D.h"
#include "brics_3d/core/IPoint3DIterator.h"

#include <Eigen/Dense>

namespace brics_3d {

/**
 * @brief Computes the centroid of a point cloud.
 * @ingroup featureExtraction
 * The arithmetic mean is used here as centroid.
 */
class Centroid3D {
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	Centroid3D();
	virtual ~Centroid3D();

	/**
	 *
	 * @param[in] inCloud The point cloud whose centroid shall be computed.
	 * @return 3D Vector representing the centroid.
	 */
	Eigen::Vector3d computeCentroid(brics_3d::PointCloud3D *inCloud);

	/**
	 *
	 * @param[in] inCloud The point cloud accessible via an iterator whose centroid shall be computed.
	 * @return 3D Vector representing the centroid.
	 */
	Eigen::Vector3d computeCentroid(IPoint3DIterator::IPoint3DIteratorPtr inCloud);

	/**
	 * Specialized version for some PCL refactored code as it uses aligned points.
	 * @param inCloud
	 * @param indices
	 * @return 3D Vector representing the centroid.
	 */
	static Eigen::Vector4d computeCentroid (brics_3d::PointCloud3D *inCloud, const std::vector<int> &indices);


};

}

#endif /* BRICS_3D_CENTROID3DESTIMATION_H_ */
