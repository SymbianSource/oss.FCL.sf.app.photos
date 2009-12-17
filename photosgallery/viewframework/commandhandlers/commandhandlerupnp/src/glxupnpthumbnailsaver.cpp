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




#include "glxupnpthumbnailsaver.h"

#include <AknIconUtils.h>                //AknIconUtils
#include <AknUtils.h>
#include <bautils.h>                     //BaflUtils
#include <data_caging_path_literals.hrh>
#include <imageconversion.h>             //CImageEncoder
#include <mglxactivemedialistresolver.h> //MGlxActiveMediaListResolver
#include <mglxmedialist.h>               //MGlxMediaList

#include <glxresourceutilities.h>        //CGlxResourceUtilities
#include <glxicons.mbg>                  //mgallery icon
#include <glxtracer.h>                      //Logs
#include <glxthumbnail.h>

// CONSTANTS
const TInt KVideoIconThumbnailWidth = 640;
const TInt KVideoIconThumbnailHeight = 480;
const TInt KIconWeight=100;
const TInt KIconHeight=80;

_LIT(KFilepath ,"C:\\Data\\upnp\\VideoThumb.jpg");    //jpeg                                   	     
_LIT(KFolder,"C:\\Data\\upnp\\");
_LIT(KIconsResName, "glxicons.mif");                  //For icons
_LIT8(KGlxMimeJpeg, "image/jpeg");

//-----------------------------------------------------------------------------------------
// Two phased Constructor
//-----------------------------------------------------------------------------------------
CGlxThumbnailSaver* CGlxThumbnailSaver:: NewL(MGlxThumbnailSaveComplete* aObserver)   
    {
    TRACER("CGlxThumbnailSaver::NewL()");  
    
    CGlxThumbnailSaver* self = new (ELeave) CGlxThumbnailSaver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
//-----------------------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------------------

CGlxThumbnailSaver::CGlxThumbnailSaver (MGlxThumbnailSaveComplete* aObserver): CActive(EPriorityStandard)  
    {
    TRACER("CGlxThumbnailSaver::CGlxThumbnailSaver ");
    iObserver = aObserver;
    }
    
//-----------------------------------------------------------------------------------------
//	2nd phase Constructor
//-----------------------------------------------------------------------------------------    
 
void CGlxThumbnailSaver::ConstructL()   
    {
     TRACER("CGlxThumbnailSaver::ConstructL()");  
    
    User::LeaveIfError (iFs.Connect());
    CActiveScheduler::Add(this);
    if ( !BaflUtils::FolderExists( iFs, KFolder ) )
        { 
        User::LeaveIfError( iFs.MkDir( KFolder ) ); //create the folder upnp
        // set the folder as hidden to prevent MDS from harvesting its contents
        User::LeaveIfError( iFs.SetAtt( KFolder, KEntryAttHidden,
                KEntryAttNormal ) );
        }
    }
    
//-----------------------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------------------

CGlxThumbnailSaver::~CGlxThumbnailSaver()
    {
    TRACER("CGlxThumbnailSaver::~CGlxThumbnailSaver()");
    
    Cancel();

    iFs.Delete( KFilepath ); // return value ignored
    iFs.RmDir( KFolder ); // return value ignored
    
    delete iImageEncoder;

    delete iTempBitmap;  

    delete iVideoStrip;

    delete iVideoStripMask; 

    iObserver = NULL;

    iFs.Close();       
    } 
    
    
//-----------------------------------------------------------------------------------------
// Creates the video strip icon
//-----------------------------------------------------------------------------------------

void CGlxThumbnailSaver::CreateVideoStripIconL()
    {
    TRACER("CGlxThumbnailSaver::CreateVideoStripIconL()");
    
    TFileName mifFile(KDC_APP_BITMAP_DIR);
    mifFile.Append(KIconsResName);
    CGlxResourceUtilities::GetResourceFilenameL(mifFile);

    AknIconUtils::CreateIconL(iVideoStrip, iVideoStripMask, mifFile, 
    EMbmGlxiconsQgn_indi_media_thumbnail,EMbmGlxiconsQgn_indi_media_thumbnail_mask);
    AknIconUtils::SetSize(iVideoStrip, TSize(KIconWeight,KIconHeight),EAspectRatioNotPreserved);
    }
 
//-----------------------------------------------------------------------------------------
// Creates the default video icon ,this default icon will be sent in case, the 
// thumbnail conversion fails
//-----------------------------------------------------------------------------------------
void CGlxThumbnailSaver::CreateDefaultVideoIconL()
    { 
    TRACER("CGlxThumbnailSaver::CreateDefaultVideoIconL");
    //create a temporary bitmap
    if(!iVideoStrip)
        {
        CreateVideoStripIconL();
        }

    CFbsBitmap* defaultBitmap=NULL;
    CFbsBitmap* defaultBitmapMask= NULL;

    TFileName mifFile(KDC_APP_BITMAP_DIR);
    mifFile.Append(KIconsResName);
    User::LeaveIfError(CompleteWithAppPath(mifFile)); 

    //creating default video icon 	 	
    AknIconUtils::CreateIconLC(defaultBitmap, defaultBitmapMask, mifFile,
    EMbmGlxiconsQgn_prop_image_notcreated, EMbmGlxiconsQgn_prop_image_notcreated_mask);
    AknIconUtils::SetSize(defaultBitmap, TSize(KVideoIconThumbnailWidth,KVideoIconThumbnailHeight),EAspectRatioNotPreserved);
    AknIconUtils::SetSize(defaultBitmapMask, TSize(KVideoIconThumbnailWidth,KVideoIconThumbnailHeight),EAspectRatioNotPreserved);

    //creates a temp bitmap
    delete iTempBitmap;
    iTempBitmap=NULL;

    iTempBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError(iTempBitmap->Create(TSize(KVideoIconThumbnailWidth,KVideoIconThumbnailHeight), KGlxThumbnailDisplayMode));

    //create a graphic device
    CFbsBitmapDevice* graphicsDevice = CFbsBitmapDevice::NewL(iTempBitmap);
    CleanupStack::PushL(graphicsDevice);

    //bitmap context
    CFbsBitGc*  bitmapContext=NULL;
    User::LeaveIfError(graphicsDevice->CreateContext(bitmapContext));
    bitmapContext->Activate(graphicsDevice);
    CleanupStack::PushL(bitmapContext);
	TRect rectSize;
	rectSize.SetWidth(KVideoIconThumbnailWidth);
	rectSize.SetHeight(KVideoIconThumbnailHeight);
	TPoint centerPosition = rectSize.Center();
	centerPosition.iX  = centerPosition.iX - (KIconWeight/2); 
	centerPosition.iY  = centerPosition.iY - (KIconHeight/2);
    //blit the thumbnail in to the temporary bitmap
    bitmapContext->BitBlt(TPoint(0,0),defaultBitmap);

    //blit the overlay icon
    bitmapContext->BitBltMasked(centerPosition,iVideoStrip,TRect(TPoint(0,0), iVideoStrip->SizeInPixels()), iVideoStripMask,EFalse);

    //create the encoder -jpeg       
    delete iImageEncoder;
    iImageEncoder=NULL;

    iImageEncoder = CImageEncoder::FileNewL(iFs,KFilepath,KGlxMimeJpeg); 

    //convert to jpeg
    // Fix for ESAU-7CVF8R: "UPNP:MC photos Crashes When user focus the DRM video on the rendering device"
    // Ensure we don't try to activate ourself when already active
    if ( IsActive() )
        {
        Cancel();
        }
    iImageEncoder->Convert(&iStatus ,*iTempBitmap);
    SetActive();
    
    CleanupStack::PopAndDestroy(bitmapContext);
    CleanupStack::PopAndDestroy(graphicsDevice);
    CleanupStack::PopAndDestroy(defaultBitmapMask);
    CleanupStack::PopAndDestroy(defaultBitmap);
    }

//---------------------------------------------------------------------------------	
//	creates the  video overlayicon     
//---------------------------------------------------------------------------------	

void CGlxThumbnailSaver::CreateVideoIconL(CFbsBitmap* aThumbnail)
    {
    TRACER("CGlxThumbnailSaver::CreateVideoIconL");

    //create a temperory bitmap
    if(!iVideoStrip)
        {
        CreateVideoStripIconL();
        }

    delete iTempBitmap;
    iTempBitmap=NULL;
    iTempBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError(iTempBitmap->Create(TSize(KVideoIconThumbnailWidth,KVideoIconThumbnailHeight), KGlxThumbnailDisplayMode));

    //create a graphic device
    CFbsBitmapDevice* graphicsDevice = CFbsBitmapDevice::NewL(iTempBitmap);
    CleanupStack::PushL(graphicsDevice);

    //bitmap context
    CFbsBitGc*  bitmapContext=NULL;
    User::LeaveIfError(graphicsDevice->CreateContext(bitmapContext));
    bitmapContext->Activate(graphicsDevice);
    CleanupStack::PushL(bitmapContext);
	TRect rectSize;
	rectSize.SetWidth(KVideoIconThumbnailWidth);
	rectSize.SetHeight(KVideoIconThumbnailHeight);
	TPoint centerPosition = rectSize.Center();
	centerPosition.iX  = centerPosition.iX - (KIconWeight/2); 
	centerPosition.iY  = centerPosition.iY - (KIconHeight/2);
		
    //blit the thumbnail in to the temporary bitmap
    bitmapContext->BitBlt(TPoint(0,0),aThumbnail);

    //blit the overlay icon
     bitmapContext->BitBltMasked(centerPosition,iVideoStrip,TRect(TPoint(0,0), iVideoStrip->SizeInPixels()), iVideoStripMask,EFalse);

    //create the encoder
    delete iImageEncoder;
    iImageEncoder= NULL;

    iImageEncoder = CImageEncoder::FileNewL(iFs,KFilepath,KGlxMimeJpeg);
    iImageEncoder->Convert(&iStatus ,*iTempBitmap);

    SetActive();

    CleanupStack::PopAndDestroy(bitmapContext);
    CleanupStack::PopAndDestroy(graphicsDevice);   
   }
   
//-----------------------------------------------------------------------------------------
// void CGlxThumbnailSaver::CreateDefaultImageIconL()
//-----------------------------------------------------------------------------------------
   void CGlxThumbnailSaver::CreateDefaultImageIconL()
   {
   	    TRACER("CGlxThumbnailSaver::CreateDefaultVideoIconL");
    //create a temporary bitmap
    CFbsBitmap* defaultBitmap=NULL;
    CFbsBitmap* defaultBitmapMask= NULL;

    TFileName mifFile(KDC_APP_BITMAP_DIR);
    mifFile.Append(KIconsResName);
    User::LeaveIfError(CompleteWithAppPath(mifFile)); 

    //creating default video icon 	 	
    AknIconUtils::CreateIconLC(defaultBitmap, defaultBitmapMask, mifFile,
    EMbmGlxiconsQgn_prop_image_notcreated, EMbmGlxiconsQgn_prop_image_notcreated_mask);
    AknIconUtils::SetSize(defaultBitmap, TSize(KVideoIconThumbnailWidth,KVideoIconThumbnailHeight),EAspectRatioNotPreserved);
    AknIconUtils::SetSize(defaultBitmapMask, TSize(KVideoIconThumbnailWidth,KVideoIconThumbnailHeight),EAspectRatioNotPreserved);

    //creates a temp bitmap
    delete iTempBitmap;
    iTempBitmap=NULL;

    iTempBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError(iTempBitmap->Create(TSize(KVideoIconThumbnailWidth,KVideoIconThumbnailHeight), KGlxThumbnailDisplayMode));

    //create a graphic device
    CFbsBitmapDevice* graphicsDevice = CFbsBitmapDevice::NewL(iTempBitmap);
    CleanupStack::PushL(graphicsDevice);

    //bitmap context
    CFbsBitGc*  bitmapContext=NULL;
    User::LeaveIfError(graphicsDevice->CreateContext(bitmapContext));
    bitmapContext->Activate(graphicsDevice);
    CleanupStack::PushL(bitmapContext);

    //blit the thumbnail in to the temporary bitmap
    bitmapContext->BitBlt(TPoint(0,0),defaultBitmap);

    //create the encoder -jpeg       
    delete iImageEncoder;
    iImageEncoder=NULL;

    iImageEncoder = CImageEncoder::FileNewL(iFs,KFilepath,KGlxMimeJpeg); 

    if ( IsActive() )
        {
        Cancel();
        }
    iImageEncoder->Convert(&iStatus ,*iTempBitmap);
    SetActive();
    
    CleanupStack::PopAndDestroy(bitmapContext);
    CleanupStack::PopAndDestroy(graphicsDevice);
    CleanupStack::PopAndDestroy(defaultBitmapMask);
    CleanupStack::PopAndDestroy(defaultBitmap);
   }
//-----------------------------------------------------------------------------------------
// CActive::RunL
//-----------------------------------------------------------------------------------------

void  CGlxThumbnailSaver::RunL()
    {
    TRACER("CGlxUpnpRenderer::RunL");

    if (iStatus == KErrNone)		 	      
        {
        TRACER("CGlxUpnpRenderer::RunL::KErrNone");
        iObserver->HandleFileSaveCompleteL(KFilepath);
        }
    }
    
//-----------------------------------------------------------------------------------------
// CActive::DoCancel()
//-----------------------------------------------------------------------------------------

void  CGlxThumbnailSaver::DoCancel()
    {
    TRACER("CGlxUpnpRenderer::DoCancel");
    
    iImageEncoder->Cancel();
    }

