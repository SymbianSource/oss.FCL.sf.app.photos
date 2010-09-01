/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The effect info class
 *
*/




#ifndef __SHWEFFECTINFO_H__
#define __SHWEFFECTINFO_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "shwconstants.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * TShwEffectInfo, encapsulates the human readable name
 * and the uid of an effect.
 */ 
NONSHARABLE_CLASS( TShwEffectInfo )
    {
    public:
    	/** 
    	 * Constructor. Initializes the object
    	 */
    	inline TShwEffectInfo()
    		: iName( 0 ), 
    		iId( KNullUid )
    		{}
    	
    	/**
    	 * Assignment operator. Ensure that data members get
    	 * properly set
    	 */
		inline void operator=( const TShwEffectInfo& aRhs )
			{
			// need to set value as TPtrC does not have assignment 
			// operator
			iName = aRhs.iName;
			iId = aRhs.iId;
			}

    	/**
    	 * Comparison operator. 
		 * @param aRhs, the object to compare against
    	 */
		inline TBool operator==( const TShwEffectInfo& aRhs )
			{
			// just check the id
			return iId == aRhs.iId;
			}
		
    public:
     
     	/**
     	 * Helper struct to identify the effects
     	 */
		struct TEffectId
			{
			/**
			 * Default constructor
			 */
			inline TEffectId (TUid aEffectUid, TInt aEffectIndex = 0) 
				: iPluginUid (aEffectUid), iIndex (aEffectIndex)
				{
				}

			/**
			 * Comparison operator
			 * @param aRhs, the object to compare against
			 */
			inline TBool operator==( const TEffectId& aRhs )
				{
				return iPluginUid == aRhs.iPluginUid &&
					iIndex == aRhs.iIndex;
				}

			TUid  iPluginUid;
			TUint iIndex;
			};
			
		TEffectId iId;
		TBuf< NShwSlideshow::KMaxEffectNameLen > iName;

    };



#endif // __SHWEFFECTINFO_H__
