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
* Description:    Provide Icon File Considering Requirement
*
*/




#include "glxmuliconprovider.h"               // Icon details
#include <avkon.mbg>
#include <e32math.h>                          // A collection of mathematical functions
#include <glxicons.mbg>
#include "glxdrmutility.h"                   // DRM utility class to provide DRM-related functionality 
#include "glxmedia.h"                        // Reference to a media item in the item pool
#include <glxlog.h>                          // Logging
#include <lbsposition.h> // TCoordinate
#include <pathinfo.h> 
#include <driveinfo.h> // Added for DriveInfo class
#include <data_caging_path_literals.hrh>
#include <glxtracer.h>
#include <glxerrormanager.h>
#include <glxthumbnailattributeinfo.h>

_LIT(KGlxIconsFilename, "glxicons.mif");

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxMulIconProvider::ConstructL()
    {
    TRACER("CGlxMulIconProvider::ConstructL");
    iResFileName.Copy(KDC_APP_BITMAP_DIR);
    iResFileName.Append(KGlxIconsFilename);
    }

// ----------------------------------------------------------------------------
// ResourceFileName
// ----------------------------------------------------------------------------
//
const TDesC& CGlxMulIconProvider::ResourceFileName() const
    {
    TRACER("CGlxMulIconProvider::ResourceFileName");
    return iResFileName;
    }

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//   
EXPORT_C CGlxMulLocationIconProvider* CGlxMulLocationIconProvider::NewL( ) 
    {
    TRACER("CGlxMulLocationIconProvider::NewL");
    CGlxMulLocationIconProvider* self = new (ELeave) CGlxMulLocationIconProvider();
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
    }
 
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//   
CGlxMulLocationIconProvider::~CGlxMulLocationIconProvider()
	{
	
	}

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//   
EXPORT_C CGlxMulDriveIconProvider* CGlxMulDriveIconProvider::NewL( )
    {
    TRACER("CGlxMulDriveIconProvider::NewL");
    CGlxMulDriveIconProvider* self = new (ELeave) CGlxMulDriveIconProvider();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//   
CGlxMulDriveIconProvider::~CGlxMulDriveIconProvider()
	{
	
	}

// ----------------------------------------------------------------------------
// IconIdL
// ----------------------------------------------------------------------------
//   
TInt CGlxMulLocationIconProvider::IconIdL( const TGlxMedia& aMedia ) const
    {
    TRACER("CGlxMulLocationIconProvider::IconIdL");
    const CGlxMedia* media = aMedia.Properties();
    if( media->IsSupported( KGlxMediaGeneralLocation ) )
        {
        TCoordinate cordinate = media->ValueTObject<TCoordinate>(KGlxMediaGeneralLocation);	 
        //Check for valid co-ordinates
        if( (!Math::IsNaN(cordinate.Latitude())) && (!Math::IsNaN(cordinate.Longitude())) ) 
            {
            return EMbmGlxiconsQgn_indi_cam4_geotag_on;
            }
      }
     return EMbmAvkonQgn_note_empty;
    }

// ----------------------------------------------------------------------------
// IconIdL
// ----------------------------------------------------------------------------
//   
TInt CGlxMulDriveIconProvider::IconIdL( const TGlxMedia& aMedia ) const
    {

    TRACER("CGlxMulDriveIconProvider::IconIdL");
    // by default set icon for internal mass storage
    const CGlxMedia* media = aMedia.Properties();
    TInt iconId= EMbmGlxiconsQgn_indi_fmgr_ms_add;
    if( media->IsSupported( KMPXMediaGeneralDrive ) )
        {
    const TDesC& drive =  media->ValueText(KMPXMediaGeneralDrive);
    if(0 == drive.CompareF(
    PathInfo::PhoneMemoryRootPath().Left( KMaxDriveName )))
        {
        // in phone memory
        iconId = EMbmGlxiconsQgn_indi_phone_add;
        }
    else 
        {
        RFs fsSession;
        User::LeaveIfError(fsSession.Connect());
        TUint drvStatus( 0 );
        TDriveUnit driveNumber(drive);
        User::LeaveIfError(DriveInfo::GetDriveStatus
        ( fsSession, driveNumber, drvStatus ) );
        // To get the entire status of the Drive as "drvStatus"
        fsSession.Close();
        if(drvStatus & DriveInfo::EDriveRemovable)
        // Do a AND operation to check that the status of the drive is "Removable" or not
            {
            // in mmc memory
            iconId = EMbmGlxiconsQgn_indi_mmc_add;
                }
            }
        }
    return iconId;    
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//   
CGlxMulStaticIconProvider::CGlxMulStaticIconProvider(TInt aResourceIconId)
    {
    TRACER("CGlxMulStaticIconProvider::CGlxMulStaticIconProvider");
    iIconId = aResourceIconId;
    
    }

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//   
EXPORT_C CGlxMulStaticIconProvider* CGlxMulStaticIconProvider::NewL(TInt aResourceIconId )
    {
    TRACER("CGlxMulStaticIconProvider::NewL");
    CGlxMulStaticIconProvider* self = new (ELeave) CGlxMulStaticIconProvider(aResourceIconId);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// IconIdL
// ----------------------------------------------------------------------------
//   
TInt CGlxMulStaticIconProvider::IconIdL(const TGlxMedia& /*aMedia*/ ) const
    {
    TRACER("CGlxMulStaticIconProvider::IconIdL");
    return iIconId;
    }

//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//  
CGlxMulDRMIconProvider::CGlxMulDRMIconProvider()
    {
    
    }
    
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//   
CGlxMulDRMIconProvider::~CGlxMulDRMIconProvider()
    {
    TRACER("CGlxMulDRMIconProvider::~CGlxMulDRMIconProvider");
    if(iDRMUtility)
        {
    iDRMUtility->Close();    // Memory Leak Fix
    }
    }


    
// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//   
EXPORT_C CGlxMulDRMIconProvider* CGlxMulDRMIconProvider::NewL( )
    {
    TRACER("CGlxMulDRMIconProvider::NewL");
    CGlxMulDRMIconProvider* self = new (ELeave) CGlxMulDRMIconProvider;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
// 
void CGlxMulDRMIconProvider::ConstructL()
    {
    TRACER("CGlxMulDRMIconProvider::ConstructL");
    iDRMUtility = CGlxDRMUtility::InstanceL();
    iIconId = EMbmAvkonQgn_note_empty;
    iMifFileName.Copy( KDC_APP_BITMAP_DIR );
    iMifFileName.Append( KGlxIconsFilename );
    }
    
// ----------------------------------------------------------------------------
// IconIdL
// ----------------------------------------------------------------------------
//   
TInt CGlxMulDRMIconProvider::IconIdL(const TGlxMedia& aMedia/**/ ) const
    {
    TRACER("CGlxMulDRMIconProvider::IconIdL");
    if( aMedia.IsDrmProtected() )
        {
        const TDesC& uri = aMedia.Uri();
        if( uri.Length() > 0)
            {
            if(iDRMUtility->IsForwardLockedL(uri))
                {
                return EMbmGlxiconsQgn_prop_drm_rights_valid_super;
                }
            else 
                {
                TMPXGeneralCategory  cat = aMedia.Category();                  
                TBool checkViewRights = (cat==EMPXImage);
                
                if(iDRMUtility->ItemRightsValidityCheckL(uri, checkViewRights))
                        {
                        return EMbmGlxiconsQgn_prop_drm_rights_valid_super;        
                        }
                     else
                        {
                        return EMbmGlxiconsQgn_prop_drm_rights_exp_super;
                        }
                }
            }
        }
    
    return iIconId;
    }

// ----------------------------------------------------------------------------
// ResourceFileName
// ----------------------------------------------------------------------------
//   
const TDesC& CGlxMulDRMIconProvider::ResourceFileName() const
    {
    TRACER("CGlxMulDRMIconProvider::ResourceFileName");
    return iMifFileName;
    }

    
// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//  
CGlxMulVideoIconProvider::CGlxMulVideoIconProvider(TBool aIsFullscreen)
	: iIsFullscreen(aIsFullscreen)
    {
    
    }
    
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//   
CGlxMulVideoIconProvider::~CGlxMulVideoIconProvider()
    {
    TRACER("CGlxMulVideoIconProvider::~CGlxMulVideoIconProviderr");
    }


    
// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//   
EXPORT_C CGlxMulVideoIconProvider* CGlxMulVideoIconProvider::NewL(TBool aIsFullscreen)
    {
    TRACER("CGlxMulVideoIconProvider::NewL");
    CGlxMulVideoIconProvider* self = new (ELeave) CGlxMulVideoIconProvider(aIsFullscreen);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
// 
void CGlxMulVideoIconProvider::ConstructL()
    {
    TRACER("CGlxMulVideoIconProvider::ConstructL");
    iIconId = EMbmAvkonQgn_note_empty;
    iMifFileName.Copy( KDC_APP_BITMAP_DIR );
    iMifFileName.Append( KGlxIconsFilename );
    }
    
// ----------------------------------------------------------------------------
// IconIdL
// ----------------------------------------------------------------------------
//   
TInt CGlxMulVideoIconProvider::IconIdL(const TGlxMedia& aMedia) const
    {
    TRACER("CGlxMulVideoIconProvider::IconIdL");
        
    if((aMedia.Category() == EMPXVideo) && (GlxErrorManager::HasAttributeErrorL(aMedia.Properties(), KGlxMediaIdThumbnail ) == KErrNone))
        {
		if (iIsFullscreen)
            {                       	        	
		    return EMbmGlxiconsQgn_indi_media_fullscreen_play;			                
            }        
    	return EMbmGlxiconsQgn_indi_media_thumbnail;      
        }   
    return iIconId;
    }

// ----------------------------------------------------------------------------
// ResourceFileName
// ----------------------------------------------------------------------------
//   
const TDesC& CGlxMulVideoIconProvider::ResourceFileName() const
    {
    TRACER("CGlxMulVideoIconProvider::ResourceFileName");
    return iMifFileName;
    }

    
