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
* Description: Gif Decoder Active Object
*
*/


#ifndef GLXDRMGIFACTIVEDECODER_H_
#define GLXDRMGIFACTIVEDECODER_H_

// INCLUDES
#include <imageconversion.h>
#include "glxdrmgiftexturecreator.h"

// CLASS DECLARATION
class CImageDecoder;

class CGlxDRMgifDecoderAO : public CActive
    {
public:
    /*
     * NewL()
     * @Param1 CGlxDrmGifTextureCreator
     */
    static CGlxDRMgifDecoderAO* NewL(CGlxDrmGifTextureCreator* aDrmGifTextureCreator);
    
    /*
     * Destructor
     */
    ~CGlxDRMgifDecoderAO();
    
    /*
     * ConvertImageL() 
     * This calls the asyncronous service request to ICL convert
     * @param1 - Destination Bitmap
     * @param2 - Destination Bitmap Mask
     * @param3 - Frame number to be decoded
     * @param4 - Image decoder 
     */
    void ConvertImageL(CFbsBitmap* aBitmap, CFbsBitmap* aBitmapMask, TInt aFrameNum,
            CImageDecoder* aDecoder);
protected:
    // from CActive
    void RunL();
    void DoCancel();
private:
    /*
     * Constructor
     */
    CGlxDRMgifDecoderAO(CGlxDrmGifTextureCreator* aDrmGifTextureCreator);

private:
    CGlxDrmGifTextureCreator* iDrmGifTextureCreator; // not owned
    CImageDecoder* iDecoder; // not owned
    };

#endif /* GLXDRMGIFACTIVEDECODER_H_ */
