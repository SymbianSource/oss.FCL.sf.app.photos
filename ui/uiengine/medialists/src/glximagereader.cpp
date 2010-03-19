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


// ---------------------------------------------------------
// CGlxImageReader::NewL
// ---------------------------------------------------------
//  
CGlxImageReader* CGlxImageReader::NewL(MImageReadyCallBack& aNotify)
    {
    TRACER("CGlxImageReader::NewL");  
    
    CGlxImageReader* self = CGlxImageReader::NewLC( aNotify);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CGlxImageReader::NewLC
// ---------------------------------------------------------
//  
CGlxImageReader* CGlxImageReader::NewLC(MImageReadyCallBack& aNotify)
    {
    TRACER("CGlxImageReader::NewLC"); 
    
    CGlxImageReader* self = new(ELeave) CGlxImageReader(aNotify);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CGlxImageReader::CGlxImageReader
// ---------------------------------------------------------
// 
CGlxImageReader::CGlxImageReader(MImageReadyCallBack& aNotify)
:CActive(0),iNotify(aNotify)
    {
    //no implementation
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
        Cancel();
        delete iImageDecoder;
        }  

    if(iFrame)
        {
        delete iFrame;
        }

    if(iImgViewerMgr)
        {
        iImgViewerMgr->Close();
        }
    }

// ---------------------------------------------------------
// CGlxImageReader::ConstructL
// ---------------------------------------------------------
//
void CGlxImageReader::ConstructL()
    {
    TRACER("CGlxImageReader::ConstructL");

    CActiveScheduler::Add(this);

    iImgViewerMgr = CGlxImageViewerManager::InstanceL();
    if (!iImgViewerMgr)
        {
        return;
        }

    TInt errInImage = KErrGeneral;
    if (iImgViewerMgr->IsPrivate())
        {
        if ( &(iImgViewerMgr->ImageFileHandle()) != NULL )
            {
            TRAP(errInImage,iImageDecoder = CImageDecoder::FileNewL(iImgViewerMgr->ImageFileHandle(), ContentAccess::EPeek));
            }
        }
    else
        {
        if ( iImgViewerMgr->ImageUri() != NULL )
            {
            TRAP(errInImage,iImageDecoder = CImageDecoder::FileNewL(CCoeEnv::Static()->FsSession(), iImgViewerMgr->ImageUri()->Des()));
            }
        }
    if (errInImage != KErrNone)
        {
        User::Leave(errInImage);
        }

    if ( iImageDecoder )
        {
        iFrame = new (ELeave) CFbsBitmap();
        iFrame->Create(iImageDecoder->FrameInfo(0).iOverallSizeInPixels,
                iImageDecoder->FrameInfo(0).iFrameDisplayMode);
        iImageDecoder->Convert(&iStatus, *iFrame, 0);
        SetActive();
        }
    }


// ---------------------------------------------------------
// CGlxImageReader::DoCancel
// ---------------------------------------------------------
//
void CGlxImageReader::DoCancel()
    {
    TRACER("CGlxImageReader::DoCancel");
    iImageDecoder->Cancel();
    }

// ---------------------------------------------------------
// CGlxImageReader::RunL
// ---------------------------------------------------------
//
void CGlxImageReader::RunL()
    {
    TRACER("CGlxImageReader::RunL");
    
    TSize size = TSize();
    TInt reqStatus = iStatus.Int(); 
    if (reqStatus == KErrNone && iFrame)
        {
        size = iFrame->SizeInPixels();
        }
    GLX_DEBUG2("CGlxImageReader::RunL() reqStatus=%d", reqStatus);   
    iNotify.ImageSizeReady(reqStatus, size);
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
        if ( &(iImgViewerMgr->ImageFileHandle()) != NULL )
            {
            content = CContent::NewLC(iImgViewerMgr->ImageFileHandle());
            }
        }
    else
        {
        if ( iImgViewerMgr->ImageUri() != NULL )
            {
            content = CContent::NewLC(iImgViewerMgr->ImageUri()->Des());
            }
        }
    __ASSERT_ALWAYS(content, Panic(EGlxPanicNullPointer));
    error = content->GetAttribute(aAttribute, value);
    CleanupStack::PopAndDestroy( content );
    GLX_DEBUG2("CGlxImageReader::GetDRMRightsL value=%d", value);
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
    return value;
    }
