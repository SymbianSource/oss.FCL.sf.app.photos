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
* Description:    This class browses file system
*
*/





#ifndef C_GLXCOLLECTIONPLUGINMONTHS_H
#define C_GLXCOLLECTIONPLUGINMONTHS_H

// INCLUDES
#include <e32cmn.h>
#include "glxcollectionpluginbase.h"

/**
 * @internal reviewed 14/06/2007 by Alex Birkett
 */

// FORWARD DECLARATIONS

// CONSTANTS
const TInt KGlxCollectionMonthsLevel = KGlxCollectionRootLevel + 1;
const TInt KGlxCollectionMonthContentsLevel = KGlxCollectionRootLevel + 2;
const TInt KGlxCollectionMonthFSLevel = KGlxCollectionRootLevel + 3;
// CLASS DECLARATION

/**
 *  CGlxCollectionPluginMonths class 
 *
 *  Plug-in basically provides browsing.
 *	@ingroup collection_component_design
 */
NONSHARABLE_CLASS(CGlxCollectionPluginMonths) : public CGlxCollectionPluginBase
    {
public: // Constructors and destructor
	/**
    * Two-phased constructor
    *
    * @param aObs observer
    * @return object of constructed
    */
    static CGlxCollectionPluginMonths* NewL(TAny* aObs);

    /**
    * Destructor
    */
    ~CGlxCollectionPluginMonths();
    
private: 
    /**
    * Constructor
    * @param aObs MMPXCollectionPluginObserver instance
    */
    CGlxCollectionPluginMonths(MMPXCollectionPluginObserver* aObs);
    /**
    * 2nd stage Constructor
    */
    void ConstructL();
    
private:
	/**
	 * Checks to see if any additional attributes are required to construct required
	 * CPI specific attribute
	 *
	 * @param aCpiAttribute CPI specific attribute to be constructed later
	 * @param aAttributeArray modifiable list of attributes to be retrieved from data source
	 */	 
	void CpiAttributeAdditionalAttributes(const TMPXAttribute& aCpiAttribute,
			RArray<TMPXAttribute>& aAttributeArray);

	/**
	 * Modifies the response to include all requested CPI specific attributes or Leaves.
	 *
	 * @param aResponse Data Source constructed response to which CPI specific attributes should be added (also contains any required data)
	 * @param aCpiAttributes list of CPI specific attributes to be constructed
	 */	 
	void HandleCpiAttributeResponseL(CMPXMedia* aResponse,
			TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> aMediaIds);

	/**
	 * Modifies the response to include all requested CPI specific attributes or Leaves.
	 *
	 * @param aResponse Data Source constructed response to which CPI specific attributes should be added (also contains any required data)
	 * @param aCpiAttributes list of CPI specific attributes to be constructed
	 * @param aMediaId CPI specific media Id
	 */	 
    void HandleCpiAttributeResponseL(CMPXMedia* aResponse,
    		TArray<TMPXAttribute> aCpiAttributes, TGlxMediaId aMediaId);

	TBool IsUpdateMessageIgnored(CMPXMessage& aMessage);

    TGlxFilterProperties DefaultFilter(TInt aLevel);
    
private:
    /**
     * enum TMonthStringType
     *
     * This is mainly passed as a param to be used by the method 
     * GetMonthNameAsStringLC().
     * GetMonthNameAsStringLC() manipulates and returns the months name in 
     * Title format or sub-title format based on this enum which is passed as 
     * param.
     */  
    enum TMonthStringType
        {
        EMonthNameAsSubtitle, /// Month name is desired in sub-title format
        EMonthNameAsTitle  /// Month name is desired in title format
        };

private:
    /**
     * Utility method to take a TMonth param, and convert it into a string. This
     * string is returned to the calling method. The returned string can contain
     * the month name in sub-title or title format. Subtitle format is used in 
     * the list views to signify months ("Jan", "Feb", ... ). Title format is 
     * used in mpnth-grid views to name the view ("January", "February", ...).     
     *
     * @param aMonth The month whose name is desired as a string.
     * @param aStrType enum specified by calling method to tell how it wants the 
     * month string to be.   
     */  
    HBufC* GetMonthNameAsStringLC( const TMonth& aMonth,
            const TMonthStringType& aStrType);
    };

#endif      // C_GLXCOLLECTIONPLUGINMONTHS_H

