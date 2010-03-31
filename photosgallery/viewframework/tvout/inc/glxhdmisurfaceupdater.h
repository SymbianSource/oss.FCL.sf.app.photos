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
class CGlxActiveCallBack;
class CGlxHdmiDecoderAO;
class CImageDecoder;
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
     * 
     */
    static CGlxHdmiSurfaceUpdater* NewL(RWindow* aWindow, const TDesC& aImageFile, 
                                         MGlxGenCallback* aCallBack);
    
    /*
     * destructor
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
    void UpdateNewImageL(const TDesC& aImageFile);
    
    /*
     * Activate Zoom 
     */
    void ActivateZoom(TBool aAutoZoomOut);
    /*
    * Deactivate Zoom 
    */
    void DeactivateZoom();
    /*
     * Zoom in our out depending on parameter 
     */
    void Zoom(TBool aZoom);
	
	/*
	ShiftToCloningMOde
	*/
	void ShiftToCloningMode();
	
	/*
	ShiftToPostingMode
	*/
	void ShiftToPostingMode();

private:
    /*
     * Ctor 
     */
    CGlxHdmiSurfaceUpdater(RWindow* aWindow, const TDesC& aImageFile,
                            MGlxGenCallback* aCallBack);
    
    /*
     * ConstructL()
     */
    void ConstructL();   
    
    /*
     * Create a New surface with given size
     */
    void CreateSurfaceL();
    /*
    * @param1 size 
    */
   void MapSurfaceL();

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
    void CreateImageDecoderL(const TDesC& aImageFile);

    /*
     * Creating all enablers for HDMI
     * @param1 if creating a surface is required, 
     * by default it is not required
     */
    void CreateHdmiL(TBool aCreateSurface = ETrue);

    static TInt TimeOut(TAny* aSelf);
	
	/*
	* ModifySurface positions of the surface to be displayed on screen
	*/
	void ModifySurfacePostion();
private:
    RWindow* iWindow;
    const TDesC& iImagePath;
    MGlxGenCallback* iCallBack;

    // GCE Surface
    RSurfaceUpdateSession iSurfUpdateSession;
    TSurfaceId              iSurfId;                // TSurfaceId                             
    RSurfaceManager*        iSurfManager;           // RSurfaceManager
    RChunk*                 iSurfChunk;             // RChunk
    TInt                    iSurfaceStride;         // surface stride
    TSurfaceConfiguration   iConfig;                // surface configuration for zoom
    
    TSize               iZoomRectSz ;
    CFbsBitmap*         iDecodedBitmap;             //Decoded bitmap of the focussed image
    
    void*               iSurfBuffer;               // Surface buffer
    CGlxActiveCallBack* iSurfBufferAO;             // Surface buffer AO 
    
    //ICL
    CGlxHdmiDecoderAO*  iGlxDecoderAO;              // Internal Image decoder AO              
    CImageDecoder*      iImageDecoder;              // Image Decoder
    RFs                 iFsSession;                 // RFs
    
    TPoint iLeftCornerForZoom;
    CPeriodic* iTimer;
    TBool iZoom;
	TBool iBitmapReady;
	TBool iAutoZoomOut;
	TBool iSurfSessionConnected;
#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif   
    
    };
#endif /* GLXHDMISURFACEUPDATER_H_ */
