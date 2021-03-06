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

#ifndef BRICS_3D_INORMALESTIMATION_H_
#define BRICS_3D_INORMALESTIMATION_H_

#include "brics_3d/core/PointCloud3D.h"
#include "brics_3d/core/NormalSet3D.h"

namespace brics_3d {

/**
 * @brief Abstract interface for normal estimation algorithms for point clouds
 * @ingroup featureExtraction
 */
class INormalEstimation {
public:
	INormalEstimation(){};
	virtual ~INormalEstimation(){};

	/**
	 * @brief Estimates normals in a point clouds.
	 *
	 * Each ith point in the cloud with have a corresponding ith normal.
	 *
	 * @param[in] pointCloud The input point cloud. This data will not
	 * be modified.
	 * @param[out] estimatedNormals Resulting set of estimated normals.
	 *
	 */
	void estimateNormals(PointCloud3D* pointCloud, NormalSet3D* estimatedNormals);
};

}

#endif /* BRICS_3D_INORMALESTIMATION_H_ */

/* EOF */
