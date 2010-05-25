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
* Description:   glximagereader.cpp
*
*/


#include <apgcli.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <glxpanic.h>

//for DRM
#include <caf/content.h>
#include <caf/attributeset.h>
#include <DRMHelper.h>

#include "glximagereader.h"

const TInt KDefaultFrameCount = 1;

// ---------------------------------------------------------
// CGlxImageReader::NewL
// ---------------------------------------------------------
//  
CGlxImageReader* CGlxImageReader::NewL()
    {
    TRACER("CGlxImageReader::NewL");  
    
    CGlxImageReader* self = CGlxImageReader::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CGlxImageReader::NewLC
// ---------------------------------------------------------
//  
CGlxImageReader* CGlxImageReader::NewLC()
    {
    TRACER("CGlxImageReader::NewLC"); 
    
    CGlxImageReader* self = new(ELeave) CGlxImageReader();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CGlxImageReader::CGlxImageReader
// ---------------------------------------------------------
// 
CGlxImageReader::CGlxImageReader()
    {
    }

// ---------------------------------------------------------
// CGlxImageReader::~CGlxImageReader
// ---------------------------------------------------------
//
CGlxImageReader::~CGlxImageReader()
    {
    TRACER("CGlxImageReader::~");
    if(iImageDecoder)
        {
        delete iImageDecoder;
        }  

    if(iImgViewerMgr)
        {
        iImgViewerMgr->DeleteInstance();
        }
    }

// ---------------------------------------------------------
// CGlxImageReader::ConstructL
// ---------------------------------------------------------
//
void CGlxImageReader::ConstructL()
    {
    TRACER("CGlxImageReader::ConstructL");

    iImgViewerMgr = CGlxImageViewerManager::InstanceL();
    if (!iImgViewerMgr)
        {
        return;
        }

    TInt errInImage = KErrNone;
    if (iImgViewerMgr->IsPrivate())
        {
        if ( &(iImgViewerMgr->ImageFileHandle()) )
            {
            TRAP(errInImage,iImageDecoder = CImageDecoder::FileNewL(iImgViewerMgr->ImageFileHandle(), ContentAccess::EPeek));
            GLX_DEBUG2("CGlxImageReader::ConstructL() FH:errInImage=%d", errInImage);
            }
        }
    else
        {
        if ( iImgViewerMgr->ImageUri() )
            {
            TRAP(errInImage,iImageDecoder = CImageDecoder::FileNewL(CCoeEnv::Static()->FsSession(), iImgViewerMgr->ImageUri()->Des()));
            GLX_DEBUG2("CGlxImageReader::ConstructL() FN:errInImage=%d", errInImage);
            }
        }
    
    if (errInImage != KErrNone)
        {
        GLX_DEBUG2("CGlxImageReader::ConstructL() LEAVE(%d)", errInImage);
        User::Leave(errInImage);
        }
    }

// ---------------------------------------------------------
// CGlxImageReader::GetDRMRightsL
// ---------------------------------------------------------
//
TInt CGlxImageReader::GetDRMRightsL(TInt aAttribute)
    {
    TRACER("CGlxImageReader::GetDRMRightsL");
    
    TInt value = KErrNone;
    TInt error = KErrNone;
    CContent* content = NULL;
    if(iImgViewerMgr->IsPrivate())
        {
        if ( &(iImgViewerMgr->ImageFileHandle()) )
            {
            content = CContent::NewLC(iImgViewerMgr->ImageFileHandle());
            }
        }
    else
        {
        if ( iImgViewerMgr->ImageUri() )
            {
            content = CContent::NewLC(iImgViewerMgr->ImageUri()->Des());
            }
        }
    __ASSERT_ALWAYS(content, Panic(EGlxPanicNullPointer));
    error = content->GetAttribute(aAttribute, value);
    CleanupStack::PopAndDestroy( content );
    
    if(error != KErrNone)
	    {
	    switch	( aAttribute )
			{
			case ContentAccess::EIsProtected : 
				value = ETrue;
				break;
			case ContentAccess::ECanView : 
				value = EFalse;
				break;
		    default:
		    	value = EFalse;
			}
	    }
    GLX_DEBUG2("CGlxImageReader::GetDRMRightsL error =%d", error);
    GLX_DEBUG2("CGlxImageReader::GetDRMRightsL value=%d", value);
    return value;
    }

// ---------------------------------------------------------
// CGlxImageReader::GetFrameCount
// ---------------------------------------------------------
//
TInt CGlxImageReader::GetFrameCount()
    {
    TRACER("CGlxImageReader::GetFrameCount");
    TInt frameCount = KDefaultFrameCount;
    if (iImageDecoder)
        {
        frameCount = iImageDecoder->FrameCount();
        }
    GLX_DEBUG2("CGlxImageReader::GetFrameCount frameCount=%d", frameCount);
    return frameCount;
    }

// ---------------------------------------------------------
// CGlxImageReader::GetDimensions
// ---------------------------------------------------------
//
TSize CGlxImageReader::GetDimensions()
    {
    TRACER("CGlxImageReader::GetDimensions");
    TSize size = TSize();
    if (iImageDecoder)
        {
        size = iImageDecoder->FrameInfo().iOverallSizeInPixels;
        }
    GLX_DEBUG3("CGlxImageReader::GetImageSize() size w(%d) h(%d)",
            size.iWidth, size.iHeight);
    return size;
    }
