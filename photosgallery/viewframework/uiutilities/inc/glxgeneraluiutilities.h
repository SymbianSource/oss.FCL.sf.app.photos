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
* Description:    general utitities
*
*/




#ifndef __GLXGENERALUIUTILITIES_H__
#define __GLXGENERALUIUTILITIES_H__

// INCLUDE FILES
#include <textresolver.h>
#include <aknnotewrappers.h>

/**
 *  General utilties class definition
 *
 *  @lib glxuiutilities.lib
 */
NONSHARABLE_CLASS(GlxGeneralUiUtilities)
	{
public:
	/**
    * Used to display error by displaying a dialog
    * @param aError the error to display
    */
	IMPORT_C static void ShowErrorNoteL(TInt aError);
	
	/**
    * Confirmation dialog wrapper
    * @param aQueryResourceId the resource id of the dialog
    * @param aTitleText the text to display on the dialog
    * @return error
    */
	IMPORT_C static TBool ConfirmQueryL( TInt aQueryResourceId, const TDesC& aTitleText );
	
	/**
    * Confirmation dialog wrapper
    * @param aTitleText the text to display on the dialog
    * @return error
    */
	IMPORT_C static TBool ConfirmQueryL( const TDesC& aTitleText );
	
	/**
    * Information note wrapper
    * @param aInfoText the string to display
    * @param aWaitingDialog whether the dialog is waiting or not
    */
	IMPORT_C static void ShowInfoNoteL( const TDesC& aInfoText, TBool aWaitingDialog );
	
		/**
    * Information note wrapper
    * @param aInfoText the string to display
    * @param aWaitingDialog whether the dialog is waiting or not
    */
	IMPORT_C static void ShowErrorNoteL( const TDesC& aInfoText, TBool aWaitingDialog );
	
	/**
    * Confirmation note wrapper
    * @param aInfoText the string to display
    * @param aWaitingDialog whether the dialog is waiting or not
    */
	IMPORT_C static void ShowConfirmationNoteL( const TDesC& aInfoText, TBool aWaitingDialog );
	
	/**
    * Formats the given source buffer and stores the formatted
    * presentation into destination buffer
    * @param aDestination Destination buffer for the formatted string
    * @param aSource Unformatted source buffer
    * @param aPosition The index of the key string.
    * @param aNumber the replacing TInt.
    * @param aNumberConversion, ETrue if language specific number conversion
    *        should be applied to aDestination after formatting
    */
	IMPORT_C static void FormatString( 	TDes& aDestination,
                             	const TDesC& aSource,
                               	TInt aPosition,
                               	TInt aNumber,
                              	TBool aNumberConversion );
                              	
   /**
    * Checks whether the screen is in landscape or portrait
    * @return indicates whether screen is in landscape
    */ 
                              	
	 IMPORT_C static TBool IsLandscape();
	 
	 
	/**
    * Wrapper of AknLayoutUtils::LayoutMirrored()
    * allows result to be forced for tesing purposes
    * @return indicates if layout should be mirrored
    */
    IMPORT_C static TBool LayoutIsMirrored();                              	
	};

#endif // __GLXGENERALUIUTILITIES_H__