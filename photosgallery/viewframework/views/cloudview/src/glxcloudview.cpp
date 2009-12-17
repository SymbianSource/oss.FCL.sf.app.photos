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
* Description:    Implementation of Cloud view
 *
*/




#include "glxcloudview.h" //class declaration

// INCLUDE FILES
#include "glxcloudviewimp.h" // class header
#include <glxtracer.h>					// For Logs


// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxCloudView::~CGlxCloudView()
	{
	TRACER("GLX_CLOUD::CGlxCloudView::~CGlxCloudView");
	// No implementation
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
EXPORT_C CGlxCloudView *CGlxCloudView::NewL
(MGlxMediaListFactory*aMediaListFactory, const TDesC &aFileName, TInt
		aViewResId, TInt aEmptyListResId, TInt aSoftKeyResId, TInt aSoftkeyMskDisabledId)
	{
	TRACER("GLX_CLOUD::CGlxCloudView::NewL");
	return CGlxCloudViewImp::NewL(aMediaListFactory, aFileName, aViewResId,
	 aEmptyListResId, aSoftKeyResId, aSoftkeyMskDisabledId);
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCloudView *CGlxCloudView::NewLC
(MGlxMediaListFactory*aMediaListFactory, const TDesC &aFileName, TInt
		aViewResId, TInt aEmptyListResId, TInt aSoftKeyResId, TInt aSoftkeyMskDisabledId)
	{
	TRACER("GLX_CLOUD::CGlxCloudView::NewLC");
	return CGlxCloudViewImp::NewLC(aMediaListFactory, aFileName, aViewResId, 
	 aEmptyListResId, aSoftKeyResId, aSoftkeyMskDisabledId);
	}

//  End of File
