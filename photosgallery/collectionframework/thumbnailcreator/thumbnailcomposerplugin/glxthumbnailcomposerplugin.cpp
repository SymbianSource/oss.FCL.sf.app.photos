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
* Description:    Thumbnail Composer plugin
*
*/




/**
 * @internal reviewed 12/07/2007 by Simon Brooks
 */

#include <e32base.h>

#include <MdELogicCondition.h>
#include <mdeconstants.h>

#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionframeworkdefs.h>

#include <glxassert.h>
#include <glxcollectionpluginall.hrh>
#include <glxthumbnailattributeinfo.h>
#include <glxbackgroundtnmessagedefs.h>
#include <glxmediaid.h>
#include <glxthumbnailinfo.h>
#include <glxfilterfactory.h>
#include <glxcommandfactory.h>
#include <glxtracer.h>
#include <hal.h>

#include "glxthumbnailcomposerplugin.h"

// This can be any number other than zero
const TUint KGlxBackgroundThumbnailAttributeId = 1;
const TInt KGlxThumbnailComposerInactivityTimeout = 30; // seconds
const TInt KGlxThumbnailComposerDelay = 10000; // microseconds
const TInt KGlxThumbnailComposerHarvestDelay = 5000000; // microseconds
const TInt KGlxThumbnailComposerLargeDelay = 30000000; // microseconds
const TInt KGlxDisplayStatusTimeOutDelay = 5000000; // 5 secs

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CGlxThumbnailCompeserInactivityTimer::~CGlxThumbnailCompeserInactivityTimer()
    {
    // no implementation
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CGlxThumbnailCompeserInactivityTimer* CGlxThumbnailCompeserInactivityTimer::NewL(TInt aPriority, CGlxThumbnailComposerPlugin& aCallback)
    {
	TRACER("CGlxThumbnailCompeserInactivityTimer::NewL");
	CGlxThumbnailCompeserInactivityTimer* self = 
	                              new (ELeave) CGlxThumbnailCompeserInactivityTimer(aPriority, aCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	CActiveScheduler::Add(self);
	return self;
    }

// ---------------------------------------------------------------------------
// After
// ---------------------------------------------------------------------------
//
void CGlxThumbnailCompeserInactivityTimer::After( TTimeIntervalMicroSeconds32 aInterval )
    {
    TRACER("CGlxThumbnailCompeserInactivityTimer::After");

    // Fix for ELLZ-7DBBDZ and JPKN-7M2AYJ: to prevent a KERN EXEC 15 panic that occurs 
    // if a timer request is made while one is already outstanding.
    Cancel();
    // Forward the call on to the base class.
    CTimer::After( aInterval );
    }

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CGlxThumbnailCompeserInactivityTimer::CGlxThumbnailCompeserInactivityTimer(TInt aPriority, CGlxThumbnailComposerPlugin& aCallback)
         : CTimer(aPriority), iCallback(aCallback)
        {
        // no implementation
        }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailCompeserInactivityTimer::RunL()
    {
	TRACER("CGlxThumbnailCompeserInactivityTimer::RunL");
	
    TInt dispState = KErrNotFound;
    TInt ret = HAL::Get(HALData::EDisplayState, dispState);
    GLX_DEBUG4("CGlxThumbnailCompeserInactivityTimer::RunL iCallback.iState = %d, ret=%d, dispState= %d", 
    																iCallback.iState, ret, dispState );
	if (iCallback.iState && ret == KErrNone && dispState)
		{
		iCallback.DelayDisplayStateCheck();
	    GLX_LOG_INFO("GlxThumbnailCompeser - DISPLAY ON - DO NOT PROCESS" );
		return;
		}

	GLX_LOG_INFO("GlxThumbnailCompeser - DISPLAY OFF - ALLOW TO PROCESS" );
		
	switch (iCallback.iState)
	    {
    	case CGlxThumbnailComposerPlugin::EStateFirstOpening:
    	case CGlxThumbnailComposerPlugin::EStateOpening:
    	    {
            iCallback.DoOpenCollectionL();
            break;
    	    }
        case CGlxThumbnailComposerPlugin::EStateActiveSmall:
        case CGlxThumbnailComposerPlugin::EStateActiveLarge:
            {
        	iCallback.DoRequestItemL();
        	break;
            }
        case CGlxThumbnailComposerPlugin::EStateClosing:
            {
            iCallback.CloseCollection();
            break;
            }
	    }
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CGlxThumbnailCompeserInactivityTimer::DoCancel()
    {
    // no implementation
    }

// ---------------------------------------------------------------------------
// RunError
// ---------------------------------------------------------------------------
//
TInt CGlxThumbnailCompeserInactivityTimer::RunError(TInt /*aError*/)
    {
	TRACER("CGlxThumbnailCompeserInactivityTimer::RunError");
    iCallback.ReturnToIdle();
    return KErrNone;
    }



// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CGlxThumbnailComposerPlugin* CGlxThumbnailComposerPlugin::NewL()
	{
	TRACER("CGlxThumbnailComposerPlugin::NewL");
	CGlxThumbnailComposerPlugin* self = 
	                              new (ELeave) CGlxThumbnailComposerPlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// ~CGlxThumbnailComposerPlugin
// ---------------------------------------------------------------------------
//
CGlxThumbnailComposerPlugin::~CGlxThumbnailComposerPlugin()
	{
	delete iPath;
    if (iCollectionUtility)
        {
        iCollectionUtility->Close();
        }

	if ( iInactivityTimer )
	    {
    	iInactivityTimer->Cancel();
	    }
	delete iInactivityTimer;
	}

// ---------------------------------------------------------------------------
// CGlxThumbnailComposerPlugin
// ---------------------------------------------------------------------------
//
CGlxThumbnailComposerPlugin::CGlxThumbnailComposerPlugin() : iLastError(KErrNone)
	{
	}

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::ConstructL()
	{
	TRACER("CGlxThumbnailComposerPlugin::ConstructL");
	iInactivityTimer = CGlxThumbnailCompeserInactivityTimer::NewL(CActive::EPriorityIdle, *this);
	}

// ---------------------------------------------------------------------------
// SetObservers
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::SetObservers()
	{
	TRAPD(err, SetObserversL());
    GLX_ASSERT_ALWAYS( KErrNone == err, Panic( EGlxPanicEnvironment ),
                        "CGlxThumbnailComposerPlugin cannot set observers, reset Harvester server" );
	}

// ---------------------------------------------------------------------------
// SetObserversL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::SetObserversL()
	{
	TRACER("CGlxThumbnailComposerPlugin::SetObserversL");
	if ( iSession )
		{
		CMdELogicCondition* condition = 
		            CMdELogicCondition::NewLC( ELogicConditionOperatorOr );

#ifdef RD_MDS_2_0

		CMdENamespaceDef* defaultNamespace = 
		               iSession->GetDefaultNamespaceDefL();

    	CMdEObjectDef* imageDef = defaultNamespace->GetObjectDefL(
    	                                MdeConstants::Image::KImageObject );
		condition->AddObjectConditionL( *imageDef );

    	CMdEObjectDef* videoDef = defaultNamespace->GetObjectDefL(
    	                                MdeConstants::Video::KVideoObject );
		condition->AddObjectConditionL( *videoDef );

#else

		CMdENamespaceDef& defaultNamespace = 
		               iSession->GetDefaultNamespaceDefL();

    	CMdEObjectDef& imageDef = defaultNamespace.GetObjectDefL(
    	                                MdeConstants::Image::KImageObject );
		condition->AddObjectConditionL( imageDef );

    	CMdEObjectDef& videoDef = defaultNamespace.GetObjectDefL(
    	                                MdeConstants::Video::KVideoObject );
		condition->AddObjectConditionL( videoDef );

#endif

		iSession->AddObjectObserverL( *this, condition );
    	CleanupStack::Pop( condition );
    	
        if ( iState == EStateIdle )
            {
            iState = EStateFirstOpening;
            iInactivityTimer->After(KGlxThumbnailComposerLargeDelay);
            }
		}
	}

// ---------------------------------------------------------------------------
// RemoveObservers
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::RemoveObservers()
	{
	if ( iSession )
		{
#ifdef RD_MDS_2_0
		iSession->RemoveObjectObserver( *this );
#else
		TRAP_IGNORE(iSession->RemoveObjectObserverL( *this ));
#endif
		}
	}

// ---------------------------------------------------------------------------
// IsComposingComplete
// ---------------------------------------------------------------------------
//
TBool CGlxThumbnailComposerPlugin::IsComposingComplete()
	{
	TRACER("CGlxThumbnailComposerPlugin::IsComposingComplete");
	// Need to stop background thumbnail generation activity
	// when harvester requests to pause. 
	    
	    if (iState != EStateIdle)
	        {
	        iState = EStateClosing;
	        
	        if ( iInactivityTimer )
	            {
	            iInactivityTimer->Cancel();
	            }

	        if (iCollectionUtility)
	            {
	            iCollectionUtility->Close();
	            iCollectionUtility = NULL;
	            }
	            
	        iState = EStateIdle;
	        }
	    
	    return ETrue ;
	}

#ifdef RD_MDS_2_0

// ---------------------------------------------------------------------------
// HandleObjectAdded
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::HandleObjectAdded(
        CMdESession& /*aSession*/, const RArray<TItemId>& /*aObjectIdArray*/ )
	{
	TRACER("CGlxThumbnailComposerPlugin::HandleObjectAdded");
    if ( ( iState == EStateIdle ) || ( iState == EStateFirstOpening ) || ( iState == EStateClosing ) )
        {
        iInactivityTimer->Cancel();
        iState = EStateOpening;
        iInactivityTimer->After(20000000);
        }
    else
        {
        iLastError = 1;
        iState = EStateActiveLarge;
        }
	}

// ---------------------------------------------------------------------------
// HandleObjectModified
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::HandleObjectModified(
                                    CMdESession& /*aSession*/, 
                                    const RArray<TItemId>& /*aObjectIdArray*/)
	{
	//no implemention needed
	}

// ---------------------------------------------------------------------------
// HandleObjectRemoved
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::HandleObjectRemoved(
                                    CMdESession& /*aSession*/, 
                                    const RArray<TItemId>& /*aObjectIdArray*/)
	{
	//no implemention needed
	}

#else

void CGlxThumbnailComposerPlugin::HandleObjectNotification(CMdESession& /*aSession*/, 
					TObserverNotificationType aType,
					const RArray<TItemId>& /*aObjectIdArray*/)
	{
	TRACER("CGlxThumbnailComposerPlugin::HandleObjectNotification");
    if (aType == ENotifyAdd)
    	{
			if ( iState == EStateIdle )
		        {
		        iState = EStateOpening;
		        iInactivityTimer->After(KGlxThumbnailComposerLargeDelay);
		        }
		    else
		        {
		        iLastError = 1;
		        iState = EStateActiveLarge;
		        iInactivityTimer->After(KGlxThumbnailComposerLargeDelay);
		        }
    	}
	}

#endif

// ---------------------------------------------------------------------------
// HandleCollectionMediaL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::HandleCollectionMediaL(
                                const CMPXMedia& /*aMedia*/, TInt aError )
    {
	TRACER("CGlxThumbnailComposerPlugin::HandleCollectionMediaL");
    if ( iPath && KErrNone != aError )
        {
        GLX_LOG_WARNING1( "Error %d getting media", aError );

        // Error generating thumbnails, try the next item
        iLastError = aError;
        iCurrentIndex++;
        RequestItemL();
        }
    }

// ---------------------------------------------------------------------------
// HandleCollectionMessageL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::HandleCollectionMessage(
                                        CMPXMessage* aMsg, TInt /*aErr*/ )
    {
	TRACER("CGlxThumbnailComposerPlugin::HandleCollectionMessage");
    if ( iState != EStateIdle && aMsg )
        {
        TRAPD( err, DoHandleCollectionMessageL( *aMsg ) );
        if ( KErrNone != err )
            {
            GLX_LOG_WARNING1( "Error %d handling message", err );
            ReturnToIdle();
            }
        }
    }

// ---------------------------------------------------------------------------
// HandleOpenL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::HandleOpenL( const CMPXMedia& /*aEntries*/,
                          TInt /*aIndex*/, TBool /*aComplete*/, TInt aError )
    {
	TRACER("CGlxThumbnailComposerPlugin::HandleOpenL");
    if ( KErrNone != aError )
        {
        GLX_LOG_WARNING1( "Error %d opening collection", aError );
        ReturnToIdle();
        }
    }

// ---------------------------------------------------------------------------
// HandleOpenL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::HandleOpenL(
            const CMPXCollectionPlaylist& /*aPlaylist*/, TInt /*aError*/ )
    {
	// No implemention needed
    }

// ---------------------------------------------------------------------------
// HandleCommandComplete
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::HandleCommandComplete(
                        CMPXCommand* /*aCommandResult*/, TInt /*aError*/ )
    {
	TRACER("CGlxThumbnailComposerPlugin::HandleCommandComplete");
    if( EStateCleanup == iState )
        {
        ReturnToIdle();
        }
    else
        {
        iInactivityTimer->After(0);
        }
    }

// ---------------------------------------------------------------------------
// DoOpenCollectionL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::DoOpenCollectionL()
    {
	TRACER("CGlxThumbnailComposerPlugin::HandleCollectionMediaL");
    if ( !iCollectionUtility )
        {
        // Open isolated collection utility
        iCollectionUtility = MMPXCollectionUtility::NewL(
                                                this, KMcModeIsolated );
        }

    CMPXFilter* filter = CMPXFilter::NewL();
    CleanupStack::PushL(filter);
    filter->SetTObjectValueL<TGlxFilterSortDirection>(KGlxFilterGeneralSortDirection, EGlxFilterSortDirectionDescending);        
    iCollectionUtility->Collection().SetFilterL(filter);
    CleanupStack::PopAndDestroy(filter);

    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    // argument  path  all collection id
   	path->AppendL( KGlxCollectionPluginAllImplementationUid );

	iCollectionUtility->Collection().OpenL( *path );
 	CleanupStack::PopAndDestroy( path );

    iState = EStateActiveSmall;                    
    }

// ---------------------------------------------------------------------------
// DoHandleCollectionMessageL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::DoHandleCollectionMessageL(
                                                 const CMPXMessage& aMsg )
    {
	TRACER("CGlxThumbnailComposerPlugin::DoHandleCollectionMessageL");
    if ( aMsg.IsSupported( KMPXMessageGeneralId ) )
        {
        TInt messageId = aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralId );

	    // Is it a background thumbnail message        
	    if ( KGlxMessageIdBackgroundThumbnail == messageId )
	        {
	        // Is it the item expected
	        TGlxMediaId id( ( TUint32 )
	             aMsg.ValueTObjectL<TMPXItemId>( KGlxBackgroundThumbnailMediaId ) );
	        if ( iPath && id.Value()
	                        == ( TUint32 )iPath->IdOfIndex( iCurrentIndex ) )
	            {
	            TInt error = aMsg.ValueTObjectL<TInt>( KGlxBackgroundThumbnailError );
	            if ( error == KErrCancel )
	                {
	                iState = EStateActiveLarge;
    	            iLastError = error;
	                }
	            else if( KErrNone != error )
	                {
                    GLX_LOG_WARNING1( "Error %d in command", error );
                    error = KErrNone;
	                }
	            // Yes, then request the next item
	            iCurrentIndex++;
	            RequestItemL();
	            }
	        }
        else if ( KMPXMessageGeneral == messageId
                && aMsg.IsSupported( KMPXMessageGeneralEvent ) )
            {
            TInt messageEvent = aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralEvent );

            // Is it a path changed event
            if ( TMPXCollectionMessage::EPathChanged == messageEvent
                && aMsg.IsSupported( KMPXMessageGeneralType ) )
                {
    	        TInt messageType = aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralType );

             	if ( EMcPathChangedByOpen == messageType
             	    && iCollectionUtility && !iPath )
            	    {
         		    iPath = iCollectionUtility->Collection().PathL();

                   	iCurrentIndex = 0;
                    RequestItemL();
            	    }
               }
            }
        }
    }

// ---------------------------------------------------------------------------
// RequestItemL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::RequestItemL()
    {
	TRACER("CGlxThumbnailComposerPlugin::RequestItemL");
    if ( iLastError == KErrCancel )
        {
        iInactivityTimer->Inactivity(KGlxThumbnailComposerInactivityTimeout);
        }
    else if ( iLastError == 1 )
        {
        iInactivityTimer->After(KGlxThumbnailComposerHarvestDelay);
        }
    else
        {
        iInactivityTimer->After(KGlxThumbnailComposerDelay);
        }
    iLastError = KErrNone;
    }
    
// ---------------------------------------------------------------------------
// DoRequestItemL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::DoRequestItemL()
    {
	TRACER("CGlxThumbnailComposerPlugin::DoRequestItemL");

    if ( iPath && iCurrentIndex < iPath->Count() )
        {
        GLX_ASSERT_ALWAYS( iCollectionUtility, Panic( EGlxPanicNullPointer ),
                            "No collection utility" );
        // request next item
    	iPath->Set( iCurrentIndex );

        RArray<TMPXAttribute> attrs;
        CleanupClosePushL( attrs );
        // Add thumbnail attribute
        attrs.AppendL( TMPXAttribute(
                KGlxMediaIdThumbnail, KGlxBackgroundThumbnailAttributeId ) );

        // Don't add spec for bitmap handle, to request background thumbnail
        CMPXAttributeSpecs* attrSpecs = NULL;
        
        // Start first task
        iCollectionUtility->Collection().MediaL(
                                    *iPath, attrs.Array(), attrSpecs );
        CleanupStack::PopAndDestroy( &attrs );
        }
    else
        {
        delete iPath;
        iPath = NULL;
        
        if ( iState == EStateActiveLarge )
            {
            // go on to do small thumbnails
            DoOpenCollectionL();
            }
        else
            {
            CMPXCommand* command
                            = TGlxCommandFactory::ThumbnailCleanupCommandLC();
            iCollectionUtility->Collection().CommandL( *command );
            CleanupStack::PopAndDestroy( command );

            iState = EStateCleanup;
            }
        }
    }

// ---------------------------------------------------------------------------
// ReturnToIdle
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::ReturnToIdle()
    {
	TRACER("CGlxThumbnailComposerPlugin::ReturnToIdle");
    iState = EStateClosing;
    iInactivityTimer->After(KGlxThumbnailComposerLargeDelay);
    }

// ---------------------------------------------------------------------------
// DelayDisplayStateCheck
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::DelayDisplayStateCheck()
    {
	TRACER("CGlxThumbnailComposerPlugin::DelayDisplayStateCheck");
    iInactivityTimer->After(KGlxDisplayStatusTimeOutDelay);	
    }

// ---------------------------------------------------------------------------
// CloseCollection
// ---------------------------------------------------------------------------
//
void CGlxThumbnailComposerPlugin::CloseCollection()
    {
	TRACER("CGlxThumbnailComposerPlugin::CloseCollection");
    if ( ( EStateClosing == iState ) && iCollectionUtility )
        {
        iCollectionUtility->Close();
        iCollectionUtility = NULL;
        iState = EStateIdle;
        }
    }
