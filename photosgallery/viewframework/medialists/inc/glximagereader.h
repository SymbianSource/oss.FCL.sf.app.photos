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
* Description:   imagereader header file
*
*/


#ifndef GLXIMAGEREADER_H_
#define GLXIMAGEREADER_H_

#include <imageconversion.h>
#include "glximageviewermanager.h"
class CFbsBitmap;

/**
 *  Image Size observer.
 *
 *  Callback interface for getting size information of a given image.
 */
class MImageReadyCallBack
    {
public:
    /**
     * Getting image size information is complete.
     *
     * @param aError         Error code.
     * @param aSz            Size of the image.
     */    
    virtual void ImageSizeReady(TInt aError, const TSize aSz) = 0;
    };

/**
 *  CGlxImageReader
 *
 *  Image Attributes Reader
 *
 *  @lib glxmedialists.lib
 */
class CGlxImageReader : public CActive
    {
private:
    static CGlxImageReader* NewLC(MImageReadyCallBack& aNotify);
    CGlxImageReader(MImageReadyCallBack& aNotify);
    void ConstructL();
    void GetFileTypeL(TDataType aMimeType);

protected:
    void DoCancel();
    void RunL();

public:
    static CGlxImageReader* NewL(MImageReadyCallBack& aNotify);
    ~CGlxImageReader();
    TInt GetDRMRightsL(TInt aAttribute);

private:
    MImageReadyCallBack&                iNotify;
    CImageDecoder*                      iImageDecoder;
    CFbsBitmap*                         iFrame;
    CGlxImageViewerManager*             iImgViewerMgr;
    };

#endif /* GLXIMAGEREADER_H_ */
