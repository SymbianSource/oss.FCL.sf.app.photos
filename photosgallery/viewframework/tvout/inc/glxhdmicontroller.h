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
    IMPORT_C static CGlxHdmiController* NewL(const TDesC& aImageFile = KNullDesC );

    /*
     * Destructor
     */
    IMPORT_C ~CGlxHdmiController();
    
    /*
     * Update Image
     * @param1 - Image file path
     * @param2 - Image dimensions
     * @param3 - frame count
     */
    IMPORT_C void SetImageL(const TDesC& aImageFile,TSize aImageDimensions, 
            TInt aFrameCount = 1, TBool aStore = ETrue);

    /*
     * To Determine it is a video and not image 
     */
    IMPORT_C void IsVideo();
    
    /*
     * Activating zoom in posting mode 
     */
    IMPORT_C void ActivateZoom();
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

private:// From MGlxTvObserver
    virtual void HandleTvStatusChangedL ( TTvChangeType aChangeType );

private:
    /*
     * Constructor
     */
    CGlxHdmiController(const TDesC& aImageFile);
    
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
     * @param1 - Image file
     * @param2 - Image dimensions
     * @param3 - framecount
     */
    void CreateSurfaceUpdaterL(const TDesC& aImageFile, TSize aImageDimensions, 
            TInt aFrameCount);
    
    /*
     * To Destroy the surface updater if present
     */
    void DestroySurfaceUpdater();
    
    /*
     * Detroy the container 
     */
    void DestroyContainer();

    /*
     * 
     */
    void StoreImageInfoL(const TDesC& aImageFile,
            TSize aImageDimensions, TInt aFrameCount);

private:
    const TDesC& iImagePath;                        // Image path
    HBufC*  iStoredImagePath;
    TSize   iImageDimensions; 
    TInt    iFrameCount;

    CGlxHdmiContainer*      iHdmiContainer;
    CGlxHdmiSurfaceUpdater* iSurfaceUpdater;
    CGlxTv*  iGlxTvOut;
    TBool iImageSupported;
    };

#endif /* GLXHDMICONTROLLER_H_ */
