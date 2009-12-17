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
                                }

// ---------------------------------------------------------
// CGlxImageReader::~CGlxImageReader
// ---------------------------------------------------------
//
CGlxImageReader::~CGlxImageReader()
    {
    TRACER("CGlxImageReader::~");
    
    Cancel();
    delete iFrame;
    delete iImageDecoder;
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
    
    CActiveScheduler::Add(this);
    iIsLaunchedFromFMngr = EFalse;
    TDataType imageType;

    iImgViewerMgr = CGlxImageViewerManager::InstanceL();
    if (iImgViewerMgr && iImgViewerMgr->IsPrivate())
        {
        iIsLaunchedFromFMngr = ETrue;    
        }
    
    GetFileTypeL(imageType);
    if (iIsLaunchedFromFMngr)
        {
        // TODO type cast handle to RFile
        iImageDecoder = CImageDecoder::FileNewL(iImgViewerMgr->ImageFileHandle(), ContentAccess::EPeek);
        }
    else
        { 
        iImageDecoder = CImageDecoder::FileNewL(CCoeEnv::Static()->FsSession(), iImgViewerMgr->ImageUri()->Des());
        }

    iFrame = new(ELeave)CFbsBitmap();
    iFrame->Create(iImageDecoder->FrameInfo(0).iOverallSizeInPixels,iImageDecoder->FrameInfo(0).iFrameDisplayMode);
    iImageDecoder->Convert(&iStatus,*iFrame,0);
    SetActive();
    }

// ---------------------------------------------------------
// CGlxImageReader::GetFileTypeL
// ---------------------------------------------------------
//
void CGlxImageReader::GetFileTypeL(TDataType aMimeType)
    {
    TRACER("CGlxImageReader::GetFileTypeL");
    
    RApaLsSession session;
    User::LeaveIfError(session.Connect());
    CleanupClosePushL(session);

    TUid uid;
    if(iIsLaunchedFromFMngr)
        {
        // TODO type cast handle to RFile
        User::LeaveIfError(session.AppForDocument(iImgViewerMgr->ImageFileHandle(), uid, aMimeType));
        }
    else
        {
        User::LeaveIfError(session.AppForDocument(iImgViewerMgr->ImageUri()->Des(),uid,aMimeType));
        }
    CleanupStack::PopAndDestroy(); // session
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
    
    const TSize sz = iFrame->SizeInPixels();
    iNotify.ImageReadyL(iStatus.Int(),sz);
    }
// ---------------------------------------------------------
// CGlxImageReader::HasDRMRightsL
// ---------------------------------------------------------
//
TBool CGlxImageReader::HasDRMRightsL()
    {
    TRACER("Entering CGlxImageReader::HasDRMRightsL");
 
    RAttributeSet attributeSet;
    CleanupClosePushL(attributeSet);
    attributeSet.AddL(ECanView);
    attributeSet.AddL(EIsProtected);
    CContent* content;
    if(iIsLaunchedFromFMngr)
        {
        content = CContent::NewL(iImgViewerMgr->ImageFileHandle());    
        }
    else
        {
        content = CContent::NewL(iImgViewerMgr->ImageUri()->Des());
        }

    // Retrieve the attributes from the agent
    User::LeaveIfError(content->GetAttributeSet(attributeSet));
    TInt rights = ERightsNone;
    TBool drmProtected = EFalse;
    attributeSet.GetValue(EIsProtected,drmProtected);
    if(!drmProtected)
        {
        CleanupStack::PopAndDestroy();
        delete content;
        content = NULL;
        return EFalse;
        }
    attributeSet.GetValue(ECanView,rights);
    CleanupStack::PopAndDestroy();

    if(content)
        {
        delete content;
        content = NULL;
        }

    if(rights)
        {
        return ETrue;
        }
    return EFalse;
    }
