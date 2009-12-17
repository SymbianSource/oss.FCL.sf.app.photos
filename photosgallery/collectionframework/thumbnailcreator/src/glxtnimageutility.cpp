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
* Description:    Utility for creating bitmaps from video files
*
*/




/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

#include "glxtnimageutility.h"

#include <MIHLScaler.h>    // MIHLScaler
#include <fbs.h>
#include <glxtracer.h>
#include <glxpanic.h>
#include <glxthumbnail.h>
#include <imageconversion.h>

#include "glxtnimagedecoderfactory.h"

#include <ExifRead.h>
#include <IclExtJpegApi.h>

// All EXIF data is within the first KGlxMaxExifSize bytes of the file
const TInt KGlxMaxExifSize = 0x10000;
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnImageUtility::CGlxtnImageUtility(RFs& aFs) : iFs(aFs)
    {
    TRACER("CGlxtnImageUtility::CGlxtnImageUtility()");
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnImageUtility::~CGlxtnImageUtility()
    {
    TRACER("CGlxtnImageUtility::~CGlxtnImageUtility()");
    delete iDecoder;
    delete iScaler;
    delete iBitGc;
    }

// -----------------------------------------------------------------------------
// Cancel
// -----------------------------------------------------------------------------
//
void CGlxtnImageUtility::Cancel()
    {
    TRACER("void CGlxtnImageUtility::Cancel()");
	if ( iDecoder ) 
		{
		iDecoder->Cancel();
		}
    if ( iScaler && iScaler->IsBusy())
        {
        iScaler->CancelProcess();
        }
    }

// -----------------------------------------------------------------------------
// AdjustSize
// -----------------------------------------------------------------------------
//
void CGlxtnImageUtility::AdjustSize(TSize& aTargetSize, const TSize& aSourceSize)
    {
    TRACER("void CGlxtnImageUtility::AdjustSize()");
	if( (aSourceSize.iWidth <= aTargetSize.iWidth ) && (aSourceSize.iHeight  <=  aTargetSize.iHeight ))
	{
	    aTargetSize.iWidth  = aSourceSize.iWidth;
		aTargetSize.iHeight = aSourceSize.iHeight;
	}
    else if ( aTargetSize.iHeight * aSourceSize.iWidth
            < aTargetSize.iWidth * aSourceSize.iHeight )
        {
        // Source has taller aspect than target so reduce target width
        aTargetSize.iWidth =  ( ( aTargetSize.iHeight * aSourceSize.iWidth )
                                / (  aSourceSize.iHeight ) );
        }
    else
        {
        // Source has wider aspect than target so reduce target height
        aTargetSize.iHeight = (aTargetSize.iWidth * aSourceSize.iHeight)
                                / aSourceSize.iWidth;
        }
    }

// -----------------------------------------------------------------------------
// DecodeImageL
// -----------------------------------------------------------------------------
//
CFbsBitmap* CGlxtnImageUtility::DecodeImageL(TRequestStatus& aStatus,
                const TDesC& aFileName, RArray<TSize>& aTargetSizes, TDisplayMode aMode)
    {
    TRACER("CFbsBitmap* CGlxtnImageUtility::DecodeImageL()");
    delete iDecoder;
    iDecoder = NULL;
    //Get Exif Metadata and the orientation tag from the file first
    TUint16 orientation = GetOrientationL(aFileName);
    TInt isExtDecoderUsed = 0;
    ///iDecoder = GlxtnImageDecoderFactory::NewL( iFs, aFileName );
    iDecoder = GlxtnImageDecoderFactory::NewL( iFs, aFileName, isExtDecoderUsed );
    iOriginalSize = iDecoder->FrameInfo().iOverallSizeInPixels;
    //Get the orientation and set rotation on the decoder 
    //as well as update the original size
    if (isExtDecoderUsed)
        {
        TInt rotAngle =  0;
        TBool flip = EFalse;
        GetRotationParameters(orientation,rotAngle,flip);
        CExtJpegDecoder* extDecoder = (CExtJpegDecoder*)iDecoder;
        extDecoder->SetRotationL(rotAngle);
        if(flip)
            {
            extDecoder->SetMirroringL();
            }
        }
    if ( 0 == iOriginalSize.iWidth || 0 == iOriginalSize.iHeight )
        {
        User::Leave(KErrCorrupt);
        }
    
    TInt count = aTargetSizes.Count();
    TSize testSize;
    TSize targetSize;
    for ( TInt i = 0 ; i < count ; i++ )
        {
        testSize = aTargetSizes[i];
        AdjustSize(testSize, iOriginalSize);
        if( ( testSize.iWidth > targetSize.iWidth ) || ( testSize.iHeight > targetSize.iHeight ) )
            {
            targetSize = testSize;
            }
        }

    // Find max scaling factor which won't make image smaller than target size
    TInt shift = 3;
    while ( shift
            && ((iOriginalSize.iWidth >> shift) < targetSize.iWidth
                || (iOriginalSize.iHeight >> shift) < targetSize.iHeight ) )
        {
        shift--;
        }
    TInt roundUp = (1 << shift) - 1;  
    TSize loadSize((iOriginalSize.iWidth + roundUp) >> shift,
                    (iOriginalSize.iHeight + roundUp) >> shift);

    CFbsBitmap* image = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(image);
    User::LeaveIfError(image->Create(loadSize, aMode));

    iDecoder->Convert(&aStatus, *image);

    CleanupStack::Pop(image);
    return image;
	}

// -----------------------------------------------------------------------------
// FreeDecoder
// -----------------------------------------------------------------------------
//
void CGlxtnImageUtility::FreeDecoder()
    {
    TRACER("void CGlxtnImageUtility::FreeDecoder()");
    delete iDecoder;
    iDecoder = NULL;
    }

// -----------------------------------------------------------------------------
// OriginalSize
// -----------------------------------------------------------------------------
//
const TSize& CGlxtnImageUtility::OriginalSize() const
    {
    TRACER("TSize& CGlxtnImageUtility::OriginalSize()");
    return iOriginalSize;
    }

//---------------------------------------------------------------------------
/**
    K-Nearest neighbour scaling for EColor64K.
    Code originally inherited from t_display.cpp by Toni Hemminki.
    Added windowing and further optimized by Jarmo Nikula 2006-2007.

    @param  aSrc       Source buffer pointer
    @param  aSrcStride Source buffer stride in bytes
    @param  aSrcCols   Source columns
    @param  aSrcRows   Source rows
    @param  aX   			 Source window top-left X-coordinate
    @param  aY   			 Source window top-left Y-coordinate
    @param  aW   			 Source window width
    @param  aH   			 Source window height
    @param  aDst       Destination buffer pointer
    @param  aDstStride Destination buffer stride in bytes
    @param  aDstCols   Destination columns, must be even
    @param  aDstRows   Destination rows
*/
// OPTION's for MMP file:
// Interleave assembly code with C, output can be found on txt-files on some BUILD directory:
// OPTION ARMCC --asm --interleave
// Modify optimization for ARM insturcion set and for maximum speed
// OPTION_REPLACE ARMCC --arm -Otime
/*    aSource->LockHeap();
    TUint16* sourceAddress = (TUint16*)aSource->DataAddress();
	aSource->UnlockHeap();
    aTarget->LockHeap();
    TUint16* targetAddress = (TUint16*)aTarget->DataAddress();
	aTarget->UnlockHeap();
    ScaleColor64K(sourceAddress, aSource->DataStride(), 
        sourceSize.iWidth, sourceSize.iHeight, 0, 0,  sourceSize.iWidth, sourceSize.iHeight,
        targetAddress, aTarget->DataStride(), 
        targetSize.iWidth, targetSize.iHeight);
*/

void CGlxtnImageUtility::FilterImageL(TRequestStatus* aStatus, CFbsBitmap* aSource, CFbsBitmap*& aFilteredSource, CFbsBitmap* aTarget)
    {
    TRACER("void CGlxtnImageUtility::FilterImageL()");
    TSize sourceSize(aSource->SizeInPixels());
    TSize targetSize(aTarget->SizeInPixels());
    
    //Fix for EKLG-7PTET2:mpxcollectionserver.exe crash in thumbnailcreator because of div by zero event
    if (targetSize.iWidth <= 0)
        {
        User::Leave(KErrDivideByZero);
        }
    
    TInt scalePercent = ((100*sourceSize.iWidth)/targetSize.iWidth) - 100;

    // no need to filter if close enough or smaller
    if ( scalePercent >= 2 && ( aTarget->DisplayMode() == EColor64K ) )
        {
        FilterL(aSource, aFilteredSource, scalePercent);
        }
    
    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone);
    }

// -----------------------------------------------------------------------------
// ScaleImageL
// -----------------------------------------------------------------------------
//
void CGlxtnImageUtility::ScaleImageL(TRequestStatus& aStatus, CFbsBitmap& aSrcBitmap,  
										const TRect& aSrcRect,	CFbsBitmap& aDstBitmap,
																	const TRect& aDstRect)
	{
    TRACER("void CGlxtnImageUtility::ScaleImageL(IHL)");
	if (!iScaler)
		{
		iScaler = IHLScaler::CreateL();	
		}
	User::LeaveIfError(iScaler->Scale(aStatus, aSrcBitmap, aSrcRect, aDstBitmap, aDstRect));      
    }

void CGlxtnImageUtility::ScaleImage64kL(TRequestStatus* aStatus, CFbsBitmap* aSource, CFbsBitmap* aFilteredSource, CFbsBitmap* aTarget)
    {
    TRACER("void CGlxtnImageUtility::ScaleImage64kL()");
    TSize sourceSize(aSource->SizeInPixels());
    TSize targetSize(aTarget->SizeInPixels());
    TInt scalePercent = ((100*sourceSize.iWidth)/targetSize.iWidth) - 100;
    TRect targetRect(targetSize);
    TRect sourceRect(sourceSize);

    CFbsBitmap* sourceImage = aFilteredSource;
    if ( !aFilteredSource )
        {
        sourceImage = aSource;
        }
    if ( scalePercent < 2 )
        {
        if ( scalePercent > 0 )
            {
            sourceRect = targetRect;
            }
        sourceImage = aSource;
        }
    sourceImage->LockHeap();
    TUint16* sourceAddress = (TUint16*)sourceImage->DataAddress();
	sourceImage->UnlockHeap();
    aTarget->LockHeap();
    TUint16* targetAddress = (TUint16*)aTarget->DataAddress();
	aTarget->UnlockHeap();
    ScaleColor64K(sourceAddress, sourceImage->DataStride(), 
        sourceImage->SizeInPixels().iWidth, sourceImage->SizeInPixels().iHeight, 0, 0,  sourceImage->SizeInPixels().iWidth, sourceImage->SizeInPixels().iHeight,
        targetAddress, aTarget->DataStride(), 
        targetSize.iWidth, targetSize.iHeight);
    
/*    if ( !iBitGc )
        {
        iBitGc = CFbsBitGc::NewL();
        }
    CFbsBitmapDevice *bitmapDevice = CFbsBitmapDevice::NewL(aTarget);
    CleanupStack::PushL(bitmapDevice);
    iBitGc->Activate(bitmapDevice);
    iBitGc->DrawBitmap(targetRect, sourceImage, sourceRect); 
    CleanupStack::PopAndDestroy(bitmapDevice);
*/
    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone);
    }

void CGlxtnImageUtility::ScaleColor64K( TUint16* aSrc, TInt aSrcStride, TInt /*aSrcCols*/, TInt /*aSrcRows*/,
	  								TInt aX, TInt aY, TInt aW, TInt aH,
          		      TUint16* aDst, TInt aDstStride, TInt aDstCols, TInt aDstRows )
    {
    TRACER("void CGlxtnImageUtility::ScaleColor64K()");
    const TUint KPrecision = 16;
    TUint hInc = ( aW<< KPrecision ) / aDstCols;
    TUint vInc = ( aH << KPrecision ) / aDstRows;
    TUint v = 0;
    for ( TUint row = aDstRows; row > 0; --row )
        {
        TInt h = hInc * (aDstCols - 1);
        TUint linestart = aSrcStride * ( aY + ( v >> KPrecision ) ) / sizeof(TUint16) + aX;
        TUint16* src = &aSrc[ linestart ];
        TUint32* dst = (TUint32*)aDst + aDstCols / 2;
        TUint32 pxl2x;
        // This loop generates 11 assembly instructions per round
        // when using "--arm --Otime" options. Since two pixels are handled per round,
        // it means 5.5 instructions per pixel on average.
        do  {
            pxl2x = src[ h >> KPrecision ]<<16;
            h -= hInc;
            pxl2x |= src[ h >> KPrecision ];
            *--dst = pxl2x;
            // Compiler eliminates CMP instruction when substraction
            // is done inside the while () statement.
            } while ( ( h -= hInc ) >= 0 );
        v += vInc;
        aDst += aDstStride / sizeof(TUint16);
        }
    }

void CGlxtnImageUtility::FilterL( CFbsBitmap* aSource, CFbsBitmap*& aFilteredSource, TInt aFilterPercent )
	{
    TRACER("void CGlxtnImageUtility::FilterL()");
	if ( !aFilteredSource )
	    {
        TSize imageSize = aSource->SizeInPixels();
        aFilteredSource =  new (ELeave) CFbsBitmap;
        aFilteredSource->Create(imageSize, aSource->DisplayMode());
        
        CFbsBitmapDevice *bitmapDevice = CFbsBitmapDevice::NewL(aFilteredSource);
        if ( !iBitGc )
            {
            iBitGc = CFbsBitGc::NewL();
            }
        iBitGc->Activate(bitmapDevice);
        iBitGc->BitBlt(TPoint(), aSource, imageSize); 
        delete bitmapDevice;
        }

    TSize imageSize = aFilteredSource->SizeInPixels();
    aFilteredSource->LockHeap();
    TUint16* sourceAddress = (TUint16*)aFilteredSource->DataAddress();
	aFilteredSource->UnlockHeap();
	const TInt KGlxScalingNeeds4Filtering = 80; // if scaling more than 1.8 need to filter by 4
	const TInt KGlxScalingNeeds8Filtering = 260; // if scaling more than 3.6 need to filter by 8
	if ( KGlxScalingNeeds8Filtering < aFilterPercent )
	    {
       	FIRFiltering8( 
    		sourceAddress, aFilteredSource->DataStride() / sizeof(TUint16),
    		imageSize.iWidth, imageSize.iHeight );
	    }
	else if ( KGlxScalingNeeds4Filtering < aFilterPercent )
	    {
       	FIRFiltering4( 
    		sourceAddress, aFilteredSource->DataStride() / sizeof(TUint16),
    		imageSize.iWidth, imageSize.iHeight );
	    }
	else
	    {
       	FIRFiltering( 
    		sourceAddress, aFilteredSource->DataStride() / sizeof(TUint16),
    		imageSize.iWidth, imageSize.iHeight );
	    }
	}

#define rmask565	0xf800
#define gmask565	0x07e0
#define bmask565	0x001f
#define rbmask565	(rmask565|bmask565)
#define rm565( rgb ) 		((rgb) & rmask565)
#define gm565( rgb ) 		((rgb) & gmask565)
#define bm565( rgb ) 		((rgb) & bmask565)
#define rbm565( rgb ) 		((rgb) & rbmask565)
#define r565( rgb ) 		((rgb)>>11)
#define g565( rgb ) 		(gm565(rgb)>>5)
#define b565( rgb ) 		(bm565(rgb))
#define rgb565( r, g, b )	(((r)<<11)|((g)<<5)|(b))

#define mask32gbr655 0x07e0f81f

// Keep below three defs in sync with each other!
#define KFIRLen    2
#define KFIRCen	   (KFIRLen / 2)
#define incFIRIndex( i ) i = (i + 1) & (KFIRLen - 1)
// Keep above three defs in sync with each other!

void CGlxtnImageUtility::FIRFiltering(
		TUint16* aDst, TUint aDstStridep, TUint aDstCols, TUint aDstRows )
    {
    TRACER("void CGlxtnImageUtility::FIRFiltering( )");
    TUint firFifo[ KFIRLen ];
    TUint i; // index for fifo in&out
    TUint16 *p;
    TUint32 px;
    TInt row, col;
    TUint FIRsum;
    // Horizontal scan.
    p = aDst;
    for ( row = aDstRows - 1; row >= 0; row-- )
        {
        // read for cache
	    //for ( col = aDstCols - 1; col >= 0; col-- ) TInt temp = p[ col ];
        // Fill in the FIR first.
        // TODO: Fill in with extrapolated values at edges!
        FIRsum = ((KFIRLen / 2)<<21) | ((KFIRLen / 2)<<11) | (KFIRLen / 2); // for correct rounding
        i = 0;
        TUint32 mask1 = mask32gbr655;
	    for ( col = 0; col < KFIRLen; col++ )
	    	{
	    	px = p[ col ];
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	    	incFIRIndex( i );
	    	}
        TUint32 ave;
	    for ( ; col < aDstCols; col++ )
	    	{
	    	ave = ( FIRsum / KFIRLen ) & mask1;
	    	p[ col - KFIRCen ] = TUint16( ave | (ave>>16) );
	        FIRsum -= firFifo[ i ];
	    	px = p[ col ];
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	        incFIRIndex( i );
            }
        p += aDstStridep;
        }
    // Vertical scan.
    for ( col = aDstCols - 1; col >= 0; col-- )
        {
        // Fill in the FIR first.
        FIRsum = ((KFIRLen / 2)<<21) | ((KFIRLen / 2)<<11) | (KFIRLen / 2); // for correct rounding
        TUint32 mask1 = mask32gbr655;
        i = 0;
	    p = aDst + col;
	    for ( row = 0; row < KFIRLen; row++ )
	    	{
	    	px = *p;
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	    	incFIRIndex( i );
	    	p += aDstStridep;
            }
        TUint32 ave;
    	p -= aDstStridep * KFIRCen;
	    for ( ; row < aDstRows; row++ )
	    	{
	    	ave = ( FIRsum / KFIRLen ) & mask1;
	    	p[0] = TUint16( ave | (ave>>16) );
	        FIRsum -= firFifo[ i ];
	    	px = p[ aDstStridep * KFIRCen ];
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	    	incFIRIndex( i );
	        p += aDstStridep;
            }
        }
    }
//
// Keep below three defs in sync with each other!
#define KFIRLen4    4
#define KFIRCen4	   (KFIRLen4 / 2)
#define incFIRIndex4( i ) i = (i + 1) & (KFIRLen4 - 1)
// Keep above three defs in sync with each other!

void CGlxtnImageUtility::FIRFiltering4(
		TUint16* aDst, TUint aDstStridep, TUint aDstCols, TUint aDstRows )
    {
    TRACER("void CGlxtnImageUtility::FIRFiltering4()");
    TUint firFifo[ KFIRLen4 ];
    TUint i; // index for fifo in&out
    TUint16 *p;
    TUint32 px;
    TInt row, col;
    TUint FIRsum;
    // Horizontal scan.
    p = aDst;
    for ( row = aDstRows - 1; row >= 0; row-- )
        {
        // read for cache
	    //for ( col = aDstCols - 1; col >= 0; col-- ) TInt temp = p[ col ];
        // Fill in the FIR first.
        // TODO: Fill in with extrapolated values at edges!
        FIRsum = ((KFIRLen4 / 2)<<21) | ((KFIRLen4 / 2)<<11) | (KFIRLen4 / 2); // for correct rounding
        i = 0;
        TUint32 mask1 = mask32gbr655;
	    for ( col = 0; col < KFIRLen4; col++ )
	    	{
	    	px = p[ col ];
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	    	incFIRIndex4( i );
	    	}
        TUint32 ave;
	    for ( ; col < aDstCols; col++ )
	    	{
	    	ave = ( FIRsum / KFIRLen4 ) & mask1;
	    	p[ col - KFIRCen4 ] = TUint16( ave | (ave>>16) );
	        FIRsum -= firFifo[ i ];
	    	px = p[ col ];
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	        incFIRIndex4( i );
            }
        p += aDstStridep;
        }
    // Vertical scan.
    for ( col = aDstCols - 1; col >= 0; col-- )
        {
        // Fill in the FIR first.
        FIRsum = ((KFIRLen4 / 2)<<21) | ((KFIRLen4 / 2)<<11) | (KFIRLen4 / 2); // for correct rounding
        TUint32 mask1 = mask32gbr655;
        i = 0;
	    p = aDst + col;
	    for ( row = 0; row < KFIRLen4; row++ )
	    	{
	    	px = *p;
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	    	incFIRIndex4( i );
	    	p += aDstStridep;
            }
        TUint32 ave;
    	p -= aDstStridep * KFIRCen4;
	    for ( ; row < aDstRows; row++ )
	    	{
	    	ave = ( FIRsum / KFIRLen4 ) & mask1;
	    	p[0] = TUint16( ave | (ave>>16) );
	        FIRsum -= firFifo[ i ];
	    	px = p[ aDstStridep * KFIRCen4 ];
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	    	incFIRIndex4( i );
	        p += aDstStridep;
            }
        }
    }

// Keep below three defs in sync with each other!
#define KFIRLen8    8
#define KFIRCen8	   (KFIRLen8 / 2)
#define incFIRIndex8( i ) i = (i + 1) & (KFIRLen8 - 1)
// Keep above three defs in sync with each other!

void CGlxtnImageUtility::FIRFiltering8(
		TUint16* aDst, TUint aDstStridep, TUint aDstCols, TUint aDstRows )
    {
    TRACER("void CGlxtnImageUtility::FIRFiltering8()");
    TUint firFifo[ KFIRLen8 ];
    TUint i; // index for fifo in&out
    TUint16 *p;
    TUint32 px;
    TInt row, col;
    TUint FIRsum;
    // Horizontal scan.
    p = aDst;
    for ( row = aDstRows - 1; row >= 0; row-- )
        {
        // read for cache
	    //for ( col = aDstCols - 1; col >= 0; col-- ) TInt temp = p[ col ];
        // Fill in the FIR first.
        // TODO: Fill in with extrapolated values at edges!
        FIRsum = ((KFIRLen8 / 2)<<21) | ((KFIRLen8 / 2)<<11) | (KFIRLen8 / 2); // for correct rounding
        i = 0;
        TUint32 mask1 = mask32gbr655;
	    for ( col = 0; col < KFIRLen8; col++ )
	    	{
	    	px = p[ col ];
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	    	incFIRIndex8( i );
	    	}
        TUint32 ave;
	    for ( ; col < aDstCols; col++ )
	    	{
	    	ave = ( FIRsum / KFIRLen8 ) & mask1;
	    	p[ col - KFIRCen8 ] = TUint16( ave | (ave>>16) );
	        FIRsum -= firFifo[ i ];
	    	px = p[ col ];
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	        incFIRIndex8( i );
            }
        p += aDstStridep;
        }
    // Vertical scan.
    for ( col = aDstCols - 1; col >= 0; col-- )
        {
        // Fill in the FIR first.
        FIRsum = ((KFIRLen8 / 2)<<21) | ((KFIRLen8 / 2)<<11) | (KFIRLen8 / 2); // for correct rounding
        TUint32 mask1 = mask32gbr655;
        i = 0;
	    p = aDst + col;
	    for ( row = 0; row < KFIRLen8; row++ )
	    	{
	    	px = *p;
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	    	incFIRIndex8( i );
	    	p += aDstStridep;
            }
        TUint32 ave;
    	p -= aDstStridep * KFIRCen8;
	    for ( ; row < aDstRows; row++ )
	    	{
	    	ave = ( FIRsum / KFIRLen8 ) & mask1;
	    	p[0] = TUint16( ave | (ave>>16) );
	        FIRsum -= firFifo[ i ];
	    	px = p[ aDstStridep * KFIRCen8 ];
            px = ((px<<16) | px) & mask1;
	    	firFifo[ i ] = px;
	        FIRsum += px;
	    	incFIRIndex8( i );
	        p += aDstStridep;
            }
        }
    }
// -----------------------------------------------------------------------------
// GetOrientation
// -----------------------------------------------------------------------------
//
TUint16 CGlxtnImageUtility::GetOrientationL(const TDesC& aFileName)
    {
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

// -----------------------------------------------------------------------------
// GetRotationParameters
// -----------------------------------------------------------------------------
//
void CGlxtnImageUtility::GetRotationParameters(TUint16 aOrientation, 
                                            TInt& aRotAngle, TBool& aFlipStatus)
    {
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
        iOriginalSize.SetSize(iOriginalSize.iHeight,iOriginalSize.iWidth); //Switch Height and width
        if(isOrientationOdd )
            {
            aFlipStatus = ETrue;
            }
        }
    }
