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
* Description:    Rename command handler
*
*/




#include "glxcommandhandlerrename.h"

#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <StringLoader.h>

#include <glxuiutilities.rsg>
#include <mglxmedialist.h>
#include <glxuistd.h>
#include <glxcommandhandlers.hrh>
#include <glxuiutility.h>
#include <glxtextentrypopup.h>
#include <glxattributeretriever.h>
#include <glxattributecontext.h>
#include <glxfetchcontextremover.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxgeneraluiutilities.h>
#include <glxicons.mbg>
#include <glxtracer.h>
#include <glxlog.h>

#include "glxcommandfactory.h"

namespace
	{
	const TInt KNameMaxLength = 128;
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerRename* CGlxCommandHandlerRename::NewL(
        MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem )
    {
    CGlxCommandHandlerRename* self = new ( ELeave ) CGlxCommandHandlerRename
    	( aMediaListProvider, aHasToolbarItem );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerRename::CGlxCommandHandlerRename( MGlxMediaListProvider*
	aMediaListProvider, TBool aHasToolbarItem )
	: CGlxMpxCommandCommandHandler( aMediaListProvider, aHasToolbarItem )
    {
    // Do nothing
    }
 
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRename::ConstructL()
    {
    iRenameText = HBufC::NewL( KNameMaxLength );

    // Load resource file
	TParse parse;
    parse.Set( KGlxUiUtilitiesResource, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile;
    resourceFile.Append( parse.FullName() );
    CGlxResourceUtilities::GetResourceFilenameL( resourceFile );  
   	iResourceOffset = CCoeEnv::Static()->AddResourceFileL( resourceFile );

   	// Add supported command
   	TCommandInfo info( EGlxCmdRename );
   	// This setup disables the command if a static item is focused,
   	// if the view is empty or if more than one item is selected.
    info.iMinSelectionLength = 1;
    info.iMaxSelectionLength = 1;
    // Filter out system items
    info.iDisallowSystemItems = ETrue;
    // Allow animated GIFs to be renamed
    info.iStopAnimationForExecution = ETrue;
    
   	AddCommandL( info );

    iErrorCallback = new (ELeave) CAsyncCallBack( TCallBack( HandleErrorL, this ), 
                                                  CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerRename::~CGlxCommandHandlerRename()
    {
    delete iRenameText;
    
    iErrorCallback->Cancel();
    delete iErrorCallback;

    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }
    }

// ---------------------------------------------------------------------------
// Create a rename command
// ---------------------------------------------------------------------------
//
CMPXCommand* CGlxCommandHandlerRename::CreateCommandL(TInt /*aCommandId*/, 
        MGlxMediaList& aMediaList, TBool& /*aConsume*/) const
    {
    TRACER( "CGlxCommandHandlerRename::CreateCommandL" );

    // Return value
    CMPXCommand* command = NULL;

    // Prompt the user for the new name
    // 
    // Show popup with the item's current title
    TPtr textPtr = iRenameText->Des();
    GetTitleL( textPtr, aMediaList );
    // store the current name.
    TBuf<KNameMaxLength> currentName; 
    currentName.Copy(textPtr);  

 	// Load the title for the popup 
    HBufC* title = StringLoader::LoadLC( R_GLX_POPUP_RENAME_TITLE );
    CGlxTextEntryPopup* popup = CGlxTextEntryPopup::NewL( *title,
    	textPtr );


    	
	if ( popup->ExecuteLD() == EEikBidOk && currentName != *iRenameText)
		{
		// Text entry was successful
		// There can be no selection when renaming, so assume that path contains focused item
	   	CMPXCollectionPath* path = aMediaList.PathLC( NGlxListDefs::EPathFocusOrSelection );
	   	// Create the rename command with the relevant text
	    command = TGlxCommandFactory::RenameCommandLC( *iRenameText, *path );
    	CleanupStack::Pop( command );
		CleanupStack::PopAndDestroy( path) ;
		}
					    	
    CleanupStack::PopAndDestroy( title );	
    
    return command;
    }

// -----------------------------------------------------------------------------
// Handle an error
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerRename::HandleErrorL(TInt aErrorCode)
    {
    TRACER( "CGlxCommandHandlerRename::HandleErrorL" );

    // Callback is required because the code to handle the error is blocking
    // There is only ever one command issued, store the error to handle later
    iError = aErrorCode;

    __ASSERT_DEBUG( !iErrorCallback->IsActive(), Panic( EGlxPanicLogicError ) );
    iErrorCallback->CallBack();
    }

// -----------------------------------------------------------------------------
// Callback to handle an error
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerRename::HandleErrorL(TAny* aPtr)
    {
    TRACER( "CGlxCommandHandlerRename::HandleErrorL" );

    static_cast<CGlxCommandHandlerRename*>( aPtr )->HandleErrorL();
    return 0;
    }

// -----------------------------------------------------------------------------
// Handle an error - e.g. if user inputs a name that already exists.
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerRename::HandleErrorL()
    {
    TRACER( "CGlxCommandHandlerRename::HandleErrorL" );

    // If name already exists, Show rename query and allow the user to try again
    // See EMYP-7A2E6J
    if ( iError == KErrAlreadyExists )
        {
        HBufC* text = StringLoader::LoadLC( R_GLX_RENAME_QUERY, *iRenameText );

        TBool confirmed = GlxGeneralUiUtilities::ConfirmQueryL( R_GLX_QUERY_OK_CANCEL, *text );
        if ( confirmed )
            {
            // Let user have another try at entering the new name
            ExecuteL( EGlxCmdRename );
            }

        CleanupStack::PopAndDestroy( text );
        }
    else
        {
        // Call the base class' implementation
        CGlxMpxCommandCommandHandler::HandleErrorL( iError );
        }
    }

// -----------------------------------------------------------------------------
// Retrieve the current item's title string
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerRename::GetTitleL( TDes& aTitle,
	MGlxMediaList& aList ) const
	{
	TRACER( "CGlxCommandHandlerRename::GetTitleL" );
    
	// Set the title to a blank name by default
	aTitle = KNullDesC;
    
    // Create an iterator to retrieve the relevant attribute
    TGlxSelectionIterator iterator;
    // only want the title of one item
	iterator.SetRange( 1 );
    CGlxAttributeContext* context = new( ELeave )
        CGlxAttributeContext( &iterator );
	CleanupStack::PushL( context );
        
	// Want to read the title attribute    
    // Configure the context
	context->AddAttributeL( KMPXMediaGeneralTitle );
	
	// Add the context to the media list
	aList.AddContextL( context, KGlxFetchContextPriorityBlocking );

    // TGlxContextRemover will remove the context when it goes out of scope
    // Used here to avoid a trap and still have safe cleanup    
    TGlxFetchContextRemover contextRemover( context, aList );
    // put to cleanupstack as cleanupstack is emptied before stack objects
    // are deleted
    CleanupClosePushL( contextRemover );
	// Yuck - a leave and a return code
	// EFalse => don't show a progress dialog
    TInt err = GlxAttributeRetriever::RetrieveL( *context, aList, EFalse );
    // Using a distinct error value as "LeaveIfError( FuncL() );" looks bad
    User::LeaveIfError( err );
    // context off the list
    CleanupStack::PopAndDestroy( &contextRemover );

    // Now get the title of the relevant item: if *an* item is selected use its
    // title, otherwise use the item with focus
	// Get the index of the item to rename
    TInt index = KErrNotFound;
	// first see if there's a selected item...
    if ( aList.SelectionCount() == 1 )
    	{
        // Find the index of the selected item
    	index = aList.Count();
    	do
            {
            --index;
            }
        while ( !aList.IsSelected( index ) && index > 0 );
    	}
    else
    	{
    	// Use the index of the item with focus
       	index = aList.FocusIndex();
    	}

	if ( index != KErrNotFound )
		{
        // use iterator to get the right item
        iterator.SetToFirst( &aList );
        const CGlxMedia* media = aList.Item( iterator++ ).Properties();
		if( media )
			{
			// found the item's media properties, extract the title
			aTitle = media->ValueText( KMPXMediaGeneralTitle ) ;
			}
		}
    CleanupStack::PopAndDestroy( context );
	}

//-----------------------------------------------------------------------------
// PopulateToolbar
//-----------------------------------------------------------------------------
//
void CGlxCommandHandlerRename::PopulateToolbarL()
	{
	TRACER( "CGlxCommandHandlerRename::PopulateToolbar" );
	}

//End of file
