/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Stub file for thumbnail and attribute context
 *
*/



#ifndef __STUB_GLXFETCHCONTEXTS_H__
#define __STUB_GLXFETCHCONTEXTS_H__

#include <glxmedia.h>
#include <glxthumbnailcontext.h>
#include <glxattributecontext.h>
#include <glxattributeretriever.h>

// this flag defines the return value for CGlxThumbnailContext::RequestCountL
extern TInt gThumbnailContextRequestCount;
// this flag defines the return value for CGlxAttributeContext::RequestCountL
extern TInt gAttributeContextRequestCount;
// this flag defines the return value for GlxAttributeRetriever::RetrieveL
extern TInt gGlxAttributeRetrieverRetrieveL;
// this flag defines the return value for TGlxMedia::GetDimensions
extern TBool gTGlxMediaGetDimensions;
// this flag defines if the test is meant to be an alloc test
extern TBool gFetchContextAllocTest;


#endif      //  __STUB_GLXFETCHCONTEXTS_H__

// End of file
