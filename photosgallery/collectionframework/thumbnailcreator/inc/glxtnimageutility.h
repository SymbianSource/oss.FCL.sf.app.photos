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
* Description:    Utility for creating bitmaps from image files
*
*/




/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef GLXTNIMAGEUTILITY_H
#define GLXTNIMAGEUTILITY_H

#include <e32base.h>
#include <gdi.h>        // For TDisplayMode

class MIHLScaler;
class CFbsBitmap;
class CFbsBitGc;
class CImageDecoder;
class RFs;

/**
 * Utility for creating bitmaps from image files
 *
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS(CGlxtnImageUtility) : public CBase
    {
public:
    CGlxtnImageUtility(RFs& aFs);
    ~CGlxtnImageUtility();

    void Cancel();

    /**
    * Reduce target size to same aspect ratio as source image.
    */
    void AdjustSize(TSize& aTargetSize, const TSize& aSourceSize);

    /**
    * Start asynchronous decoding process for a high quality thumbnail.
    * @param aStatus Request status for the asynchronous operation.
    * @param aFileName Full path of image file.
    * @param aTargetSize Size for the image - modified by this function to
    *           preserve the aspect ratio.
    * @param aMode Display mode for the image.
    */
    CFbsBitmap* DecodeImageL(TRequestStatus& aStatus, const TDesC& aFileName,
                                RArray<TSize>& aTargetSizes, TDisplayMode aMode);

    /**
    * Free resources used by decoding.
    */
    void FreeDecoder();

    /**
    * Get the original size of the image decoded.
    */
    const TSize& OriginalSize() const;

    /**
    * Start asynchronous filtering process for a high quality thumbnail.
    * @param aStatus Request status for the asynchronous operation.
    * @param aSource Source image bitmap.
    * @param aFilteredSource Filtered  image bitmap.
    * @param aTarget Target thumbnail bitmap.
    */
    void FilterImageL(TRequestStatus* aStatus, CFbsBitmap* aSource, CFbsBitmap*& aFilteredSource, CFbsBitmap* aTarget);
    
    /**
    * Start asynchronous scaling process for a high quality thumbnail.
    * @param aStatus Request status for the asynchronous operation.
    * @param aSrcBitmap Source image bitmap.
    * @param aSrcRect Source image rectangle.
    * @param aDstBitmap Destination thumbnail bitmap.
    * @param aDstRect Destination image rectangle.
    */
	void ScaleImageL(TRequestStatus& aStatus, CFbsBitmap& aSrcBitmap, const TRect& aSrcRect, CFbsBitmap& aDstBitmap, const TRect& aDstRect);

    /**
    * Start asynchronous scaling process for a high quality thumbnail.
    * @param aStatus Request status for the asynchronous operation.
    * @param aSource Source image bitmap.
    * @param aFilteredSource Filtered Source image bitmap.
    * @param aTarget Target thumbnail bitmap.
    */
    void ScaleImage64kL(TRequestStatus* aStatus, CFbsBitmap* aSource, CFbsBitmap* aFilteredSource, CFbsBitmap* aTarget);

private:
    
    void ScaleColor64K( TUint16* aSrc, TInt aSrcStride, TInt aSrcCols, TInt aSrcRows,
	  								TInt aX, TInt aY, TInt aW, TInt aH,
          		      TUint16* aDst, TInt aDstStride, TInt aDstCols, TInt aDstRows );
          		      
    void FilterL( CFbsBitmap* aSource, CFbsBitmap*& aFilteredSource, TInt aFilterPercent );

    void FIRFiltering(
		TUint16* aDst, TUint aDstStridep, TUint aDstCols, TUint aDstRows );

    void FIRFiltering4(
		TUint16* aDst, TUint aDstStridep, TUint aDstCols, TUint aDstRows );

    void FIRFiltering8(
		TUint16* aDst, TUint aDstStridep, TUint aDstCols, TUint aDstRows );
    /*
     * Get the orientation of the file
     * This API reads the Exif tag in an image and returns the orientation of that Image 
     */
    TUint16 GetOrientationL(const TDesC& aFileName) ;
    /*
     * Get the Rotation Angle and Mirroring status of the image so as to set it on the decoder 
     * This API calculates the rotation angle and the Mirroring status required by the decoder
     * @param aOrientation Orientation of the image.
     * @param aRotAngle Rotation angle to be set on the image decoder.
     * @param aFlipStatus Mirroring status for the decoder if 1 then mirroring should be set.
     * Internally it also sets the iOriginalSize to the required size
     */
    void GetRotationParameters(TUint16 aOrientation, TInt& aRotAngle, TBool& aFlipStatus) ;
private:
    // File server session
    RFs& iFs;
    // Decoder used to read image from file
    CImageDecoder* iDecoder;
    // Bitmap scaler
    MIHLScaler* iScaler;
    /** Size of the image in the file being decoded */
    TSize iOriginalSize;
    CFbsBitmap* iImage;
    TUint16* iAddress;
    CFbsBitGc* iBitGc;
    };

#endif  // GLXTNIMAGEUTILITY_H
