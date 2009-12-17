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
* Description:    Texture Manager component/Decodes the Image in step for large images
 *
*/




#ifndef C_GLXBITMAPDECODERWRAPPER_H
#define C_GLXBITMAPDECODERWRAPPER_H

// INCLUDE FILES
#include<alf/alftimedvalue.h>
#include <IclExtJpegApi.h>  // For CExtJpegDecoder

/**
 * Observer interface used to be notified when texture bitmap   is completed.
 */
NONSHARABLE_CLASS( MGlxBitmapDecoderObserver )
        {
        public:
            /**
             * Called when the content of the texture is changed.
             * @param aHasContent Whether the texture now has content.
             */
            virtual void HandleBitmapDecodedL(TInt aThumbnailIndex,CFbsBitmap* aBitmap) = 0;
        };
/**
 * This class does the decoding of the images in steps depending on the size of the images .
 * this is a wrapper class, doesn't actually decode the images.
 */

class CGlxBitmapDecoderWrapper: public CActive
    {
public:
    /*This Enums specifies the state of the Decoding*/
    enum TDecoderState
        {
        ESTate0, ESTate1,ESTateFinal
        };  
    /*This Enums specifies the size of the images*/
    enum TImageLevel
        {
        ELevel0, ELevel1,ELevel2
        }; 
    /**
     * Two-phased constructor
     * @param aObserver:The Observer contains the CGlxTextureManagerImpl Object
     * @return Constructed object
     */
    static CGlxBitmapDecoderWrapper* NewL(MGlxBitmapDecoderObserver* aObserver);

    /**
     * Destructor
     */
    ~CGlxBitmapDecoderWrapper();

    /**
     * intiates the Decoding the Image and updates the no:of steps to decode
     * @param aSourceFileName :contains the file name
     * @param aindex:contains the index value
     */
    void DoDecodeImageL(const TDesC & aSourceFileName,TInt aIndex);

    /**
     * starts the decoding and updates the state of the decoding
     */
    void DecodeImageL();
    /**
     * Request free memory with Out of Memory Monitor
     * @param aBytesRequested : Request for free memory in Bytes
     */
    TInt OOMRequestFreeMemoryL( TInt aBytesRequested);

public:    
    /**
     * Standard Active object functions
     * */
    virtual void RunL();
    virtual void DoCancel();

private:
    /*
     * Constructor
     * */
    CGlxBitmapDecoderWrapper();
    
    /*
     *Second-phase constuction 
     * */
    void ConstructL(MGlxBitmapDecoderObserver* aObserver);
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
    /* Contains the TextureManagerImpl object,calls the HandleBitmapDecoded*/
    MGlxBitmapDecoderObserver* iObserver;
    /*Specifies the Decoder */
    CImageDecoder*          iImageDecoder; // decoder from ICL API
    /*Contains the Thumbnail Index*/
    TInt iThumbnailIndex;
    /*Contains the Bitmap generated*/
    CFbsBitmap* iBitmap;
    /*contains the original size of the image*/
    TAlfRealSize iOriginalSize;
    /*A handle to a file server session.*/ 
    RFs iFs;
    /*Specifies the state of decoding ,if it is in initial level(3MP), Middlelevel(6MP) or final level*/
    TDecoderState iDecoderState;
    /*Specifies the size of teh image and levels to decode*/
    TImageLevel   iImageLevel;
#ifdef _DEBUG
    
    TTime iDecodeProcessstartTime;
    TTime iStartTime;
    TTime iStopTime;
#endif
    };

#endif //C_GLXBITMAPDECODERWRAPPER_H
