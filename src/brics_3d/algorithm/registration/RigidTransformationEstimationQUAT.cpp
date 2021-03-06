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

#include "RigidTransformationEstimationQUAT.h"
#include <iostream>

#define OPENMP_NUM_THREADS 4 //only to make code compilable
#include "6dslam/src/icp6Dquat.h"

using std::cout;
using std::endl;

namespace brics_3d {

RigidTransformationEstimationQUAT::RigidTransformationEstimationQUAT() {


}

RigidTransformationEstimationQUAT::~RigidTransformationEstimationQUAT() {

}

double RigidTransformationEstimationQUAT::estimateTransformation(std::vector<CorrespondencePoint3DPair>* pointPairs, IHomogeneousMatrix44* resultTransformation) {
	double resultError = -1.0;
	bool quiet = true;
	icp6Dminimizer *errorMinimizer = new icp6D_QUAT(quiet);

	vector<PtPair> minimizerPointPairs;
	double* alignxf = new double [16];
	double centroid_m[3] = {0.0, 0.0, 0.0};
	double centroid_d[3] = {0.0, 0.0, 0.0};

	/* convert data structures */
	for (unsigned int i = 0; i < pointPairs->size(); ++i) {
		double firstPoint[3];
		firstPoint[0] = (*pointPairs)[i].firstPoint.getX();
		firstPoint[1] = (*pointPairs)[i].firstPoint.getY();
		firstPoint[2] = (*pointPairs)[i].firstPoint.getZ();

		double secondPoint[3];
		secondPoint[0] = (*pointPairs)[i].secondPoint.getX();
		secondPoint[1] = (*pointPairs)[i].secondPoint.getY();
		secondPoint[2] = (*pointPairs)[i].secondPoint.getZ();

		PtPair pair;
		pair.p1 = firstPoint;
		pair.p2 = secondPoint;
		minimizerPointPairs.push_back(pair);

		/* calculate centroids */
		centroid_m[0] += firstPoint[0]; //TODO: why flipped  _m and _d in 6DSLAM code?!?
		centroid_m[1] += firstPoint[1];
		centroid_m[2] += firstPoint[2];
		centroid_d[0] += secondPoint[0];
		centroid_d[1] += secondPoint[1];
		centroid_d[2] += secondPoint[2];
	}

	centroid_m[0] /= minimizerPointPairs.size();
	centroid_m[1] /= minimizerPointPairs.size();
	centroid_m[2] /= minimizerPointPairs.size();
	centroid_d[0] /= minimizerPointPairs.size();
	centroid_d[1] /= minimizerPointPairs.size();
	centroid_d[2] /= minimizerPointPairs.size();

//	cout << "centroid of pointCloud1 " << centroid_m[0] << ", " << centroid_m[1] << ", " << centroid_m[2] << endl; //DBG output
//	cout << "centroid of pointCloud2 " << centroid_d[0] << ", " << centroid_d[1] << ", " << centroid_d[2] << endl;

	resultError = errorMinimizer->Point_Point_Align(minimizerPointPairs, alignxf, centroid_m, centroid_d);
//	for (int i = 0; i < 16; ++i) { // DBG output
//		cout << alignxf[i];
//		if (((i+1)%4) == 0) {
//			cout << endl;
//		}
//	}

	double* resultRawData;
	resultRawData = resultTransformation->setRawData();
	for (int i = 0; i < 16; ++i) {
		resultRawData[i] = alignxf[i];
	}

	delete errorMinimizer;

	return resultError;
}

}

/* EOF */
