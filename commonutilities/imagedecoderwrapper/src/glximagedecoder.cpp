/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/

#include "glximagedecoder.h"
#include "glximagedecoderwrapper.h"

#include <bitmaptransforms.h>
#include <imageconversion.h>
#include <IclExtJpegApi.h>  // For CExtJpegDecoder


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxImageDecoder* CGlxImageDecoder::NewL(GlxImageDecoderWrapper* decoderWrapper)
{
	CGlxImageDecoder* self = new(ELeave)CGlxImageDecoder();
    CleanupStack::PushL(self);
    self->ConstructL(decoderWrapper);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CGlxImageDecoder::CGlxImageDecoder()
                         :CActive( EPriorityStandard )
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//

CGlxImageDecoder::~CGlxImageDecoder()
{
    iFs.Close();
    Cancel();
    if (iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
	if(iBitmap) {
		delete iBitmap;
		iBitmap = NULL;
	}
}

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CGlxImageDecoder::ConstructL(GlxImageDecoderWrapper* decoderWrapper)
{
    iDecoderWrapper = decoderWrapper;
    User::LeaveIfError(iFs.Connect());
    CActiveScheduler::Add( this );
}

// ---------------------------------------------------------------------------
// DoDecodeImageL
// ---------------------------------------------------------------------------	
void CGlxImageDecoder::DecodeImageL(QString aSourceFileName)
{
	//convert the argument to Symbian Format
	TPtrC16 sourceFileName(reinterpret_cast<const TUint16*>(aSourceFileName.utf16()));
	if (iImageDecoder)
        {
		iImageDecoder->Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
	/*TRAPD( err, iImageDecoder = CExtJpegDecoder::FileNewL(
            CExtJpegDecoder::EHwImplementation, iFs, sourceFileName, CImageDecoder::EOptionNone ) );
    if ( KErrNone != err )
        {
        TRAP(err,iImageDecoder = CExtJpegDecoder::FileNewL(
                CExtJpegDecoder::ESwImplementation, iFs, sourceFileName, CImageDecoder::EOptionNone ) );
        if ( KErrNone != err )
            {
            // Not a JPEG - use standard decoder
            iImageDecoder = CImageDecoder::FileNewL( iFs, sourceFileName, CImageDecoder::EOptionNone );
            }
        }*/
	iImageDecoder = CImageDecoder::FileNewL( iFs, sourceFileName, CImageDecoder::EOptionNone );
	TSize imageSize = iImageDecoder->FrameInfo().iOverallSizeInPixels;
	//clear the existing Bitmap
	if(iBitmap)
	{
		delete iBitmap;
		iBitmap = NULL;
	}
	// create the destination bitmap and pass it on to the decoder
    if(!iBitmap)
        {
        iBitmap = new (ELeave) CFbsBitmap();
        iBitmap->Create( imageSize,EColor64K);
        iImageDecoder->Convert( &iStatus, *iBitmap );
		SetActive();
		}
}
// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CGlxImageDecoder::RunL()
    {
    if( iStatus == KErrNone )
        {   
        iDecoderWrapper->decodedImageAvailable();
        delete iImageDecoder;
        iImageDecoder = NULL;
		iFs.Close();
        }	
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CGlxImageDecoder::DoCancel()
    {
    if(iImageDecoder)
        {
        iImageDecoder->Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
	iFs.Close();
    if(iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
	//todo add cleanup logic for pixmap
    }

// ---------------------------------------------------------------------------
// GetPixmap
// ---------------------------------------------------------------------------
//
QPixmap CGlxImageDecoder::GetPixmap()
	{
	if(iBitmap)
		{
		//convert the bitmap to pixmap
		iBitmap->LockHeap();
		TUint32 *tempData = iBitmap->DataAddress();
		uchar *data = (uchar *)(tempData);	
		int bytesPerLine = iBitmap->ScanLineLength(iBitmap->SizeInPixels().iWidth , iBitmap->DisplayMode());
		 QImage::Format format;
    switch(iBitmap->DisplayMode()) {
    case EGray2:
        format = QImage::Format_MonoLSB;
        break;
    case EColor256:
    case EGray256:
        format = QImage::Format_Indexed8;
        break;
    case EColor4K:
        format = QImage::Format_RGB444;
        break;
    case EColor64K:
        format = QImage::Format_RGB16;
        break;
    case EColor16M:
        format = QImage::Format_RGB666;
        break;
    case EColor16MU:
        format = QImage::Format_RGB32;
        break;
    case EColor16MA:
        format = QImage::Format_ARGB32;
        break;
#if !defined(__SERIES60_31__) && !defined(__S60_32__)
    case EColor16MAP:
        format = QImage::Format_ARGB32_Premultiplied;
        break;
#endif
    default:
        format = QImage::Format_Invalid;
        break;
    }
		//QImage share the memory occupied by data
		QImage image(data, iBitmap->SizeInPixels().iWidth, iBitmap->SizeInPixels().iHeight, bytesPerLine, format);
		iDecodedPixmap = QPixmap::fromImage(image);
		iBitmap->UnlockHeap();
		//clean the bitmap as it is not required anymore
		delete iBitmap;
		iBitmap = NULL;
		}
	return iDecodedPixmap;
	}

// ---------------------------------------------------------------------------
// ResetDecoder
// ---------------------------------------------------------------------------
//

void CGlxImageDecoder::ResetDecoder()
{
	Cancel();
	if(iBitmap) {
		delete iBitmap;
		iBitmap = NULL;
	}
	iDecodedPixmap = QPixmap();
}

