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


class MImageReadyCallBack
    {
public:
    virtual void ImageReadyL(const TInt& aError, const TSize aSz) = 0;
    };


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
    TBool HasDRMRightsL();

private:
    MImageReadyCallBack&                iNotify;
    CImageDecoder*                      iImageDecoder;
    CFbsBitmap*                         iFrame;
    TBool                               iIsLaunchedFromFMngr;
    CGlxImageViewerManager*             iImgViewerMgr;
    };

#endif /* GLXIMAGEREADER_H_ */
