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
* Description:    Handles & propogates UI state change notifications.
*
*/

#ifndef GLXHDMISURFACEUPDATER_H_
#define GLXHDMISURFACEUPDATER_H_

#include <w32std.h>
#include <fbs.h>

// GCE Surface
#include "graphics/surfacemanager.h"
#include "graphics/surface.h"
#include "graphics/surfaceupdateclient.h"
#include <graphics/surfaceconfiguration.h>

// forward decleration
class CGlxActiveCallBack;
class CGlxHdmiDecoderAO;
class CImageDecoder;
class CAlfCompositionSource;
class TSurfaceConfiguration;

class MGlxGenCallback
    {
public:
    virtual void DoGenCallback() = 0;
    };

class CGlxHdmiSurfaceUpdater: public CBase
    {
public:
    /*
     * NewL
     */
    static CGlxHdmiSurfaceUpdater* NewL(RWindow* aWindow, const TDesC& aImageFile, 
            CFbsBitmap* aFsBitmap, MGlxGenCallback* aCallBack,TBool aEffectsOn);

    /*
     * Destructor
     */
    ~CGlxHdmiSurfaceUpdater();

public:
    /*
     * This is to cancel the active object from decoding 
     */
    void HandleRunL(TRequestStatus& aStatus);

    /*
     * This updates the new image.
     */
    void UpdateNewImageL(const TDesC& aImageFile,CFbsBitmap* aFsBitmap);

    /*
     * Activate Zoom 
     */
    void ActivateZoom(TBool aAutoZoomOut);
    /*
     * Deactivate Zoom 
     */
    void DeactivateZoom();

    /*
	ShiftToCloningMOde
     */
    void ShiftToCloningMode();

    /*
	ShiftToPostingMode
     */
    void ShiftToPostingMode();

    /*
     * Fadeing of the Surface
     * @param1 ETrue - FadeIn ( as in gaining brightness )
     *         EFalse - FadeOut ( as in loosing brightness ) 
     */
    void FadeTheSurface(TBool aFadeInOut);

private:
    /*
     * Constructor 
     */
    CGlxHdmiSurfaceUpdater(RWindow* aWindow, MGlxGenCallback* aCallBack,TBool aEffectsOn);

    /*
     * ConstructL()
     */
    void ConstructL(CFbsBitmap* aFsBitmap,const TDesC& aImageFile);   

    /*
     * Create a New surface with given size
     */
    void CreateSurfaceL();
    /*
     * @param1 size 
     */
    void MapSurfaceL();

    /*
     * This fundtion wont be called , could be used if double buffering is planned in future
     * to start the second decoder AO and update the surface with a new session. 
     */
    static TInt SurfBuffer0Ready(TAny* aObject);    
    
    /*
     * Call a refresh on the screen  
     */
    void Refresh();

    /*
     * Dump the buffer on to the surface stride 
     */
    void SwapBuffers();

    /*
     * Release contents 
     */
    void ReleaseContent();

    /*
     * Create bitmap  
     */
    void CreateBitmapL();

    /*
     * Create an image decoder with given file
     * @param1 - Image file 
     */
    void CreateImageDecoderL();

    /*
     * Creating all enablers for HDMI
     * @param1 if creating a surface is required, 
     * by default it is not required
     */
    void CreateHdmiL(TBool aCreateSurface = ETrue);

    /*
     * Zoom in our out depending on parameter 
     */
    void Zoom(TBool aZoom);

    /*
     * This if for zoom timer timeout
     */
    static TInt TimeOut(TAny* aSelf);
    
    /*
     * Animation time out timer
     */
    static TInt AnimationTimeOut(TAny* aSelf);
    
    /*
     * ModifySurface positions of the surface to be displayed on screen
     */
    void ModifySurfacePostion();

    /*
     * Process the image for TV 
     */
    void ProcessTvImage();

    /*
     * Shows the FS thumbnail first before showing 
     * Decoded HD image 
     */
    void ShowFsThumbnailL();

    /*
     * Recalculate the size for png/bmp as decoder fails to 
     * decode for desired size 
     */
    TSize ReCalculateSizeL();

    /*
     * If the image format is non jpeg, then we need to calculate as per
     * reduction factor and reduced size as what the decoder is going to return us
     * This function returns if that needs to be done. 
     */
    TBool DoesMimeTypeNeedsRecalculateL();

    /*
     * If the image format is non jpeg, then we need to scale the bitmap after it is
     * decoded, as the return value would not fit the screen 
     */
    void ScaleDecodedBitmapL(TInt aBitmapIndex);

    /*
     * InitiateHDMI
     */
    void InitiateHdmiL(CFbsBitmap* aFsBitmap,const TDesC& aImageFile);

    /*
     * Animate untill loop is complete 
     */
    void Animate();

private:
    RWindow*            iWindow;                    // window object
    CFbsBitmap*         iFsBitmap;                  // FS bitmap
    MGlxGenCallback*    iCallBack;                  // callback to the HdmiContainer window
    HBufC*              iImagePath;                 // To store the image uri path
    TBool               iEffectsOn;

    TBool       iShwFsThumbnail;                    // If the Fs thumbnail is to be shown before decoding HD images
    TBool       iIsNonJpeg;                         // If the item is non jpeg
    TBool       iFadeIn;                            // If FadeIn or Out for only SLideshow animation
    
    // GCE Surface
    RSurfaceUpdateSession iSurfUpdateSession;
    TSurfaceId              iSurfId;                // TSurfaceId                             
    RSurfaceManager*        iSurfManager;           // RSurfaceManager
    RChunk*                 iSurfChunk;             // RChunk
    TInt                    iSurfaceStride;         // surface stride
    TSurfaceConfiguration   iConfig;                // surface configuration for zoom

    CFbsBitmap*         iDecodedBitmap[3];          // Array of Decoded bitmaps of the focussed image
                                                    // 1 - contains FS/Grid Thumbnail, 2- HD image, 
                                                    // (optional)3-Scaled png/bmp images

    void*               iSurfBuffer;               // Surface buffer
    CGlxActiveCallBack* iSurfBufferAO;             // Surface buffer AO 

    CAlfCompositionSource* ialfCompositionSurface;

    //ICL
    CGlxHdmiDecoderAO*  iGlxDecoderAO;              // Internal Image decoder AO              
    CImageDecoder*      iImageDecoder;              // Image Decoder
    RFs                 iFsSession;                 // RFs

    TPoint      iLeftCornerForZoom;                 //  
    CPeriodic*  iTimer;                             // Timer for Zoom 
    CPeriodic*  iAnimTimer;                         // Timer for Animation
    TInt        iAnimCounter;

    // Various objects to store sizes and count
    TSize       iTvScreenSize;                      // to store the Tv screen size
    TSize       iTargetBitmapSize;                  // To Store the target bitmap size to display(as per Aspect Ratio)
    
    // Various flags to store values
    TBool       iZoom;                              // Is zoomed
    TBool       iBitmapReady;                       // If the bitmap is decoded and ready
    TBool       iAutoZoomOut;                       // If the UI has asked for auto zoomout
    TBool       iSurfSessionConnected;              // If surface session is connected
	TBool       iShiftToCloning;
#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif   
	// The bitmap index in the array of bitmaps
    enum {
        EFSBitmapIndex = 0,
        EJpgDecodedBitmapIndex,
		ENonJpgDecodedBitmapIndex
        };
    };
#endif /* GLXHDMISURFACEUPDATER_H_ */
