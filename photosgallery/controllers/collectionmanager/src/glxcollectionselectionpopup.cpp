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
* Description:    Interface for displaying/using the popup to select collections
*
*/



#include "glxcollectionselectionpopup.h"

#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionplugintags.hrh>
#include <mpxcollectionpath.h>
#include <glxfilterfactory.h>
#include <mglxmedialist.h>
#include <glxmediaselectionpopup.h>

 
// -----------------------------------------------------------------------------
// ShowPopupL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TGlxCollectionSelectionPopup::ShowPopupL(
        RArray<TUint32>& aSelectedIds, TUint32 aCollectionType, 
        TBool aAllowMultipleSelection, TBool aEnableContainerCreation)
	{
	TUint32 collectionId = 0;
	CMPXFilter* filter = NULL;

    if (aCollectionType == KGlxCollectionTypeIdAlbum)
        {
        aAllowMultipleSelection = EFalse;
        collectionId = KGlxCollectionPluginAlbumsImplementationUid;   
        filter = TGlxFilterFactory::CreateCameraAlbumExclusionFilterL();
	    CleanupStack::PushL(filter);
        }
    else if (aCollectionType ==  KGlxCollectionTypeIdTag)
        {
        collectionId = KGlxTagCollectionPluginImplementationUid;
        filter = TGlxFilterFactory::CreateIncludeEmptyContainersFilterL();
        CleanupStack::PushL(filter);
        }
    
    __ASSERT_DEBUG(collectionId, Panic(EGlxPanicIllegalState));
     // build the path.
    
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    
    path->AppendL(collectionId);
      
    CGlxMediaSelectionPopup* popup = new (ELeave) CGlxMediaSelectionPopup;
    
    TBool accepted = EFalse;
    CMPXCollectionPath* selection = popup->ExecuteLD(*path, accepted, aAllowMultipleSelection, aEnableContainerCreation, filter);
        
    if (accepted)
        {
        CleanupStack::PushL(selection);
        aSelectedIds.Reset();
        
        TArray<TInt> selectionArray = selection->Selection();
        
        TInt count = selectionArray.Count();
        if (count)
        	{
	        for (TInt i = 0; i < count; i++)
	            {
	            aSelectedIds.AppendL(selection->IdOfIndex(selectionArray[i]));
	            }
        	}
        else
        	{
        	aSelectedIds.AppendL(selection->Id());
        	}
        
        CleanupStack::PopAndDestroy(selection);
        }

    CleanupStack::PopAndDestroy(path);
    
    if (filter)
    	{
    	CleanupStack::PopAndDestroy(filter);
    	}
    	
    
    if (accepted)
        {        
        return KErrNone;
        }
    else
        {
        return KErrCancel;
        }
	}
