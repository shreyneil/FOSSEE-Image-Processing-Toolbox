/***************************************************
Author : Shreyash Sharma
***************************************************/
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/core.hpp>
#include <string.h>


using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
using namespace cv::ml;
extern "C"
{
    #include "api_scilab.h"	
    #include "Scierror.h"
    #include "BOOL.h"
    #include <localization.h>
    #include "sciprint.h"
    int opencv_traindtreeClassifier(char *fname, unsigned long fname_len)
    {
        // Error management variables
        SciErr sciErr;

        //------Local variables------//
        int upright = 1;
        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
        Ptr<SURF> detector = SURF::create(400, 4, 2, 1, upright);
        Ptr<DescriptorExtractor> extractor = detector;
        Ptr<BOWImgDescriptorExtractor>bowDE = makePtr<BOWImgDescriptorExtractor>(extractor,matcher);
        char *fileName = NULL;
        Mat dictionary,inp,features;
        vector<KeyPoint> keyPoints;


        int *piAddr = NULL;
        int *piChild = NULL;
        double iRet=0, iRet1=0, iRet4=0, iRet5=0, iRet8 = 0, iRet9 = 0;
        int  iRet2 = 0 , iRet3 = 0, iRet6 =0 ,iRet7 = 0,iRet13=0, md = 20, msc = 10,max_cat = 10 , cv_f = 1;
        int iType = 0;
	int iRows = 0;
	int iCols = 0;
	int i,j;
	char* classifierName = NULL;
        double *pdblReal = NULL;
        double *priors = NULL;
        double aplpha = 0,beta = 0, reg_acc = 0, param1= 0 , param2 = 0;
        bool save = 1, use_surr = false, use_1se = true , is_prune = true;
        char *trainmethod = NULL;
        int iRows1, iCols1;
        char **pstData = NULL;
        int *piLen = NULL;
        int *count = NULL;
        char **description = NULL;
        char ***location = NULL;
        char *bagOfFeaturesLocation = NULL;
        int descriptionCount;
        char *classifierLocation = "classifier.yml";
        char *objectType = "classifier"; 
         
     

        //------Check number of parameters------//
        CheckInputArgument(pvApiCtx, 3, 13);
        CheckOutputArgument(pvApiCtx, 1, 1);

        //------Get input arguments------//
        sciErr = getVarAddressFromPosition(pvApiCtx, 1, &piAddr);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isListType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #1 is not of type imageSet.\n");
            return 0;
        }

        // Extracting object type and checking if type is imageSet
        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 1, &iRows, &iCols, NULL, NULL);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        piLen = (int*)malloc(sizeof(int) * iRows * iCols);

        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 1, &iRows, &iCols, piLen, NULL);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        pstData = (char**)malloc(sizeof(char*) * iRows * iCols);

        for(int iter = 0 ; iter < iRows * iCols ; iter++)
        {
            pstData[iter] = (char*)malloc(sizeof(char) * (piLen[iter] + 1));//+ 1 for null termination
        }

        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 1, &iRows, &iCols, piLen, pstData);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        if(!(strcmp(pstData[0],"imageSet")==0))
        {
            Scierror(999, "Error: The input argument #1 is not of type imageSet.\n");
            return 0;
        }

        // Extracting Description attribute of input argument
        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 2, &iRows, &iCols, NULL, NULL);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        piLen = (int*)malloc(sizeof(int) * iRows * iCols);

        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 2, &iRows, &iCols, piLen, NULL);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        description = (char**)malloc(sizeof(char*) * iRows * iCols);

        for(int iter = 0 ; iter < iRows * iCols ; iter++)
        {
            description[iter] = (char*)malloc(sizeof(char) * (piLen[iter] + 1));//+ 1 for null termination
        }

        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 2, &iRows, &iCols, piLen, description);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        descriptionCount = iRows;

        // Extracting Count attribute of input argument
        sciErr = getMatrixOfInteger32InList(pvApiCtx, piAddr, 3, &iRows, &iCols, &count);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        location = (char***) malloc(sizeof(char**) * descriptionCount);
        sciErr = getListItemAddress(pvApiCtx, piAddr, 4, &piChild);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        for(int iter = 1; iter<=descriptionCount; iter++)
        {
            sciErr = getMatrixOfStringInList(pvApiCtx, piChild, iter, &iRows, &iCols, NULL, NULL);
            if(sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }

            piLen = (int*)malloc(sizeof(int) * iRows * iCols);

            sciErr = getMatrixOfStringInList(pvApiCtx, piChild, iter, &iRows, &iCols, piLen, NULL);
            if(sciErr.iErr)
            {
                printError(&sciErr, 0);
                return 0;
            }

            location[iter-1] = (char**)malloc(sizeof(char*) * iRows * iCols);

            for(int colIter = 0 ; colIter < iRows * iCols ; colIter++)
            {
                location[iter-1][colIter] = (char*)malloc(sizeof(char) * (piLen[colIter] + 1));//+ 1 for null termination
            }

            sciErr = getMatrixOfStringInList(pvApiCtx, piChild, iter, &iRows, &iCols, piLen, location[iter-1]);
            if(sciErr.iErr)
            {
                printError(&sciErr, 0);

                return 0;
            }
        }
        // Second argument
        sciErr = getVarAddressFromPosition(pvApiCtx, 2, &piAddr);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isListType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #2 is not of type bagOfFeatures.\n");
            return 0;
        }

        // Extracting object type and checking if type is bagOfFeatures
        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 1, &iRows, &iCols, NULL, NULL);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        piLen = (int*)malloc(sizeof(int) * iRows * iCols);

        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 1, &iRows, &iCols, piLen, NULL);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        pstData = (char**)malloc(sizeof(char*) * iRows * iCols);

        for(int iter = 0 ; iter < iRows * iCols ; iter++)
        {
            pstData[iter] = (char*)malloc(sizeof(char) * (piLen[iter] + 1));//+ 1 for null termination
        }

        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 1, &iRows, &iCols, piLen, pstData);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        if(!(strcmp(pstData[0],"bagOfFeatures")==0))
        {
            Scierror(999, "Error: The input argument #2 is not of type bagOfFeatures.\n");
            return 0;
        }

    
        // Extracting name of next argument takes three calls to getMatrixOfString
        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 2, &iRows, &iCols, NULL, NULL);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        piLen = (int*) malloc(sizeof(int) * iRows * iCols);

        sciErr = getMatrixOfStringInList(pvApiCtx,  piAddr, 2,  &iRows,  &iCols,  piLen,  NULL);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        pstData = (char**) malloc(sizeof(char*) * iRows * iCols);
        for(int iterPstData = 0; iterPstData < iRows * iCols; iterPstData++)
        {
            pstData[iterPstData] = (char*) malloc(sizeof(char) * piLen[iterPstData] + 1);
        }

        sciErr = getMatrixOfStringInList(pvApiCtx, piAddr, 2, &iRows, &iCols, piLen, pstData);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        if(iRows!=1 || iCols!=1)
        {
            Scierror(999, "1x1 Matrix expected for bagOfFeatures argument.");
            return 0;
        }
        bagOfFeaturesLocation = pstData[0];
        //sciprint("%s",classifierName);
        // Handling 3rd argument.
		sciErr = getVarAddressFromPosition(pvApiCtx, 3, &piAddr);
		if(sciErr.iErr)
		{
			printError(&sciErr, 0);
			return 0;
		}
        if(isStringType(pvApiCtx, piAddr))
        {
    		if(isScalar(pvApiCtx, piAddr))
            {
                char* stringData = NULL;

                iRet13 = getAllocatedSingleString(pvApiCtx, piAddr, &stringData);
                if(!iRet13)
                {
                    
                    classifierName = stringData;
                }
                else
                {
                    Scierror(999, "Error: Could not assign input arg #3 to local var.\n");
                    return 0;
                }
            }
           	else
    	    {
    		    Scierror(999, "Error: The input argument #3 is not scalar type.\n");
        		return 0;
    	    }
        }
        else 
        {
            Scierror(999, "Error: The input argument #3 is not a string.\n");
            return 0;
        }
         
        // 4th arguement
        if(*getNbInputArgument(pvApiCtx) == 4)
       
        { 
          sciErr = getVarAddressFromPosition(pvApiCtx, 4, &piAddr);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isBooleanType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #4 is not of type save.\n");
            return 0;
        }


        //extracting bool type arguement
        if(isBooleanType(pvApiCtx, piAddr))
        {
            
        if(isScalar(pvApiCtx, piAddr))
        {
                  int iBool = 0;
            
               iRet1 = getScalarBoolean(pvApiCtx, piAddr, &iBool);
             
               if(!iRet1)
              {
              
                save = iBool;
              }
        }
        
        }
        
        }

        // 5th arguement   
        if(*getNbInputArgument(pvApiCtx) == 5)
       
        { 
          sciErr = getVarAddressFromPosition(pvApiCtx, 5, &piAddr);
         if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isIntegerType(pvApiCtx, piAddr)||!isDoubleType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #5 is not of type md.\n");
            return 0;
        }


        //extracting double type arguement
        else if(isIntegerType(pvApiCtx, piAddr)||isDoubleType(pvApiCtx, piAddr))
	{           
                        if(isScalar(pvApiCtx, piAddr))
			{
				double cData0  = 0;
				iRet2 = getScalarDouble(pvApiCtx, piAddr, &cData0);
				if(!iRet2)
				{
				       cData0 = md;
				}
                        }
         }
        
         }
         
         // 6th arguement   
        if(*getNbInputArgument(pvApiCtx) == 6)
       
        { 
          sciErr = getVarAddressFromPosition(pvApiCtx, 6, &piAddr);
         if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isIntegerType(pvApiCtx, piAddr)||isDoubleType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #6 is not of type msc.\n");
            return 0;
        }


        //extracting integer type arguement
        else if(isIntegerType(pvApiCtx, piAddr)||isDoubleType(pvApiCtx, piAddr))
	{           
                        if(isScalar(pvApiCtx, piAddr))
			{
				double cData  = 0;
				iRet3 = getScalarDouble(pvApiCtx, piAddr, &cData);
				if(!iRet3)
				{
					cData = msc;
				}
                        }
         }
        
         }
          

         
         // 7th arguement   
        if(*getNbInputArgument(pvApiCtx) == 7)
       
        { 
          sciErr = getVarAddressFromPosition(pvApiCtx, 7, &piAddr);
         if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isDoubleType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #7 is not of type reg_acc.\n");
            return 0;
        }


        //extracting double type arguement
        else if(isDoubleType(pvApiCtx, piAddr))
	{
		if(isScalar(pvApiCtx, piAddr))
		{
			
                        double dblReal;
			
			iRet4= getScalarDouble(pvApiCtx, piAddr, &dblReal);
				if(!iRet4)
				{
					reg_acc = iRet4;
		}		} 
         }
        
         }
         
         // 8th arguement
        if(*getNbInputArgument(pvApiCtx) == 8)
       
        { 
          sciErr = getVarAddressFromPosition(pvApiCtx, 8, &piAddr);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isBooleanType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #8 is not of type use_surrogate.\n");
            return 0;
        }


        //extracting bool type arguement
        if(isBooleanType(pvApiCtx, piAddr))
        {
            
        if(isScalar(pvApiCtx, piAddr))
        {
                  int iBool1 = 0;
            
               iRet5 = getScalarBoolean(pvApiCtx, piAddr, &iBool1);
             
               if(!iRet5)
              {
              
                use_surr = iBool1;
              }
        }
        
        }
        
        }	
          
            // 9th arguement   
        if(*getNbInputArgument(pvApiCtx) == 9)
       
        { 
          sciErr = getVarAddressFromPosition(pvApiCtx, 9, &piAddr);
         if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isIntegerType(pvApiCtx, piAddr)||!isDoubleType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #9 is not of type maximum categories.\n");
            return 0;
        }


        //extracting integer type arguement
        else if(isIntegerType(pvApiCtx, piAddr)||isDoubleType(pvApiCtx, piAddr))
	{           
                        if(isScalar(pvApiCtx, piAddr))
			{
				double cData1  = 0;
				iRet6 = getScalarDouble(pvApiCtx, piAddr, &cData1);
				if(!iRet6)
				{
					cData1 = max_cat;
				}
                        }
         }
        
         }
      
            // 10th arguement   
        if(*getNbInputArgument(pvApiCtx) == 10)
       
        { 
          sciErr = getVarAddressFromPosition(pvApiCtx, 10, &piAddr);
         if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isIntegerType(pvApiCtx, piAddr)||!isDoubleType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #10 is not of type cv folds.\n");
            return 0;
        }


        //extracting integer type arguement
        else if(isIntegerType(pvApiCtx, piAddr)||isDoubleType(pvApiCtx, piAddr))
	{           
                        if(isScalar(pvApiCtx, piAddr))
			{
				double cData2  = 0;
				iRet7 = getScalarDouble(pvApiCtx, piAddr, &cData2);
				if(!iRet7)
				{
					cData2 = cv_f;
				}
                        }
         }
        
         }
         // 11th arguement
        if(*getNbInputArgument(pvApiCtx) == 11)
       
        { 
          sciErr = getVarAddressFromPosition(pvApiCtx, 11, &piAddr);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isBooleanType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #11 is not of type use_1se.\n");
            return 0;
        }


        //extracting bool type arguement
        if(isBooleanType(pvApiCtx, piAddr))
        {
            
        if(isScalar(pvApiCtx, piAddr))
        {
                  int iBool2 = 0;
            
               iRet8 = getScalarBoolean(pvApiCtx, piAddr, &iBool2);
             
               if(!iRet8)
              {
              
                use_1se = iBool2;
              }
        }
        
        }
        
        }
        
        //  12th arguement
        if(*getNbInputArgument(pvApiCtx) == 12)
       
        { 
          sciErr = getVarAddressFromPosition(pvApiCtx, 12, &piAddr);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        if(!isBooleanType(pvApiCtx, piAddr))
        {
            Scierror(999, "Error: The input argument #12 is not of type ispruned.\n");
            return 0;
        }


        //extracting bool type arguement
        if(isBooleanType(pvApiCtx, piAddr))
        {
            
        if(isScalar(pvApiCtx, piAddr))
        {
                  int iBool3 = 0;
            
               iRet9 = getScalarBoolean(pvApiCtx, piAddr, &iBool3);
             
               if(!iRet9)
              {
              
                is_prune = iBool3;
              }
        }
        
        }
        
        }
         
         //13th arguement
       
        /*if(*getNbInputArgument(pvApiCtx) == 13)
       
        {
  
          sciErr = getVarAddressFromPosition(pvApiCtx, 13, &piAddr);
	
        if(sciErr.iErr)
	{
		printError(&sciErr, 0);
		return 0;
	}
        
        sciErr = getVarType(pvApiCtx, piAddr, &iType);
	if(sciErr.iErr || iType != sci_matrix)
	{
		printError(&sciErr, 0);
		return 0;
	}
         
        
        //extracting the argument
        else
	{
		if(iRows1 == descriptionCount  && iCols1 == 1)
                 //get size and data from Scilab memory
		{
                  sciErr = getMatrixOfDouble(pvApiCtx, piAddr, &iRows1, &iCols1, &priors);
	        }
               
                if(sciErr.iErr)
               { 
                  printError(&sciErr, 0);
                  
                  return 0;
		      
               }
                 
        }
       
        }*/
        

        classifierLocation = classifierName;
        strcat(classifierLocation, ".yml");
        
          
        //------Actual processing------//
        FileStorage fs(bagOfFeaturesLocation, FileStorage::READ);
        fs["dictionary"] >> dictionary;
        fs.release();
        if(dictionary.rows==0 || dictionary.cols==0)
        {
            sciprint("Error: The provided file for bagOfFeatures may be invalid.\n");
        }
        sciprint("Training an image category classifier for %d categories.\n",descriptionCount);
        sciprint("-------------------------------------------------------\n\n");
        for(int i=0;i<descriptionCount;i++)
        {
            sciprint("# Category %d: %s\n",i+1,description[i]);
        }
        sciprint("\n");
        int dictionarySize = dictionary.rows;
        Mat labels(0, 1, CV_32FC1);
        Mat trainingData(0, dictionarySize, CV_32FC1);
        bowDE->setVocabulary(dictionary);
        for(int i=0; i<descriptionCount;i++)
        {
            sciprint("# Encoding features for Category %d ...",i+1);
            for(int j=0; j<count[i]; j++)
            {
                features.release();
                keyPoints.clear();
                fileName = location[i][j];
                inp = imread(fileName);
                detector->detect(inp,keyPoints);
                bowDE->compute(inp,keyPoints,features);
                trainingData.push_back(features);
                labels.push_back((float) i);
            }
            sciprint("done.\n");
        }
        sciprint("\n# Training the category classifier...");
               //handling matrix
       
       Mat m1 = Mat(descriptionCount,1,CV_32FC1,&priors);
       
       

        
        Ptr<DTrees> m = DTrees::create();
        sciprint("done.\n");

  
        try
        {
           m->setMaxDepth(md);

        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;
        }
        
        
        
        try
        {
           m->setMinSampleCount(msc);
        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;
        }      
            
        try
        {
          m->setRegressionAccuracy(reg_acc);
        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;
        }
           try
        {
          m->setUseSurrogates(use_surr);
        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;
        }
        
           try
        {
          m->setMaxCategories(max_cat);

        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;        
        }
        
          try
        {
          m->setCVFolds(cv_f);

        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;
        }
         
          try
        {
          m->setUse1SERule(use_1se);

        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;
        }
        
          try
        {
          m->setTruncatePrunedTree(is_prune);

        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;
        }
        

         
        try
        {
           m->setPriors(m1);


        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;        
        }
     
        try
        {
           m->train(trainingData, cv::ml::ROW_SAMPLE, labels);


        }
        catch(cv::Exception &e)
        {
           Scierror(999,e.what());
           return 0;
        }
        
        transpose(trainingData,trainingData);
     
        if(save)
        {
        	try
        	{
       			m->save(classifierLocation);

        	}
        	catch(cv::Exception &e)
        	{
           		Scierror(999,e.what());
                        return 0;
        	}
        }

        sciErr = createList(pvApiCtx, nbInputArgument(pvApiCtx) + 1, 4, &piAddr);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        sciErr = createMatrixOfStringInList(pvApiCtx, nbInputArgument(pvApiCtx)+1, piAddr, 1, 1, 1, &objectType);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        sciErr = createMatrixOfStringInList(pvApiCtx, nbInputArgument(pvApiCtx)+1, piAddr, 2, 1, 1, &classifierLocation);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        sciErr = createMatrixOfStringInList(pvApiCtx, nbInputArgument(pvApiCtx)+1, piAddr, 3, 1, 1, &bagOfFeaturesLocation);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        sciErr = createMatrixOfStringInList(pvApiCtx, nbInputArgument(pvApiCtx)+1, piAddr, 4, descriptionCount, 1, description);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        //------Return Arguments------//
        AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx)+1;
        ReturnArguments(pvApiCtx);
        sciprint("\n# Done");
        return 0;
    }
    /* ==================================================================== */
}	
