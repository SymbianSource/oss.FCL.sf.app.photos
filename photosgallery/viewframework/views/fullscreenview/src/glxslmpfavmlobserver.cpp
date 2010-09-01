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
* Description:    Implementation of Single line meta pane favourites observer
*
*/

#include <glxcollectionpluginalbums.hrh>            // Albums collection plugin implementation id
#include <glxfilterfactory.h>                       // FilterFactory
#include <mglxmedialist.h>                          // CGlxMedialist
#include <glxattributecontext.h>                    // Attribute context
#include <glxuistd.h>                               // Attribute fetch priority
#include <glxtracer.h>
#include <glxlog.h>
#include <glxslmpfavmlobserver.h>

// ---------------------------------------------------------------------------
// NewL()
// ---------------------------------------------------------------------------
//  
CGlxSLMPFavMLObserver* CGlxSLMPFavMLObserver::NewL( CGlxSingleLineMetaPane& aSingleLineMetapane,
        TInt aFocusIndex, MGlxMediaList* aList)
    {
    TRACER("CGlxSLMPFavMLObserver::NewL()");
    CGlxSLMPFavMLObserver* self = new(ELeave) CGlxSLMPFavMLObserver(aSingleLineMetapane);
    CleanupStack::PushL(self);
    self->ConstructL(aFocusIndex, aList);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CGlxSLMPFavMLObserver()
// ---------------------------------------------------------------------------
//  
CGlxSLMPFavMLObserver::CGlxSLMPFavMLObserver(CGlxSingleLineMetaPane& aSingleLineMetaPane) 
            : iSingleLineMetaPane( aSingleLineMetaPane)
    {
    TRACER("CGlxSLMPFavMLObserver::CGlxSLMPFavMLObserver()");
    // Nothing done here
    }

// ---------------------------------------------------------------------------
// ~CGlxSLMPFavMLObserver()
// ---------------------------------------------------------------------------
//  
CGlxSLMPFavMLObserver::~CGlxSLMPFavMLObserver()
    {
    TRACER("CGlxSLMPFavMLObserver::~CGlxSLMPFavMLObserver()");
    if (iFavMediaList)
        {
        iFavMediaList->RemoveMediaListObserver(this);
        iFavMediaList->RemoveContext( iFavAttribContext );
        delete iFavAttribContext;
        iFavMediaList->Close();
        }
    }

// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::ConstructL(TInt aFocusIndex, MGlxMediaList* aList)
    {
    TRACER("CGlxSLMPFavMLObserver::ConstructL()");
    CMPXCollectionPath* favCollection = CMPXCollectionPath::NewL();
    CleanupStack::PushL(favCollection);
    // The target collection has to be appeneded with the albums plugin id
    favCollection->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
    // The target collection has also to be appeneded with the the relation id.
    // appending another level into the albums to get favourites and 1 is the relation id of albums
    favCollection->AppendL( TMPXItemId(1) );
    favCollection->Set( 0 );
    const TGlxMedia& item = aList->Item(aFocusIndex);
    const TDesC& uri = item.Uri();
    CMPXFilter* filter = TGlxFilterFactory::CreateURIFilterL(item.Uri());  
    CleanupStack::PushL(filter);

    // Create the media list
    iFavMediaList =  MGlxMediaList::InstanceL( *favCollection, 
                      TGlxHierarchyId(KGlxCollectionPluginAlbumsDllUid), filter );
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( favCollection );
    iFavMediaList->AddMediaListObserverL(this);
    iFavAttribContext = CGlxDefaultAttributeContext::NewL();
    // add all the attributes needed in handleattributes available
    // this is because even
    iFavAttribContext->AddAttributeL( KMPXMediaGeneralCount );
    // Using priority as same as UMP view's album pane
    iFavMediaList->AddContextL( iFavAttribContext, KGlxFetchContextPriorityLow );
    }

// ---------------------------------------------------------------------------
// HandleAttributesAvailableL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleAttributesAvailableL(TInt/* aItemIndex*/, 
        const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxSLMPFavMLObserver::HandleAttributesAvailableL()");
    // No Implementation Required
    }

// ---------------------------------------------------------------------------
// HandleItemAddedL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleItemAddedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/,
        MGlxMediaList* aList )
    {
    TRACER("CGlxSLMPFavMLObserver::HandleItemAddedL()");
    GLX_LOG_INFO1("CGlxSLMPFavMLObserver::HandleItemAddedL() medialist count = %d", aList->Count());
    // Passing ETrue as mediacount denotes item present in medialist 
    iSingleLineMetaPane.HandleUpdateIconL(ETrue);
    }

// ---------------------------------------------------------------------------
// HandleItemRemovedL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleItemRemovedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/,
        MGlxMediaList* /*aList */)
    {
    TRACER("CGlxSLMPFavMLObserver::HandleItemRemovedL()");
    // Passing EFalse as mediacount 0 denotes item not present in medialist 
    iSingleLineMetaPane.HandleUpdateIconL(EFalse);
    }

// ---------------------------------------------------------------------------
// HandleFocusChangedL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleFocusChangedL( NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxSLMPFavMLObserver::HandleFocusChangedL()");
    // No Implementation Required
    }

// ---------------------------------------------------------------------------
// HandleItemSelectedL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/,
        MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxSLMPFavMLObserver::HandleItemSelectedL()");
    // No Implementation Required
    }

// ---------------------------------------------------------------------------
// HandleMessageL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleMessageL( const CMPXMessage& /*aMessage*/, 
        MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxSLMPFavMLObserver::HandleMessageL()");
    // No Implementation Required
    }

// ---------------------------------------------------------------------------
// HandleError()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleError( TInt /*aError*/ )
    {
    TRACER("CGlxSLMPFavMLObserver::HandleError()");
    // No Implementation Required
    }

// ---------------------------------------------------------------------------
// HandleCommandCompleteL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/,
        TInt /*aError*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxSLMPFavMLObserver::HandleCommandCompleteL()");
    // No Implementation Required
    }

// ---------------------------------------------------------------------------
// HandleMediaL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxSLMPFavMLObserver::HandleMediaL()");
    // No Implementation Required
    }

// ---------------------------------------------------------------------------
// HandleItemModifiedL()
// ---------------------------------------------------------------------------
//  
void CGlxSLMPFavMLObserver::HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxSLMPFavMLObserver::HandleItemModifiedL()");
    // No Implementation Required
    }


//EOF
