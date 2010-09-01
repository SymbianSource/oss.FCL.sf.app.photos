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
* Description:    general ui utilities
*
*/




#include <StringLoader.h>
#include <glxuiutilities.rsg>
#include <bautils.h>
#include "glxgeneraluiutilities.h"

// -----------------------------------------------------------------------------
// GlxGeneralUiUtilities::ShowErrorNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void GlxGeneralUiUtilities::ShowErrorNoteL(TInt aError)
	{
	// TextResolver instance for error resolving.
    CTextResolver* textresolver = CTextResolver::NewLC();
    // Resolve the error text
    const TDesC& text =
        textresolver->ResolveErrorString( aError );
    ShowErrorNoteL(text, ETrue);
    CleanupStack::PopAndDestroy( textresolver );
	}

// -----------------------------------------------------------------------------
// GlxGeneralUiUtilities::ConfirmQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool GlxGeneralUiUtilities::ConfirmQueryL( TInt aQueryResourceId,
                                                const TDesC& aTitleText )
    {
    CAknQueryDialog* query = CAknQueryDialog::NewL();
    TInt ret = query->ExecuteLD( aQueryResourceId, aTitleText );

    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        return ETrue;
        }

    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// GlxGeneralUiUtilities::ConfirmQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool GlxGeneralUiUtilities::ConfirmQueryL( const TDesC& aTitleText )
    {
    return ConfirmQueryL(R_GLX_QUERY_OK_CANCEL, aTitleText);
    }
    
// -----------------------------------------------------------------------------
// GlxGeneralUiUtilities::ShowErrorNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void GlxGeneralUiUtilities::ShowErrorNoteL( const TDesC& aInfoText,
                                      TBool aWaitingDialog )
    {
    CAknErrorNote* infoNote =
                          new( ELeave ) CAknErrorNote( aWaitingDialog );
    infoNote->ExecuteLD( aInfoText );
    }   

// -----------------------------------------------------------------------------
// GlxGeneralUiUtilities::ShowInfoNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void GlxGeneralUiUtilities::ShowInfoNoteL( const TDesC& aInfoText,
                                      TBool aWaitingDialog )
    {
    CAknInformationNote* infoNote =
                          new( ELeave ) CAknInformationNote( aWaitingDialog );
    infoNote->ExecuteLD( aInfoText );
    }

// -----------------------------------------------------------------------------
// GlxGeneralUiUtilities::ShowConfirmationNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void GlxGeneralUiUtilities::ShowConfirmationNoteL( const TDesC& aInfoText,
                                              TBool aWaitingDialog )
    {
    CAknConfirmationNote* confNote =
                            new( ELeave ) CAknConfirmationNote( aWaitingDialog );
    confNote->ExecuteLD( aInfoText );
    }
    

// -----------------------------------------------------------------------------
// GlxGeneralUiUtilities::FormatString
// -----------------------------------------------------------------------------
//
EXPORT_C void GlxGeneralUiUtilities::FormatString( TDes& aDestination,
                                      const TDesC& aSource,
                                      TInt aPosition,
                                      TInt aNumber,
                                      TBool aNumberConversion )
    {
    StringLoader::Format( aDestination, aSource, aPosition, aNumber );

    if( aNumberConversion )
        {
        AknTextUtils::LanguageSpecificNumberConversion( aDestination );
        }
    }
    
// -----------------------------------------------------------------------------
// GlxGeneralUiUtilities::IsLandscape()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool GlxGeneralUiUtilities::IsLandscape()
    {    
    // This class does not have access to a CEikonEnv and hence 
    // pls ignore the code scanner warning - Using CEikonEnv::Static
    CAknAppUiBase::TAppUiOrientation orientation = static_cast<CAknAppUi*>((CEikonEnv::Static()->EikAppUi()))->Orientation();

    if (orientation == CAknAppUiBase::EAppUiOrientationLandscape)
        {	
        return true;		
        }

    else if (orientation == CAknAppUiBase::EAppUiOrientationUnspecified )
        {
        AknLayoutUtils::TAknCbaLocation cbaLocation = AknLayoutUtils::CbaLocation();
        if ( (cbaLocation == AknLayoutUtils::EAknCbaLocationRight) ||
             (cbaLocation == AknLayoutUtils::EAknCbaLocationLeft) )
            {
            //landscape
            return true;
            }
        else
            {
            // Portrait
            return false;
            }
        }
        
    return false;
    }
    
    
// -----------------------------------------------------------------------------
// GlxGeneralUiUtilities::LayoutIsMirrored
// -----------------------------------------------------------------------------
//
EXPORT_C TBool GlxGeneralUiUtilities::LayoutIsMirrored()
    {
// remove comment from next line to force arabic/hebrew layout for testing    
//#define FORCE_ARABIC_HEBREW_LAYOUT   
#ifdef FORCE_ARABIC_HEBREW_LAYOUT
    return ETrue;
#else
    return AknLayoutUtils::LayoutMirrored();
#endif
    }
