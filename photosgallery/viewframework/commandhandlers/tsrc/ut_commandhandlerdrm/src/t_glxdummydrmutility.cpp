/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    CGlxMediaList unit tests
*
*/




//  CLASS HEADER
#include <glxdrmutility.h>
#include <glxattributeretriever.h>


_LIT( KGlxDRMValid, "DRM valid");
_LIT( KGlxDRMHasOnlineInfo, "DRM has info" );

void CGlxDRMUtility::Close()
    {
    delete this;
    }
    
TBool CGlxDRMUtility::CheckOpenRightsL( const TDesC& aUri, TBool )
    {
    TBool rightsInvalid = ETrue;
    
    if( aUri.Compare( KGlxDRMValid ) == 0 )
        {
        rightsInvalid = EFalse;
        }
        
    return rightsInvalid;
    }
    
TBool CGlxDRMUtility::ConsumeRightsL(const TDesC&)
    {
    return ETrue;
    }    
    
TBool CGlxDRMUtility::CanShowInfoOnlineL(TDesC& aUri)
    {
    TBool hasInfo = EFalse;
    
    if( aUri.Compare( KGlxDRMHasOnlineInfo ) == 0 )
        {
        hasInfo = ETrue;
        }
        
    return hasInfo;
    }
    
void CGlxDRMUtility::ShowInfoOnlineL(TDesC& /*aUri*/)
    {
    
    }
    
void CGlxDRMUtility::ShowRightsInfoL(const TDesC&)
    {
    
    }
    
TBool CGlxDRMUtility::IsForwardLockedL(const TDesC&)
    {
    return ETrue;
    }   
    
CGlxDRMUtility* CGlxDRMUtility::InstanceL()
    {
    CGlxDRMUtility* drm = new(ELeave)CGlxDRMUtility();
    return drm;
    }
    
TSize CGlxDRMUtility::DRMThumbnailSize(TSize& aSize)
    {
    return aSize;
    }
    
CGlxDRMUtility::CGlxDRMUtility()
    {
    
    }
    
CGlxDRMUtility::~CGlxDRMUtility()
    {
    
    }
    
TInt GlxAttributeRetriever::RetrieveL(const MGlxFetchContext&, MGlxMediaList&, TBool )
    {
    return KErrNone;
    }
    
//  END OF FILE
