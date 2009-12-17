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
* Description:    wrapper for Back Stepping Service
*
*/




// from AIW and LIW
#include <liwcommon.h>
#include <liwservicehandler.h>

#include <glxtracer.h>
#include <glxlog.h>

// parameter names used by BS Service
#include "glxbsserviceconstants.h"

// for CleanupResetAndDestroyPushL
#include <mmf/common/mmfcontrollerpluginresolver.h>


#include "glxbackservicewrapper.h"

using namespace LIW;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CGlxBackServiceWrapper::CGlxBackServiceWrapper()
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CGlxBackServiceWrapper::ConstructL( const TUid aUid )
    {
    TRACER( "CGlxBackServiceWrapper::ConstructL" );
    // create Service Handler and keep as long as access to BS Service needed
    iServiceHandler = CLiwServiceHandler::NewL();
    // for convinience keep pointers to Service Handler param lists
	iInParamList = &iServiceHandler->InParamListL();
	iOutParamList = &iServiceHandler->OutParamListL();

    RCriteriaArray criteriaArray;
	CleanupResetAndDestroyPushL( criteriaArray );
	
	// create Liw criteria
	CLiwCriteriaItem* criterion = 
	                CLiwCriteriaItem::NewLC( KLiwCmdAsStr, KBSInterface, KBSServiceID );
	criterion->SetServiceClass( TUid::Uid( KLiwClassBase ) );

	criteriaArray.AppendL( criterion );
	CleanupStack::Pop( criterion );

    // attach Liw criteria
	iServiceHandler->AttachL( criteriaArray );
	// get BS Service interface
	iServiceHandler->ExecuteServiceCmdL( *criterion, 
	                                        *iInParamList, 
	                                        *iOutParamList );

	CleanupStack::PopAndDestroy( &criteriaArray );
	
	// extract BS interface from output params
	TInt pos( 0 );
	iOutParamList->FindFirst( pos, KBSInterface );
	if( pos != KErrNotFound )
		{
		iBSInterface = (*iOutParamList)[pos].Value().AsInterface();	
		}
    
    if ( !iBSInterface )
        {
        // handle no BS Service
        User::Leave( KErrNotFound );
        }

    InitializeL( aUid );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxBackServiceWrapper* CGlxBackServiceWrapper::NewL( 
                                                            const TUid aUid )
    {
    TRACER( "CGlxBackServiceWrapper::NewL" );
    CGlxBackServiceWrapper* self = 
                        CGlxBackServiceWrapper::NewLC( aUid );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxBackServiceWrapper* CGlxBackServiceWrapper::NewLC( 
                                                            const TUid aUid )
    {
    TRACER( "CGlxBackServiceWrapper::NewLC" );
    CGlxBackServiceWrapper* self = new( ELeave ) CGlxBackServiceWrapper;
    CleanupStack::PushL( self );
    self->ConstructL( aUid );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CGlxBackServiceWrapper::~CGlxBackServiceWrapper()
    {
    TRACER( "CGlxBackServiceWrapper::~CGlxBackServiceWrapper" );
    if ( iBSInterface )
        {
        iBSInterface->Close();
        }

    delete iServiceHandler;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CGlxBackServiceWrapper::ForwardActivationEventL( const TDesC8& aState, 
                                                        const TBool aEnter )
    {
    TRACER( "CGlxBackServiceWrapper::ForwardActivationEventL" );
    iInParamList->Reset();
	iOutParamList->Reset();

	TLiwGenericParam state( KBSInParamState, TLiwVariant( aState ) );
	iInParamList->AppendL( state );
	TLiwGenericParam enter( KBSInParamEnter, TLiwVariant( aEnter ) );
	iInParamList->AppendL( enter );
	
	iBSInterface->ExecuteCmdL( KBSCmdForwardActivationEvent, 
	                            *iInParamList, 
	                            *iOutParamList );
	
    TInt result = HandleResultL();
	
	iInParamList->Reset();
	iOutParamList->Reset();
	
	return result;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CGlxBackServiceWrapper::HandleBackCommandL( const TDesC8& aState, 
                                                   const TBool aCheckOnly)
    {
    TRACER( "CGlxBackServiceWrapper::HandleBackCommandL" );
    iInParamList->Reset();
	iOutParamList->Reset();

	TLiwGenericParam state( KBSInParamState, TLiwVariant( aState ) );
	iInParamList->AppendL( state );
	TLiwGenericParam checkOnly( KBSInParamCheckOnly, TLiwVariant( aCheckOnly ) );
	iInParamList->AppendL( checkOnly );
	
	iBSInterface->ExecuteCmdL( KBSCmdHandleBackCommand, 
	                            *iInParamList, 
	                            *iOutParamList );
	
	TInt result = HandleResultL();
	
	iInParamList->Reset();
	iOutParamList->Reset();
	
	return result;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CGlxBackServiceWrapper::InitializeL( const TUid aUid )
    {
    TRACER( "CGlxBackServiceWrapper::InitializeL" );
    iInParamList->Reset();
	iOutParamList->Reset();

	TLiwGenericParam appUid( KBSInParamAppUid, TLiwVariant( aUid.iUid ) );
	iInParamList->AppendL( appUid );
	
	iBSInterface->ExecuteCmdL( KBSCmdInitialize, 
	                            *iInParamList, 
	                            *iOutParamList );

    if ( HandleResultL() != 1 )
        {
        // Initialize command was not consumed - handle failiure
        User::Leave( KErrArgument );
        }
	
	iInParamList->Reset();
	iOutParamList->Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CGlxBackServiceWrapper::HandleResultL()
    {
    TRACER( "CGlxBackServiceWrapper::HandleResultL" );
    TInt statusInfo( KErrNotFound );
    
    TInt posErr( 0 );
	iOutParamList->FindFirst( posErr, EGenericParamError );
	if( posErr != KErrNotFound )
		{
		// error code found - extract and handle
		TInt errorCode( KErrNone );
		(*iOutParamList)[posErr].Value().Get( errorCode );
		
		User::LeaveIfError( errorCode );
		}

    // if no error returned, carry on
    TInt posStat( 0 );
    iOutParamList->FindFirst( posStat, KBSOutParamStatusInfo );
    if( posStat != KErrNotFound )
		{
		// status info present - extract and return
		(*iOutParamList)[posStat].Value().Get( statusInfo );
		}
	else
	    {
	    // no return value
	    // this should not happen
	    User::Leave( KErrNotFound );
	    }

    return statusInfo;
    }
