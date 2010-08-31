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
* Description:   Implementation of GlxtnImageDecoderFactory
*
*/



#include "glxtnimagedecoderfactory.h"

#include <glxtracer.h>
#include <IclExtJpegApi.h>  // For CExtJpegDecoder

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CImageDecoder* GlxtnImageDecoderFactory::NewL( RFs& aFs,
                                                const TDesC& aFilename )
    {
    TRACER("CImageDecoder* GlxtnImageDecoderFactory::NewL()");
    CImageDecoder* decoder = NULL;

    // Use extended JPEG decoder
    TRAPD( err, decoder = CExtJpegDecoder::FileNewL(
                CExtJpegDecoder::EHwImplementation, aFs, aFilename, CImageDecoder::EOptionNone ) );
    if ( KErrNone != err )
        {
        TRAP(err,decoder = CExtJpegDecoder::FileNewL(
                CExtJpegDecoder::ESwImplementation, aFs, aFilename, CImageDecoder::EOptionNone ) );
        if ( KErrNone != err )
            {
            // Not a JPEG - use standard decoder
            decoder = CImageDecoder::FileNewL( aFs, aFilename, CImageDecoder::EOptionNone );
            }
        }

    return decoder;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CImageDecoder* GlxtnImageDecoderFactory::NewL( RFs& aFs, const TDesC8& aData )
    {
    TRACER("CImageDecoder* GlxtnImageDecoderFactory::NewL()");
    CImageDecoder* decoder = NULL;

    TRAPD( err, decoder = CExtJpegDecoder::DataNewL(
                        CExtJpegDecoder::EHwImplementation, aFs, aData ) );
    if ( KErrNone != err )
        {
        TRAP(err, decoder = CExtJpegDecoder::DataNewL(
                        CExtJpegDecoder::ESwImplementation, aFs, aData ) );
        if ( KErrNone != err )
            {
            // Not a JPEG - use standard decoder
            decoder = CImageDecoder::DataNewL( aFs, aData, CImageDecoder::EOptionNone );
            }
        }

    return decoder;
    }
