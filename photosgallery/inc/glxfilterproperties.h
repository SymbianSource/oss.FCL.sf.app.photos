/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Represents a set of filter properties
*
*/




#ifndef T_GLXFILTERPROPERTIES_H
#define T_GLXFILTERPROPERTIES_H

// INCLUDES
#include <glxmediaid.h>
#include <glxfiltergeneraldefs.h>
#include <mpxcollectionpath.h>	// for deep copying the path
// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION


class TGlxFilterProperties 
	{
public:
	inline TGlxFilterProperties () :
				iSortOrder(EGlxFilterSortOrderNotUsed),
				iSortDirection(EGlxFilterSortDirectionNotUsed),
				iIncludeCameraAlbum(ETrue),
				iMinCount(0),
				iContainsItem(0),
				iExcludeAnimation(EFalse),
				iLastCaptureDate(EFalse),
				iMaxCount(KMaxTUint),
				iThumbnailLoadability(0,0),
				iPath(NULL),
				iItemType(EGlxFilterVideoAndImages),
				iPromoteSystemItems(EFalse),
				iOrigin(EGlxFilterOriginNotUsed),
				iUri(NULL),
				iStartDate(0),
				iEndDate(0),
				iNoDRM(EFalse)
				  {}

    /**
    * Destructor
    */	
	inline ~TGlxFilterProperties ()
	    {	
        delete iPath;        
	    }

    /**
    * overloaded assignment operator
	*
	* @param aFilterProperties, assigned object
	* @return TGlxFilterProperties object after deep copying the path.
    */		
    inline TGlxFilterProperties& operator=(const TGlxFilterProperties& aFilterProperties) 
        { 
        if(this != &aFilterProperties)
            {
            iSortOrder = aFilterProperties.iSortOrder;
            iSortDirection = aFilterProperties.iSortDirection;
            iIncludeCameraAlbum = aFilterProperties.iIncludeCameraAlbum;
            iMinCount = aFilterProperties.iMinCount;
            iContainsItem = aFilterProperties.iContainsItem;
            iExcludeAnimation = aFilterProperties.iExcludeAnimation;
            iLastCaptureDate = aFilterProperties.iLastCaptureDate;
			iMaxCount = aFilterProperties.iMaxCount;
            iThumbnailLoadability = aFilterProperties.iThumbnailLoadability;
            iItemType = aFilterProperties.iItemType;
            iPromoteSystemItems = aFilterProperties.iPromoteSystemItems;
            iOrigin = aFilterProperties.iOrigin;
            iUri = aFilterProperties.iUri;
            iStartDate = aFilterProperties.iStartDate;
            iEndDate = aFilterProperties.iEndDate;
            iNoDRM = aFilterProperties.iNoDRM;
            
			// creating a copy of iPath
            if(aFilterProperties.iPath)
                {
                const CMPXCollectionPath* path = aFilterProperties.iPath; 
                iPath = CMPXCollectionPath::NewL(*path);                                    
                }
            else
                {
                iPath = NULL;
                }
            }        
        return *this;
        }
	/**
    * Copy constructor
	*
	* @param aFilterProperties, assigned object
    */
    inline TGlxFilterProperties(const TGlxFilterProperties& aFilterProperties)
        {
        iSortOrder = aFilterProperties.iSortOrder;
        iSortDirection = aFilterProperties.iSortDirection;
        iIncludeCameraAlbum = aFilterProperties.iIncludeCameraAlbum;
        iMinCount = aFilterProperties.iMinCount;
        iContainsItem = aFilterProperties.iContainsItem;
        iExcludeAnimation = aFilterProperties.iExcludeAnimation;
        iLastCaptureDate = aFilterProperties.iLastCaptureDate;
		iMaxCount = aFilterProperties.iMaxCount;
        iThumbnailLoadability = aFilterProperties.iThumbnailLoadability;
        iItemType = aFilterProperties.iItemType;
        iPromoteSystemItems = aFilterProperties.iPromoteSystemItems;
        iOrigin = aFilterProperties.iOrigin;
        iUri = aFilterProperties.iUri;
        iStartDate = aFilterProperties.iStartDate;
        iEndDate = aFilterProperties.iEndDate;
        iNoDRM = aFilterProperties.iNoDRM;
        
		// creating a copy of iPath
        if(aFilterProperties.iPath)
            {
            const CMPXCollectionPath* path = aFilterProperties.iPath; 
            iPath = CMPXCollectionPath::NewL(*path);                   
            }
        else
            {
            iPath = NULL;
            }
        }
public:
	TGlxFilterSortOrder iSortOrder;
	TGlxFilterSortDirection iSortDirection;
	TBool iIncludeCameraAlbum;
	TInt iMinCount;
	TGlxMediaId iContainsItem;
	TBool iExcludeAnimation;
	TBool iLastCaptureDate;
	TInt iMaxCount;
	TSize iThumbnailLoadability;
	CMPXCollectionPath* iPath; 
	TGlxFilterItemType iItemType;
	TBool iPromoteSystemItems;
	TGlxFilterOrigin iOrigin;
	const TDesC* iUri;
	TTime iStartDate;
	TTime iEndDate;
	TBool iNoDRM;
	};


#endif // T_GLXFILTERPROPERTIES_H
