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
* Description:    Saves the thumbnail
*
*/



#ifndef C_GLXTHUMBNAILSAVER_H__
#define C_GLXTHUMBNAILSAVER_H__

#include<e32def.h>
#include<e32base.h>
#include<f32file.h>

class CFbsBitmap;
class CGlxDefaultThumbnailContext;
class CImageEncoder;
class CFbsBitGc;


  

//  CLASS DEFINITION

/**
 * Observer for the thumbnail save activity
 */
class  MGlxThumbnailSaveComplete
    {
public:     
    /**
    * Informs the completion of file save
    * @param a aPath contains the file path
    * @return none
    */    
    virtual void HandleFileSaveCompleteL(const TDesC& aPath) = 0;
    };
    
//  CLASS DEFINITION
/**
 * Thumbnail saver class 
 */
NONSHARABLE_CLASS( CGlxThumbnailSaver ):public CActive
    {

public:

    /**
    * Two-phased constructor.
    */
    static CGlxThumbnailSaver* NewL(MGlxThumbnailSaveComplete* aObserver);

    /**
    * Destructor.
    */
    ~CGlxThumbnailSaver();

    /**
    * Creates the video overlay icon 
    * @return none
    */
    void CreateVideoIconL(CFbsBitmap* aThumbnail);
    
    /**
    * Creates the default video overlay icon
    * @return none
    */
    void CreateDefaultVideoIconL();
    
    /**
    * Creates the default image overlay icon
    * @return none
    */
    void CreateDefaultImageIconL();

private:

    /**
    * Constructor 
    * @return none
    */
    CGlxThumbnailSaver(MGlxThumbnailSaveComplete* aObserver);

    /**
    * Symbian 2-phase constructor 
    * @return none
    */
    void ConstructL(); 

    /**
    * Creates the video strip icon 
    * @return none
    */    
    void CreateVideoStripIconL();

    /**
    * Active objects request completion event
    */
    void RunL();

    /**
    * Cancellation of a request
    */ 	    
    void DoCancel();
    
private:   

    //File server session instance
    RFs iFs;

    //videostrip ,This will be overlayed on the video thumbnail
    CFbsBitmap* iVideoStrip;

    //video strip mask ,used to create the video strip icon from the mif file
    CFbsBitmap* iVideoStripMask;

    //image encoder,used for the video overlay icon saving
    CImageEncoder* iImageEncoder;

    //temporay bitmap
    //used to blit the overlay icon and the thumnail of the video
    CFbsBitmap* iTempBitmap;

    //  Observer which informs the renderer class  about the completion of the  thumbnail save
    MGlxThumbnailSaveComplete* iObserver;
    };
#endif    
