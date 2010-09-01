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
* Description:    Implementation of download Grid view plugin 
*
*/





/**
 * @internal reviewed 26/06/2007 by Rowland Cook
 */

// INCLUDE FILES

#include "glxdownloadsgridviewplugin.h"

#include "glxgridviewplugin.hrh"
#include <avkon.hrh>
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>

#include <AknUtils.h>
#include <bautils.h>
#include <glxcollectiongeneraldefs.h>
#include <glxcommandhandlerdownload.h>
#include <glxcommandhandlerhelp.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <glxmedia.h>
#include <glxmedialist.h>
#include <glxmediastaticitemdefs.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxsettingsmodel.h>			              // For Cenrep Keys
#include <glxgridview.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h>
#include <StringLoader.h>

#include <glxgridviewdata.rsg>
#include <glxicons.mbg> // icons 
#include <glxdownloadtext.rsg> 

_LIT(KGlxGridResource,"glxgridviewdata.rsc");

const TInt KStaticItemId1 = 12345;
const TInt KStaticItemId2 = 12346;


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxDownloadsGridViewPlugin::CGlxDownloadsGridViewPlugin()

    {
    iResourceIds.iMenuId = R_GLX_GRID_MENU;
    iResourceIds.iViewId = R_GRID_VIEW;
    iResourceIds.iCbaId = R_GRID_SOFTKEYS;
    iResourceIds.iEmptyViewTextId = R_GRID_EMPTY_VIEW_TEXT;
    iResourceIds.iOkOptionsMenuId = R_GRID_OK_MENUBAR;
    iViewUID = KGlxDownloadsGridViewImplementationId;
    
            
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxDownloadsGridViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxDownloadsGridViewPlugin* CGlxDownloadsGridViewPlugin::NewL()
    {
    CGlxDownloadsGridViewPlugin* self = 
        new ( ELeave ) CGlxDownloadsGridViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxDownloadsGridViewPlugin::~CGlxDownloadsGridViewPlugin()
    {
    if ( iResOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResOffset );
        }
    }


   
void CGlxDownloadsGridViewPlugin::AddCommandHandlersL()
    {
    TRACER("CGlxDownloadsGridViewPlugin::AddCommandHandlersL()");
    CGlxGridViewPluginBase::AddCommandHandlersL();

	TInt isDownloadLink = CGlxSettingsModel::ValueL<TInt>( KGlxDownloadLink );
	
	if ( isDownloadLink ) 
	    {
        GLX_LOG_INFO("Adding CGlxCommandHandlerDownload");
        iGridView->AddCommandHandlerL(CGlxCommandHandlerDownload::NewL());
	    }
    
    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext = LGAL_HLP_DOWNLOADS_GRID;
    helpInfo.iViewContext = LGAL_HLP_DOWNLOADS_FULLSCREEN;
    
    iGridView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));
    }



// -----------------------------------------------------------------------------
// GetResourceFilenameL
// -----------------------------------------------------------------------------
//
void CGlxDownloadsGridViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TParse parse;
    parse.Set(KGlxGridResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }


// -----------------------------------------------------------------------------
// CreateMediaListL
// -----------------------------------------------------------------------------
//
MGlxMediaList& CGlxDownloadsGridViewPlugin::CreateMediaListL(
                              MMPXCollectionUtility& aCollectionUtility) const 
    {
    TRACER("CGlxDownloadsGridViewPlugin::CreateMediaListL()");
    MGlxMediaList* mediaList = NULL;
	
    // Create media list
    CMPXCollectionPath* path = aCollectionUtility.Collection().PathL();
    CleanupStack::PushL(path);
    
    // need to check the level for EUnit test case, if there are no more level in the path
    // there is no point in going back. 
    if ( path->Levels() )
        {
        path->Back();
        }
    mediaList = MGlxMediaList::InstanceL(*path);
    CleanupClosePushL(*mediaList);
    TInt isDownloadLink = CGlxSettingsModel::ValueL<TInt>( KGlxDownloadLink );
	if (!isDownloadLink) 
	    {
        CleanupStack::Pop(mediaList);
        CleanupStack::PopAndDestroy(path);
        return *mediaList;
	    }
  
    // add a static items
    TGlxMediaId testId(KStaticItemId1);
    CGlxMedia* newMedia1 = new (ELeave) CGlxMedia(testId);
    CleanupStack::PushL(newMedia1);

    TGlxMediaId testId2(KStaticItemId2);
    CGlxMedia* newMedia2 = new (ELeave) CGlxMedia(testId2);
    CleanupStack::PushL(newMedia2);
    TFileName reslocName = CGlxSettingsModel::ValueL<TFileName>( KGlxDownloadLinkText );
    TParse parse;
    parse.Set(reslocName, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);
    if(!iResOffset)
        {
        iResOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);
        } 
    HBufC* title = 
              StringLoader::LoadLC(R_GRID_VIEW_TEXT_DOWNLOAD_HEADER);
    newMedia1->SetTextValueL(KMPXMediaGeneralTitle, *title);
    newMedia2->SetTextValueL(KMPXMediaGeneralTitle, *title);
    CleanupStack::PopAndDestroy(title);
    
    HBufC* subtitle = 
              StringLoader::LoadLC(R_GRID_VIEW_TEXT_DOWNLOAD_IMAGES);
    newMedia1->SetTextValueL(KGlxMediaCollectionPluginSpecificSubTitle, 
                             *subtitle);
    CleanupStack::PopAndDestroy(subtitle);
      
    HBufC* subtitle2 = 
              StringLoader::LoadLC(R_GRID_VIEW_TEXT_DOWNLOAD_VIDEOS);
    newMedia2->SetTextValueL(KGlxMediaCollectionPluginSpecificSubTitle, 
                             *subtitle2);
    CleanupStack::PopAndDestroy(subtitle2);
    
    // add static attribute containing the command id of the command
    // handled by the command handler
    newMedia1->SetTObjectValueL<TInt>(
            TMPXAttribute(KGlxMediaIdStaticItem, 
                          KGlxMediaStaticItemCommand),
            EGlxCmdDownloadImages);
    
    newMedia2->SetTObjectValueL<TInt>(
            TMPXAttribute(KGlxMediaIdStaticItem, 
                          KGlxMediaStaticItemCommand),
            EGlxCmdDownloadVideos);
    
    const TFileName resName = CGlxSettingsModel::ValueL<TFileName>( KGlxDownloadLinkIcon );
    parse.Set(resName, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName iconFile(KNullDesC);
    iconFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(iconFile);  
    
    TIconInfo info;
    //add icons to the static media items
    info.bitmapId = EMbmGlxiconsQgn_prop_download_images;
    info.maskId = EMbmGlxiconsQgn_prop_download_images_mask;
    info.bmpfile = iconFile;
    newMedia1->SetTObjectValueL(KMPXMediaGeneralIcon, info);
    
    info.bitmapId = EMbmGlxiconsQgn_prop_download_video;
    info.maskId = EMbmGlxiconsQgn_prop_download_video_mask;
    newMedia2->SetTObjectValueL(KMPXMediaGeneralIcon, info);
    
    // Add the items to the list
    mediaList->AddStaticItemL(newMedia2, NGlxListDefs::EInsertFirst);
    mediaList->AddStaticItemL(newMedia1,  NGlxListDefs::EInsertFirst);
    
    //set the focus on 1st static item
    mediaList->SetFocusL(NGlxListDefs::EAbsolute,0);
     
    CleanupStack::Pop(newMedia2);
    CleanupStack::Pop(newMedia1);
       
    CleanupStack::Pop(mediaList); 
    CleanupStack::PopAndDestroy(path); 
    
    return *mediaList;
    }
//  End of File
