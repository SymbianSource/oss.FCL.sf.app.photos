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
// For decoding via TNM 
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include <thumbnailobjectsource.h>
#include <thumbnaildata.h>
#include <glxmedia.h>

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

class CGlxBitmapDecoderWrapper: public CBase
                               ,public MThumbnailManagerObserver
    
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
    void DoDecodeImageL(const TGlxMedia& aMedia,TInt aIndex);

    /**
     * starts the decoding and updates the state of the decoding
     */
    void DecodeImageL();
    /**
     * Request free memory with Out of Memory Monitor
     * @param aBytesRequested : Request for free memory in Bytes
     */
    TInt OOMRequestFreeMemoryL( TInt aBytesRequested);
    /**
     * Cancels all the pending requests and release resources
     */
    void CancelRequest();

    
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
     * Gets Thumbnails from TNM. 
     */
    void GetThumbnailL( HBufC* aImagePath );
    /**
    *  From MThumbnailManagerObserver, not used
    */
    virtual void ThumbnailPreviewReady( MThumbnailData& aThumbnail, 
                                TThumbnailRequestId aId );
                                
    /**
    *  From MThumbnailManagerObserver
    */  
    virtual void ThumbnailReady( TInt aError, 
                         MThumbnailData& aThumbnail, 
                         TThumbnailRequestId aId );

private:    
    /* Contains the TextureManagerImpl object,calls the HandleBitmapDecoded*/
    MGlxBitmapDecoderObserver* iObserver;
    /*Contains the Thumbnail Index*/
    TInt iThumbnailIndex;
    /*Contains the Bitmap generated*/
    CFbsBitmap* iBitmap;
    /*contains the original size of the image*/
    TAlfRealSize iOriginalSize;
    /*To store the target image size*/
    TSize iTargetBitmapSize;
    /*To store the image uri path*/ 
    HBufC* iImagePath;                     
#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif

    CThumbnailManager* iTnManager;
    TThumbnailRequestId iTnReqId;
    };

#endif //C_GLXBITMAPDECODERWRAPPER_H
