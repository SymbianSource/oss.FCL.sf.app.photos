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
* Description:   Definition of GlxtnImageDecoderFactory
*
*/



#ifndef GLXTNIMAGEDECODERFACTORY_H
#define GLXTNIMAGEDECODERFACTORY_H

#include <e32std.h>

class CImageDecoder;
class RFs;

/**
 * Factory to create appropriate decoder for a given image.
 *
 * @author Dan Rhodes
 * @ingroup glx_thumbnail_creator
 */
class GlxtnImageDecoderFactory
    {
public:
    /**
    * Create a decoder for an image file.  If the file is a JPEG, the function
    * will first try to create a hardware decoder.  If that fails a software
    * decoder will be created.
    * @param aFs File server session.
    * @param aFilename Full path of the file.
    * @return Pointer to image decoder (caller takes ownership).
    */
    static CImageDecoder* NewL( RFs& aFs, const TDesC& aFilename );
    /**
    * Create a decoder for JPEG image data.
    * @param aFs File server session.
    * @param aData Buffer containing the image to be decoded.
    * @return Pointer to image decoder (caller takes ownership).
    */
    static CImageDecoder* NewL( RFs& aFs, const TDesC8& aData );
    };

#endif  // GLXTNIMAGEDECODERFACTORY_H
