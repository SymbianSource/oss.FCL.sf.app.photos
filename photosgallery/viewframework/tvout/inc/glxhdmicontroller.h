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
* Description:    Handles & propogates UI state change notifications.
*
*/

#ifndef GLXHDMICONTROLLER_H_
#define GLXHDMICONTROLLER_H_

// Internal includes
#include <mglxtvobserver.h>     // for inteface MGlxTvObserver
#include <mglxhdmidecoderobserver.h> //for MGlxHDMIDecoderObserver

class CGlxHdmiContainer;
class CGlxHdmiSurfaceUpdater;
class CGlxTv;

/*
 * This class will be called from FS and Slideshow for its requirements 
 */
class CGlxHdmiController : public CBase,
                            public MGlxTvObserver              // for TV Out
    {
public:
    /*
     * NewLC 
     * @param1 - Image file path default to NULL
     */
    IMPORT_C static CGlxHdmiController* NewL(
            MGlxHDMIDecoderObserver& aDecoderObserver);

    /*
     * Destructor
     */
    IMPORT_C ~CGlxHdmiController();
    
    /*
     * Sets the Image to be displayed
     * @aImageFile - Image file path
     * @aNextImageFile - Next image file path
     * @aFsBitmap - Bitmap to be displayed
     * @aStore - Image info should be stored or not
     */
    IMPORT_C void SetImageL(const TDesC& aImageFile, 
            const TDesC& aNextImageFile = KNullDesC, 
            CFbsBitmap* aFsBitmap = NULL, TBool aStore = ETrue);

    /*
     * To intimate that the item is not supported.  
     */
    IMPORT_C void ItemNotSupported();
    
    /*
     * Activating zoom in posting mode 
     */
    IMPORT_C void ActivateZoom(TBool aAutoZoomOut);
    /*
     * Deactivating zoom in posting mode 
     */
    IMPORT_C void DeactivateZoom();
    
    /*
     * ShiftToCloningMode
     */
    IMPORT_C void ShiftToCloningMode();
    
    /*
     * ShiftToPostingMode
     */
    IMPORT_C void ShiftToPostingMode();

    /*
     * Tells if HDMi is Connected.
     */
    IMPORT_C TBool IsHDMIConnected();

private:// From MGlxTvObserver
    void HandleTvStatusChangedL ( TTvChangeType aChangeType );

private:
    /*
     * Constructor
     */
    CGlxHdmiController(MGlxHDMIDecoderObserver& aDecoderObserver);
    
    /*
     * ConstructL 
     */
    void ConstructL();
    
    /*
     * Create the Hdmi Container 
     */
    void CreateHdmiContainerL();
    
    /*
     * Create surface updater and update background surface 
     * @aImageFile - Image file     
     * @aNextImageFile - Next image file     
     */
    void CreateSurfaceUpdaterL(const TDesC& aImageFile,
            const TDesC& aNextImageFile = KNullDesC);
    
    /*
     * To Destroy the surface updater if present
     */
    void DestroySurfaceUpdater();
    
    /*
     * Detroy the container 
     */
    void DestroyContainer();

    /*
     * Stores the Image File name
     * @aImageFile - Image file path
     * @aNextImageFile - Next image file path
     * @aFsBitmap - Bitmap to be displayed
     */
    void StoreImageInfoL(const TDesC& aImageFile,
            const TDesC& aNextImageFile, CFbsBitmap* aFsBitmap);

private:
    CFbsBitmap* iFsBitmap;
    HBufC*  iStoredImagePath;
    HBufC*  iStoredNextImagePath;
    CGlxHdmiContainer*      iHdmiContainer;
    CGlxHdmiSurfaceUpdater* iSurfaceUpdater;
    CGlxTv*  iGlxTvOut;
    TBool   iIsImageSupported;          // Flag to see if Image is supported
    TBool   iIsPhotosInForeground;          // Flag for determine if Photos is in foreground
	MGlxHDMIDecoderObserver& iDecoderObserver;
    };

#endif /* GLXHDMICONTROLLER_H_ */