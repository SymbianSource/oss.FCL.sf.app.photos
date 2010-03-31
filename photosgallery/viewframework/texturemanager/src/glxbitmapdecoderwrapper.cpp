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
* Description:    Texture Manager component/Decodes the Image in step for large sized images
 *
*/




// INCLUDE FILES
#include "glxbitmapdecoderwrapper.h"
#include <glxtracer.h>      //  For Log
#include <bitmaptransforms.h>
#include <imageconversion.h>
#include <ExifRead.h>
#include <glxlog.h>
#include <hal.h>
#include <hal_data.h>
#include <mglxcache.h>
#include <oommonitorsession.h>
namespace
{
const TInt KGlxDecodingThreshold = 3000000;


// Photos low_ram_threshold [8.637 MB] as in oomconfig.xml
const TInt KGlxCriticalRAMForPhotos = 9056550;
const TInt KGlxDecodeBitmapFactor = 3;
// All EXIF data is within the first KGlxMaxExifSize bytes of the file
const TInt KGlxMaxExifSize = 0x10000;
}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxBitmapDecoderWrapper* CGlxBitmapDecoderWrapper::NewL(MGlxBitmapDecoderObserver* aObserver)
    {
    TRACER("CGlxBitmapDecoderWrapper:: NewL ");
    CGlxBitmapDecoderWrapper* self = new(ELeave)CGlxBitmapDecoderWrapper();
    CleanupStack::PushL(self);
    self->ConstructL(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CGlxBitmapDecoderWrapper::CGlxBitmapDecoderWrapper()
                         :CActive( EPriorityLow )
		        {
		        TRACER("CGlxBitmapDecoderWrapper:: NewL ");
		        }		

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//

CGlxBitmapDecoderWrapper::~CGlxBitmapDecoderWrapper()
    {
    TRACER("CGlxBitmapDecoderWrapper:: ~CGlxBitmapDecoderWrapper ");
    iFs.Close();
    Cancel();
    if (iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CGlxBitmapDecoderWrapper::ConstructL(MGlxBitmapDecoderObserver* aObserver)
    {
    TRACER("CGlxBitmapDecoderWrapper::ConstructL ");
    iObserver = aObserver;
    User::LeaveIfError(iFs.Connect());
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// DoDecodeImageL
// ---------------------------------------------------------------------------		

void CGlxBitmapDecoderWrapper::DoDecodeImageL(const TDesC & aSourceFileName,TInt aIndex)
    {
    TRACER("CGlxBitmapDecoderWrapper:: DoDecodeImageL ");
    iThumbnailIndex = aIndex;
    //Variable used to get the decoder type used
    TBool isExtDecoderUsed = ETrue;
    
#ifdef _DEBUG
    iDecodeProcessstartTime.HomeTime();
#endif                          

    if (iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    // Use extended JPEG decoder
    TRAPD( err, iImageDecoder = CExtJpegDecoder::FileNewL(
            CExtJpegDecoder::EHwImplementation, iFs, aSourceFileName, CImageDecoder::EOptionNone ) );
    if ( KErrNone != err )
        {
        GLX_LOG_INFO( "DoDecodeImageL:: ESwImplementation" );
        TRAP(err,iImageDecoder = CExtJpegDecoder::FileNewL(
                CExtJpegDecoder::ESwImplementation, iFs, aSourceFileName, CImageDecoder::EOptionNone ) );
        if ( KErrNone != err )
            {
            GLX_LOG_INFO( "DoDecodeImageL:: CImageDecoder" );
            // Not a JPEG - use standard decoder
            iImageDecoder = CImageDecoder::FileNewL( iFs, aSourceFileName, CImageDecoder::EOptionNone );
            isExtDecoderUsed = EFalse;
            }
        }
#ifdef _DEBUG
        iStopTime.HomeTime();
        GLX_DEBUG1("=== DECODER CREATION ===");
        GLX_DEBUG2("Decoder Creation took <%d> us", 
                   (TInt)iStopTime.MicroSecondsFrom(iDecodeProcessstartTime).Int64());   
#endif                          
		
    TSize imageSize = iImageDecoder->FrameInfo().iOverallSizeInPixels;
    if(isExtDecoderUsed)
        {
        TUint16 orientation=0;
        //Read the orientation from the Exif header of the image if present
        TRAPD(error, orientation=GetOrientationL(aSourceFileName));
        if(KErrNone == error )
            {
            //Get the rotation angle and the flip status from orientation
            TInt rotAngle = 0;
            TBool flipStatus = EFalse;
            GetRotationParameters(orientation, rotAngle, flipStatus);
            //Set the parameters to the decoder
            CExtJpegDecoder* extDecoder = (CExtJpegDecoder*)iImageDecoder;
            extDecoder->SetRotationL(rotAngle);
            if(flipStatus)
                {
                extDecoder->SetMirroringL();
                }
            //Switch Image Height and width in case orientation > 4 as that 
            //corresponds to angles 180 and 270 degrees
            if (orientation > 4)
                {
                imageSize.SetSize(imageSize.iHeight,imageSize.iWidth);
                }
            }

        }
    iOriginalSize.iWidth = imageSize.iWidth;
    iOriginalSize.iHeight = imageSize.iHeight;

    if (iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    DecodeImageL();
    }
// ---------------------------------------------------------------------------
// DecodeImageL
// ---------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::DecodeImageL()
    {
    TRACER("CGlxBitmapDecoderWrapper:: DecodeImageL ");
    TReal32 mFactor = 1;
    //Set Size according to level and state
    TReal32 width = iOriginalSize.iWidth;
    TReal32 height = iOriginalSize.iHeight;
    GLX_LOG_INFO1("DecodeImageL:width=%f", width);
    GLX_LOG_INFO1("DecodeImageL:height=%f",height);

    
    if ( KGlxDecodingThreshold < (width * height))
        {
        mFactor = TReal32(KGlxDecodingThreshold) / (width*height);
        }

    // create the destination bitmap
    if(!iBitmap)
        {
        TInt freeMemory = 0;
        HAL::Get( HALData::EMemoryRAMFree, freeMemory );
        width*=mFactor;
        height*=mFactor;
        TInt minmemorytodecode = KGlxDecodeBitmapFactor*width*height;     
        GLX_LOG_INFO2("DecodeImageL: after factoring width=%f, height=%f", width, height);
        GLX_LOG_INFO2("DecodeImageL:minmemorytodecode=%d, freememory=%d",
                minmemorytodecode, freeMemory);
        if(minmemorytodecode < (freeMemory - KGlxCriticalRAMForPhotos))
            {
            GLX_LOG_INFO("DecodeImageL:RAM available decoding image");

            iBitmap = new (ELeave) CFbsBitmap();
            iBitmap->Create( TSize(width,height),iImageDecoder->FrameInfo().iFrameDisplayMode );
#ifdef _DEBUG
            iStartTime.HomeTime(); // Get home time
#endif                          
            iImageDecoder->Convert( &iStatus, *iBitmap );
            }
        else
            {
            //case when sufficient memory is not available
            //request OOM FW to release the required memory
            GLX_LOG_INFO("DecodeImageL:insufficient RAM - request OOM");
            TInt err = OOMRequestFreeMemoryL(minmemorytodecode);
            if(err == KErrNoMemory)
                {
                //if OOM fails, release Photos Cache
                GLX_LOG_INFO("DecodeImageL:insufficient RAM - OOM failed - request Cache");
                MGlxCache* cacheManager = MGlxCache::InstanceL();
                cacheManager->ReleaseRAML(ETrue);            
                cacheManager->Close();
                //Try and release memory again
                err = OOMRequestFreeMemoryL(minmemorytodecode);
                }
            if(err != KErrNoMemory)
                {
                GLX_LOG_INFO("DecodeImageL:Sufficient RAM available");
                iBitmap = new (ELeave) CFbsBitmap();
                iBitmap->Create( TSize(width,height),iImageDecoder->FrameInfo().iFrameDisplayMode );
#ifdef _DEBUG
	            iStartTime.HomeTime(); // Get home time
#endif                          
                iImageDecoder->Convert( &iStatus, *iBitmap );
                }
            else
                {
                GLX_LOG_INFO("NOT ENOUGH MEMORY - Using the Fullscreen Thumbnail For Zoom");
                //release the file held by decoder immediately.
                iImageDecoder->Cancel();
                delete iImageDecoder;
                iImageDecoder = NULL;
                //Inform the client that there is no decode happened and there we take care 
                //of showing the fullscreen thumbnail.
                iObserver->HandleBitmapDecodedL(iThumbnailIndex,NULL);
                return;                
                }
            }

        SetActive();
        }
    }
// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::RunL()
    {
    TRACER("CGlxBitmapDecoderWrapper:: RunL ");
    if( iStatus == KErrNone )
        {
        iObserver->HandleBitmapDecodedL(iThumbnailIndex,iBitmap);
        iBitmap = NULL;

        //release the file held by decoder immediately.
        GLX_LOG_INFO( " CGlxBitmapDecoderWrapper::RunL:Decoding Finished" );
        iImageDecoder->Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
#ifdef _DEBUG
        iStopTime.HomeTime();
        GLX_DEBUG1("=== IMAGE DECODE ===");
        GLX_DEBUG2("=>Image Decode Took took <%d> us", 
                       (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());            
#endif                          

        }
    }
// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::DoCancel()
    {
    TRACER("CGlxBitmapDecoderWrapper:: DoCancel ");
    if(iImageDecoder)
        {
        GLX_LOG_INFO( " CGlxBitmapDecoderWrapper::DoCancel:Deleting" );
        iImageDecoder->Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    if(iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    }

// ---------------------------------------------------------------------------
// GetOrientationL
// ---------------------------------------------------------------------------
//
TUint16 CGlxBitmapDecoderWrapper::GetOrientationL(const TDesC& aFileName)
    {
    TRACER("CGlxBitmapDecoderWrapper:: GetOrientationL ");
    //Get Exif Metadata and the orientation tag from the file first
            RFile file;
            CleanupClosePushL(file);
            User::LeaveIfError(file.Open(iFs,
                    aFileName, EFileRead));
            TInt size;
            User::LeaveIfError(file.Size(size));
            if ( KGlxMaxExifSize < size )
                {
                size = KGlxMaxExifSize;
                }
            TUint16 orientation = 9;
            HBufC8* exifData = HBufC8::NewLC(size);
            TPtr8 ptr(exifData->Des());
            User::LeaveIfError(file.Read(ptr));
            CExifRead* exifReader = NULL;
            TRAPD(exifErr,exifReader = CExifRead::NewL(*exifData, CExifRead::ENoJpeg));
            if(exifErr == KErrNone)
                {
                CleanupStack::PushL(exifReader);

                TInt readErr = exifReader->GetOrientation(orientation);
                if(readErr != KErrNone)
                    {
                    orientation = 9;
                    }
                CleanupStack::PopAndDestroy(exifReader);
                }
            CleanupStack::PopAndDestroy(exifData);
            //Close and pop file Session
            CleanupStack::PopAndDestroy(&file);
            return orientation;
    
    }
// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::GetRotationParameters(TUint16 aOrientation, TInt& aRotAngle, TBool& aFlipStatus) 
    {
    TRACER("CGlxBitmapDecoderWrapper:: DoCancel ");
    //Get the orientation and set rotation on the decoder 
    //as well as update the original size
    aRotAngle =  0;
    aFlipStatus = EFalse;
    TInt isOrientationOdd = aOrientation%2;
    if(aOrientation>8)
        {
        return;
        }
    if(aOrientation >= 3 && aOrientation < 5)
        {
        aRotAngle = 180;
        }
    else if(aOrientation >= 5 && aOrientation < 7)
        {
        aRotAngle = 90;

        }
    else if(aOrientation >= 7 && aOrientation <= 8)
        {
        aRotAngle = 270;
        }
    if(aOrientation>4 )
        {
        if(isOrientationOdd )
            {
            aFlipStatus = ETrue;
            }
        }   

    }

// ---------------------------------------------------------------------------
// OOMRequestFreeMemoryL
// ---------------------------------------------------------------------------
//
TInt CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL( TInt aBytesRequested)
    {
    TRACER("CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL");
    GLX_LOG_INFO1("CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL() aBytesRequested=%d",
            aBytesRequested);
    ROomMonitorSession oomMonitor;
    User::LeaveIfError( oomMonitor.Connect() );
    // No leaving code after this point, so no need to use cleanup stack
    // for oomMonitor
    TInt errorCode = oomMonitor.RequestFreeMemory( aBytesRequested );
    GLX_LOG_INFO1("CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL(1) errorCode=%d",errorCode);
    if ( errorCode != KErrNone )
        {
        // try one more time 
        errorCode = oomMonitor.RequestFreeMemory( aBytesRequested );
        GLX_LOG_INFO1("CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL(2) errorCode=%d",errorCode);
        }
    oomMonitor.Close();
    return errorCode;
    }
