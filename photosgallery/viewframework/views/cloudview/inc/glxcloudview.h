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
* Description:    Cloud view implementation
 *
*/




#ifndef C_GLXCLOUDVIEW_H
#define C_GLXCLOUDVIEW_H

// INCLUDES
#include <glxmedialistviewbase.h>	// For CGlxMediaListViewBase

// CLASS DECLARATION

/**
 *  Cloud view which displays weihted list of items associated with gallery items
 *
 *  @lib glxcloudview.lib
 */
NONSHARABLE_CLASS(CGlxCloudView): public CGlxMediaListViewBase
	{
public:
	/** 	
	 *  @function NewL
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aMediaListFactory - Handle to the medialist provider
	 *  @param aViewResId - Resource id of the View
	 *  @param aTitleResId - Resource id for the Title text
	 *  @param aEmptyListResId - Resource id for the empty listbox text
	 *  @param aSoftKeyResId - Resource id for the softkey text
	 *  @paramaSoftkeyMskDisabledId - Resource id for msk disabled.
	 *  @return Pointer to newly created object
	 */
	IMPORT_C static CGlxCloudView *NewL(MGlxMediaListFactory
			*aMediaListFactory, const TDesC &aFileName, TInt aViewResId, 
			TInt aEmptyListResId, TInt aSoftKeyResId, TInt aSoftkeyMskDisabledId);

	/** 	
	 *  @function NewLC
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aMediaListFactory - Handle to the medialist provider
	 *  @param aViewResId - Resource id of the View
	 *  @param aTitleResId - Resource id for the Title text
	 *  @param aEmptyListResId - Resource id for the empty listbox text
	 *  @param aSoftKeyResId - Resource id for the softkey text
	 *  @paramaSoftkeyMskDisabledId - Resource id for msk disabled.
	 *  @return Pointer to newly created object
	 */
	IMPORT_C static CGlxCloudView *NewLC(MGlxMediaListFactory
			*aMediaListFactory, const TDesC &aFileName, TInt aViewResId, 
			TInt aEmptyListResId, TInt aSoftKeyResId, TInt aSoftkeyMskDisabledId);

	/**
	 * Destructor
	 */
	virtual ~CGlxCloudView();
	};

#endif // C_GLXCLOUDVIEW_H

// End of File
