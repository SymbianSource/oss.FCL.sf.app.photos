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
* Description:    Global defs
*
*/




/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */


#ifndef _GLXTNSTD_H
#define _GLXTNSTD_H

#include "glxid.h"
#include <e32base.h>
class TGlxTasktnIdTypeBase {}; // Don't use this
typedef TGlxId<TGlxTasktnIdTypeBase> TGlxtnTaskId;	// Use this

// Thumbnail task Ids
const TUint KGlxtnTaskIdGenerateThumbnail = 1;
const TUint KGlxtnTaskIdSaveThumbnail = 2;
const TUint KGlxtnTaskIdBackgroundThumbnailGeneration = 3;
const TUint KGlxtnTaskIdQuickThumbnail = 4;
const TUint KGlxtnTaskIdDeleteThumbnails = 5;
const TUint KGlxtnTaskIdFilterAvailable = 6;
const TUint KGlxtnTaskIdCleanupThumbnails = 7;
const TUint KGlxtnTaskIdZoomedThumbnail = 8;

// Task priorities
const TInt KGlxTaskPriorityCreateList = CActive::EPriorityStandard + 1;
const TInt KGlxTaskPriorityFetchProperties = KGlxTaskPriorityCreateList - 1;
const TInt KGlxTaskPriorityForegroundThumbnailProvisioning = KGlxTaskPriorityFetchProperties - 1;
const TInt KGlxTaskPriorityFileSystemScan = KGlxTaskPriorityForegroundThumbnailProvisioning - 1;
const TInt KGlxTaskPriorityBackgroundThumbnailProvisioning = EPriorityLow;
const TInt KGlxTaskPriorityBackgroundThumbnailGenerator = EPriorityLow - 1;

// Thumbnail availablity
const TInt KGlxThumbnailAvailable = 0;
const TInt KGlxThumbnailNotAvailable = 1;



/**
 * Thumbnail image formats
 */
enum TGlxImageDataFormat 
	{
	EGlxIDF_Bitmap, // Externalised CFbsBitmap
	EGlxIDF_JPEG    // Encoded with JPEG format
	};

#endif // _GLXTNSTD_H