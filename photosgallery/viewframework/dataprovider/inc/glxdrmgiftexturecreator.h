/* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Gif Texture creator implementation
*
*/

#ifndef GLXDRMGIFTEXTURECREATOR_H_
#define GLXDRMGIFTEXTURECREATOR_H_

#include <w32std.h>
#include <fbs.h>
#include <icl\imagedata.h>

class CGlxDRMgifDecoderAO;
class CImageDecoder;
class CGlxUiUtility;
class CGlxBinding;
class TGlxMedia;                            // Reference to a media item in the item pool
class CAlfEnv;

namespace Alf
    {
    class IMulModel;                        // An interface for the data model
    class IMulWidget;                       // An interface for all Multimedia widgets
    class MulVisualItem;                    // Client need to use this class to add data in data model
    }

const TInt KGlxMaxFrameCount = 25;

class CGlxDrmGifTextureCreator: public CBase
    {
public:
    /*
     * 
     */
    static CGlxDrmGifTextureCreator* NewL(const CGlxBinding& aBinding,
            const TGlxMedia& aMedia, TInt aItemIndex,  Alf::IMulModel* aModel);
    
    /*
     * destructor
     */
    ~CGlxDrmGifTextureCreator();
public:
    /*
     * This is to cancel the active object from decoding 
     */
    void HandleRunL(TRequestStatus& aStatus);

    /*
     * This updates the new image.
     */
    void UpdateNewImageL(const TGlxMedia& aMedia, TInt aItemIndex);
    
    /*
     * Enable/Disable the animation
     */
    void AnimateDRMGifItem(TBool aAnimate);

private:
    /*
     * Ctor 
     */
    CGlxDrmGifTextureCreator(const CGlxBinding& aBinding,
            const TGlxMedia& aMedia, TInt aItemIndex,  Alf::IMulModel* aModel);
    
    /*
     * ConstructL()
     */
    void ConstructL();   
    
    /*
     * Call a refresh on the screen  
     */
    void RefreshL();
    
    /*
     * Release contents 
     */
    void ReleaseContent();
    
    /*
     * Create bitmap  
     */
    void CreateBitmapAndStartDecodingL();
    
    /*
     * Create an image decoder with given file
     * @param - Image file 
     */
    void CreateImageDecoderL(const TDesC& aImageFile);

	static TInt TimerCallbackL(TAny* aThis);
    
    /*
     * ProcessTimerEventL
     */
    void ProcessTimerEventL();

    /*
	 * ProcessImageL
	 */
	void ProcessImageL();
	
	void SetTexture(TInt aTextureId=KErrNotFound);

private:

    TSize iOrigImageDimensions;
    TInt iFrameCount;

    CFbsBitmap*         iDecodedBitmap[KGlxMaxFrameCount];             //Decoded bitmap of the focussed image
    CFbsBitmap*         iDecodedMask[KGlxMaxFrameCount];
    
    //ICL
    CGlxDRMgifDecoderAO*  iGlxDecoderAO;            // Internal Image decoder AO              
    CImageDecoder*      iImageDecoder;              // Image Decoder
    TInt                iAnimCount;                 // animation count
    RFs                 iFsSession;                 // RFs
    
    CPeriodic* iTimer;
    CPeriodic* iAnimationTimer;
	TBool iBitmapReady;
#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif   
    
    const CGlxBinding* iBinding;
    const TGlxMedia* iMedia;
    Alf::IMulModel* iModel;
    TInt iItemIndex;
    CGlxUiUtility* iUiUtility ;
    TBool iAnimateFlag;
    TBool iTransparencyPossible;
    TFrameInfo iFrameInfo;
    TBool iFrameShift;
    };
#endif /* GLXDRMGIFTEXTURECREATOR_H_ */
