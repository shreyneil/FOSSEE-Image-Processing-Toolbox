/***************************************************
Author : Shreyash Sharma
***************************************************/

#include <numeric>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/types_c.h"
#include <iostream>
using namespace cv;
using namespace std;
extern "C" {
   #include "api_scilab.h"
   #include "Scierror.h"
   #include "BOOL.h"
   #include <localization.h> 
   #include <sciprint.h> 
   #include "../common.h"
   double * readdoublearray_fromscilab(int);

   int opencv_Calibrate(char * fname, unsigned long fname_len) {
      //variables

      int i, j, k, n, m;
      int iRows = 0, iCols = 0;
      int * piAddr = NULL;
      double * pdblReal = NULL;
      double x, y, rms;

      CheckInputArgument(pvApiCtx, 2, 4);
      CheckOutputArgument(pvApiCtx, 4, 4);

      SciErr sciErr;
      vector < std::vector < Point2d > > imagepoints1(1), imagepoints2(1);
      vector < std::vector < Point3d > > objectPoints(1);
      //CvPoint2D64f*   imagePoints = NULL;
      //CvPoint3D64f*   objectPoints=NULL;
      //CvPoint2D64f*   reprojectPoints=NULL;

      //imagePoints1 = (CvPoint2D64f*)cvAlloc( numPoints *numImages * sizeof(CvPoint2D64f));
      //imagePoints2 = (CvPoint2D64f*)cvAlloc( numPoints *numImages * sizeof(CvPoint2D64f));

      Mat cameraMatrix1;
      Mat cameraMatrix2;
      Mat distCoeffsActual1, distCoeffsActual2;
      Mat rotationMatrix;
      Mat translationVector;
      int num;

      ////////parsing input

      n = * getNbInputArgument(pvApiCtx);
      //first
      sciErr = getVarAddressFromPosition(pvApiCtx, 1, & piAddr);
      if (sciErr.iErr) {
         printError( & sciErr, 0);
         return 0;
      }
      sciErr = getMatrixOfDouble(pvApiCtx, piAddr, & iRows, & iCols, & pdblReal);
      if (sciErr.iErr) {
         printError( & sciErr, 0);
         return 0;
      }
      //stores 
      sciprint("object points obtained\n");
      //objectPoints = (CvPoint3D64f*)cvAlloc( iRows *1* sizeof(CvPoint3D64f));
      for (i = 0; i < iRows; ++i)
         objectPoints[0].push_back(Point3d(double(pdblReal[(0 * iRows) + i]), double(pdblReal[(1 * iRows) + i]), 0.0));

      //     for( i = 0; i < iRows ;i++ )
      //     {

      //         x=pdblReal[(0 * iRows) + i];
      //         y=pdblReal[(1 * iRows) + i];
      //         (objectPoints+i)->x = float(x);
      //         (objectPoints+i)->y = float(y);
      //         (objectPoints+i)->z = 0.0;
      // }
      int pointCount = iRows;
      //2nd Input
      //sciprint("%d",objectPoints.type()); 
      sciprint("object points converted\n");
      sciErr = getVarAddressFromPosition(pvApiCtx, 2, & piAddr);
      if (sciErr.iErr) {
         printError( & sciErr, 0);
         return 0;
      }

      if (!isListType(pvApiCtx, piAddr)) {

         Scierror(999, "\nthe imagepoints1 Argument must be a list of points \n");
         return 0;
      }
      sciErr = getListItemNumber(pvApiCtx, piAddr, & num);
      if (sciErr.iErr) {
         printError( & sciErr, 0);

         return 0;
      }

      //get items from list
      sciprint("list number obtained\n");
      for (int i = 1; i <= num; i++) {

         sciErr = getMatrixOfDoubleInList(pvApiCtx, piAddr, i, & iRows, & iCols, & pdblReal);

         if (sciErr.iErr) {
            printError( & sciErr, 0);
            return 0;
         }

         for (j = 0; j < iRows; ++j)
            imagepoints1[i - 1].push_back(Point2d(double(pdblReal[(0 * iRows) + j]), double(pdblReal[(1 * iRows) + j])));

      }

      //3rd input
      /*sciprint("\n");
      sciErr = getVarAddressFromPosition(pvApiCtx, 3, &piAddr);
      if (sciErr.iErr)
     {
        printError(&sciErr, 0);
        return 0;
    }

    if(!isListType(pvApiCtx, piAddr))
        {
            
            Scierror(999,"\nthe imagepoints2 Argument must be a list of points \n");
            return 0;
        }
    sciErr = getListItemNumber(pvApiCtx, piAddr, &num);
    if(sciErr.iErr)
    {
        printError(&sciErr, 0);
        
        return 0;
    }

    //get items from list
    //sciprint("fisheye\n");
    for(int i=1;i<=num;i++)
    {
    	sciErr=getMatrixOfDoubleInList(pvApiCtx, piAddr,i,&iRows, &iCols, &pdblReal);
    	if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

    	for(j = 0; j < iRows; ++j)
            imagepoints2[i-1].push_back( Point2d(double(pdblReal[(0 * iRows) + j]), double(pdblReal[(1 * iRows) + j])));


    }*/
      sciprint("fisheye\n");
      ///Getting optional arguments
      int p;
      switch (n) {
      case 3:
         pdblReal = readdoublearray_fromscilab(3);
         for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
               cameraMatrix1.at < double > (i, j) = pdblReal[(j * 3) + i];
         break;

      case 4:
         pdblReal = readdoublearray_fromscilab(3);
         for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
               cameraMatrix1.at < double > (i, j) = pdblReal[(j * 3) + i];

         sciErr = getVarAddressFromPosition(pvApiCtx, 4, & piAddr);
         if (sciErr.iErr) {
            printError( & sciErr, 0);
            return 0;
         }
         sciErr = getMatrixOfDouble(pvApiCtx, piAddr, & iRows, & iCols, & pdblReal);
         if (sciErr.iErr) {
            printError( & sciErr, 0);
            return 0;
         }

         if (iRows == 1)
            p = iCols;
         else if (iCols == 1)
            p = iRows;
         else {
            Scierror(1, "Distortion Points matrix (arg 5) must be a 1 X N or N X 1 matrix");
            return 0;
         }
         if (p == 4 or p == 5 or p == 8);
         else {
            Scierror(1, " N must be 4 or 5 or 8");
            return 0;
         }
         for (i = 0; i < p; i++)
            distCoeffsActual1.at < double > (0, i) = pdblReal[i];

         break;
      }
      /*case 6:

    		pdblReal=readdoublearray_fromscilab(4);
    			for(i=0;i<3;i++)
        		for(j=0;j<3;j++)
            		cameraMatrix1.at<double>(i,j) = pdblReal[(j * 3) + i];

    			sciErr = getVarAddressFromPosition(pvApiCtx,5,&piAddr);
    		if (sciErr.iErr)
    		{
        		printError(&sciErr, 0);
        		return 0;
    		}
    		sciErr = getMatrixOfDouble(pvApiCtx, piAddr, &iRows, &iCols, &pdblReal);
    		if(sciErr.iErr)
    		{
        		printError(&sciErr, 0);
        		return 0;
    		}
    
			    if(iRows== 1)
			        p = iCols;
			    else if (iCols == 1)
			        p = iRows;
			    else{
			        Scierror(1,"Distortion Points matrix (arg 5) must be a 1 X N or N X 1 matrix");
			        return 0;
			    }
			    if(p==4 or p==5 or p==8);
			    else{
			        Scierror(1," N must be 4 or 5 or 8");
			        return 0;
			    }
				    for(i=0;i<p;i++)
				        distCoeffsActual1.at<double>(0,i) = pdblReal[i];
        
    	break;

    	case 7:
    			pdblReal=readdoublearray_fromscilab(4);
    			for(i=0;i<3;i++)
        		for(j=0;j<3;j++)
            		cameraMatrix1.at<double>(i,j) = pdblReal[(j * 3) + i];

    			sciErr = getVarAddressFromPosition(pvApiCtx,5,&piAddr);
    		if (sciErr.iErr)
    		{
        		printError(&sciErr, 0);
        		return 0;
    		}
    		sciErr = getMatrixOfDouble(pvApiCtx, piAddr, &iRows, &iCols, &pdblReal);
    		if(sciErr.iErr)
    		{
        		printError(&sciErr, 0);
        		return 0;
    		}
    
			    if(iRows== 1)
			        p = iCols;
			    else if (iCols == 1)
			        p = iRows;
			    else{
			        Scierror(1,"Distortion Points matrix (arg 5) must be a 1 X N or N X 1 matrix");
			        return 0;
			    }
			    if(p==4 or p==5 or p==8);
			    else{
			        Scierror(1," N must be 4 or 5 or 8");
			        return 0;
			    }
				    for(i=0;i<p;i++)
				        distCoeffsActual1.at<double>(0,i) = pdblReal[i];
		        pdblReal=readdoublearray_fromscilab(6);
		    	for(i=0;i<3;i++)
		        for(j=0;j<3;j++)
		            cameraMatrix2.at<double>(i,j) = pdblReal[(j * 3) + i];
		        sciErr = getVarAddressFromPosition(pvApiCtx,7,&piAddr);
    		if (sciErr.iErr)
    		{
        		printError(&sciErr, 0);
        		return 0;
    		}
    		sciErr = getMatrixOfDouble(pvApiCtx, piAddr, &iRows, &iCols, &pdblReal);
    		if(sciErr.iErr)
    		{
        		printError(&sciErr, 0);
        		return 0;
    		}
    
			    if(iRows== 1)
			        p = iCols;
			    else if (iCols == 1)
			        p = iRows;
			    else{
			        Scierror(1,"Distortion Points matrix (arg 5) must be a 1 X N or N X 1 matrix");
			        return 0;
			    }
			    if(p==4 or p==5 or p==8);
			    else{
			        Scierror(1," N must be 4 or 5 or 8");
			        return 0;
			    }
				    for(i=0;i<p;i++)
				        distCoeffsActual2.at<double>(0,i) = pdblReal[i];



    	break;
    }*/
      // CvMat _objectPoints = cvMat(1, pointCount, CV_64FC3, objectPoints);
      //sciprint("fisheye\n");
      try {
         Size imageSize(960, 600);
         rms = fisheye::calibrate(objectPoints, imagepoints1, imageSize,
            cameraMatrix1, distCoeffsActual1,
            rotationMatrix, translationVector, fisheye::CALIB_FIX_SKEW | fisheye::CALIB_RECOMPUTE_EXTRINSIC,
            TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, DBL_EPSILON));

      }
	  catch (cv::Exception & e) 
        {

            Scierror(999,e.what());
            return 0;

        }

      // CV_CALIB_FIX_ASPECT_RATIO +
      // CV_CALIB_ZERO_TANGENT_DIST +
      // CV_CALIB_SAME_FOCAL_LENGTH +
      // CV_CALIB_RATIONAL_MODEL +
      // CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5,

      sciprint("%d\n%d\n", rotationMatrix.rows, rotationMatrix.cols);
      sciprint("%d\n%d\n", translationVector.rows, translationVector.cols);
      sciprint("Calibration done with RMS error=%f\n", rms);

      ///Return Arguments to Scilab
      sciprint("yo7");
      double * pstdata1 = NULL, * pstdata2 = NULL, * pstdata3 = NULL, * pstdata4 = NULL, * pstdata5 = NULL, * pstdata6 = NULL;
      pstdata1 = (double * ) malloc(sizeof(double) * 3 * 3);

      for (i = 0; i < 3; i++)
         for (j = 0; j < 3; j++)
            pstdata1[(j * 3) + i] = cameraMatrix1.at < double > (i, j);
      sciprint("yo8");
      sciErr = createMatrixOfDouble(pvApiCtx, nbInputArgument(pvApiCtx) + 1, 3, 3, pstdata1);
      if (sciErr.iErr) {
         printError( & sciErr, 0);
         return 0;
      }
      sciprint("yo9");
      pstdata2 = (double * ) malloc(sizeof(double) * 4 * 1);

      for (i = 0; i < 4; i++)
         pstdata2[i] = distCoeffsActual1.at < double > (0, i);

      sciErr = createMatrixOfDouble(pvApiCtx, nbInputArgument(pvApiCtx) + 2, 1, 4, pstdata2);
      if (sciErr.iErr) {
         printError( & sciErr, 0);
         return 0;
      }

      pstdata5 = (double * ) malloc(sizeof(double) * rotationMatrix.rows * rotationMatrix.cols);

      for (i = 0; i < rotationMatrix.rows; i++)
         for (j = 0; j < rotationMatrix.cols; j++) {

            pstdata5[(i * rotationMatrix.rows) + j] = rotationMatrix.at < double > (i, j);

         }

      sciErr = createMatrixOfDouble(pvApiCtx, nbInputArgument(pvApiCtx) + 3, rotationMatrix.rows, rotationMatrix.cols, pstdata5);
      if (sciErr.iErr) {
         printError( & sciErr, 0);
         return 0;
      }

      pstdata6 = (double * ) malloc(sizeof(double) * translationVector.rows * translationVector.cols);

      for (i = 0; i < translationVector.rows; i++)
         for (j = 0; j < translationVector.cols; j++) {

            pstdata6[(i * translationVector.rows) + j] = translationVector.at < double > (i, j);

         }

      sciErr = createMatrixOfDouble(pvApiCtx, nbInputArgument(pvApiCtx) + 4, translationVector.rows, translationVector.cols, pstdata6);
      if (sciErr.iErr) {
         printError( & sciErr, 0);
         return 0;
      }

      for (int i = 1; i <= 4; i++)
         AssignOutputVariable(pvApiCtx, i) = nbInputArgument(pvApiCtx) + i;
      //Returning the Output Variables as arguments to the Scilab environment
      ReturnArguments(pvApiCtx);
      return 0;

   }

   double * readdoublearray_fromscilab(int cnt) {

      SciErr sciErr;
      int * piAddr = NULL;
      double val = 0;
      int intErr;
      int iRows, iCols;
      double * pdblReal;

      sciErr = getVarAddressFromPosition(pvApiCtx, cnt, & piAddr);
      if (sciErr.iErr) {
         printError( & sciErr, 0);
         return 0;
      }

      sciErr = getMatrixOfDouble(pvApiCtx, piAddr, & iRows, & iCols, & pdblReal);
      if (sciErr.iErr) {
         printError( & sciErr, 0);
         return 0;
      }
      if (isDoubleType(pvApiCtx, piAddr))
         return pdblReal;
      else {
         sciprint("Error: the %d argument is not of the type double!", cnt);
         return 0;

      }

   }

}
