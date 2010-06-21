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
     */
    virtual void RunL();
    virtual void DoCancel();

private:
    /**
     * Constructor
     */
    CGlxBitmapDecoderWrapper();
    
    /**
     * Second-phase constuction 
     */
    void ConstructL(MGlxBitmapDecoderObserver* aObserver);
    
    /**
     * If the image format is non jpeg, then we need to calculate as per
     * reduction factor and reduced size as what the decoder is going to return us
     * This function returns if that needs to be done. 
     */
    TBool DoesMimeTypeNeedsRecalculateL();
    
    /**
     * Recalculate the size for png/bmp as decoder fails to 
     * decode for desired size 
     */
    TSize ReCalculateSizeL();

private:    
    /* Contains the TextureManagerImpl object,calls the HandleBitmapDecoded*/
    MGlxBitmapDecoderObserver* iObserver;
    /*Specifies the Decoder */
    CImageDecoder* iImageDecoder; // decoder from ICL API
    /*Contains the Thumbnail Index*/
    TInt iThumbnailIndex;
    /*Contains the Bitmap generated*/
    CFbsBitmap* iBitmap;
    /*contains the original size of the image*/
    TAlfRealSize iOriginalSize;
    /*A handle to a file server session.*/ 
    RFs iFs;
    /*To store the target image size*/
    TSize iTargetBitmapSize;
    /*To store the image uri path*/ 
    HBufC* iImagePath;                     
#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif
    };

#endif //C_GLXBITMAPDECODERWRAPPER_H
