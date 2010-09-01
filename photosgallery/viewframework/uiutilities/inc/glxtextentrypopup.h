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
* Description:    text entry popup class
*
*/




#ifndef __GLXTEXTENTRYPOPUP_H__
#define __GLXTEXTENTRYPOPUP_H__

#include <AknQueryDialog.h>

/**
 *  text entry popup class definition
 *
 *  @lib glxuiutilities.lib
 *
 * @internal reviewed 06/06/2007 by Dave Schofield
 */
class CGlxTextEntryPopup : protected CAknTextQueryDialog
	{
public:
	
	/**
	 * Creates a new CGlxTextEntryPopup.
	 * @param aTitle Dialog title.
	 * @param aText default dialog text
	 *      on completion of ExecuteLD() contains text entered by user.
	 * @return a pointer to a new instance of CGlxTextEntryPopup.
	 */
	IMPORT_C static CGlxTextEntryPopup* NewL(const TDesC& aTitle, TDes& aText);	

	/**
    * calls the popup for text entry
    * @return Zero, unless it is a waiting dialog. 
    * For a waiting dialog, the return value is the ID 
    * of the button that closed the dialog, or zero if 
    * it was the cancel button 
    */
	IMPORT_C TInt ExecuteLD();

    /**
    * Updates left softkey 
    */	
	virtual void UpdateLeftSoftKeyL();
	
	/**
    * Sets left softkey based on conditions whether to make it visible or not
    * @param aAllowEmptyString - bool to set the leftsoft key status
    */
	IMPORT_C void SetLeftSoftKeyL(TBool aAllowEmptyString);

	
protected:
	/**
	 * Constructor
	 * @param aText default dialog text
	 *      on completion of ExecuteLD() contains text entered by user.
	 */
	CGlxTextEntryPopup(TDes& aText);	
	/**
	 * Second stage constructor
	 * @param aTitle Dialog title.
	 */
	void ConstructL(const TDesC& aTitle);
	
	/**
	 * Helper function to revert state in case anything leaves
	 * between state changes
	 */
	static void RollbackState( TAny* aParam );
private:
    //Bool which sets the leftsoftkey status
    TBool iAllowEmptyString;	
	};

#endif // __GLXTEXTENTRYPOPUP_H__
