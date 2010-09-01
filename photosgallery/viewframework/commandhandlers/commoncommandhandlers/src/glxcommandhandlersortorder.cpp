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
* Description:    SortOrder command handler
*
*/




#include "glxcommandhandlersortorder.h"

#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>
#include <glxpanic.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <centralrepository.h>		// For CRepository
#include <mglxmedialist.h>			// For MGlxMediaList
#include <glxsettingsmodel.h>		// For cenrep setting 
	

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerSortOrder* CGlxCommandHandlerSortOrder::NewL(
        					MGlxMediaListProvider* aMediaListProvider,
							const TUint32 aCRKey)
    {
    CGlxCommandHandlerSortOrder* self = new ( ELeave )
    	CGlxCommandHandlerSortOrder(aMediaListProvider, aCRKey);
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
CGlxCommandHandlerSortOrder::CGlxCommandHandlerSortOrder(
							MGlxMediaListProvider* aMediaListProvider,
							const TUint32 aCRKey)
    : CGlxMediaListCommandHandler(aMediaListProvider), iKey(aCRKey)
    {
    // Do nothing
    }
 
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSortOrder::ConstructL()
    {
    TRACER("CGlxCommandHandlerSortOrder::ConstructL");
    
    // Add supported commands
    GLX_LOG_INFO("CGlxCommandHandlerSortOrder::ConstructL::Add alphabetical sort order command handler");
   	TCommandInfo info( EGlxCmdSortByAlphabetical );
    info.iMinSelectionLength = 1;
    info.iMaxSelectionLength = KMaxTInt;
   	AddCommandL(info);
   	
   	GLX_LOG_INFO("CGlxCommandHandlerSortOrder::ConstructL::Add frequency sort order command handler");
   	TCommandInfo info2( EGlxCmdSortByCount );
    info2.iMinSelectionLength = 1;
    info2.iMaxSelectionLength = KMaxTInt;
   	AddCommandL(info2);
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerSortOrder::~CGlxCommandHandlerSortOrder()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// DoExecute - the relevant action for the command id
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerSortOrder::DoExecuteL(TInt aCommandId,
													MGlxMediaList& aList)
	{
    TRACER("CGlxCommandHandlerSortOrder::DoExecuteL");
    
    __ASSERT_DEBUG( NULL != &aList, Panic(EGlxPanicIllegalArgument) );
	
	GLX_LOG_INFO1("CGlxCommandHandlerSortOrder::DoExecuteL::Command Id (%d)",aCommandId);
	TGlxFilterSortOrder newSortOrder = EGlxFilterSortOrderNotUsed;
	TGlxFilterSortDirection newSortDirection = EGlxFilterSortDirectionNotUsed;
    TBool handledCommand = EFalse;

	switch (aCommandId)
		{
        case EGlxCmdSortByAlphabetical:
            {
  			GLX_LOG_INFO("CGlxCommandHandlerSortOrder::DoExecuteL::Sort Alphabetically");

       		newSortOrder = EGlxFilterSortOrderAlphabetical;
       		newSortDirection = EGlxFilterSortDirectionAscending;
       		handledCommand = ETrue;
            break;
            }
        case EGlxCmdSortByCount:
            {
  			GLX_LOG_INFO("CGlxCommandHandlerSortOrder::DoExecuteL::Sort by count");
  			
       		newSortOrder = EGlxFilterSortOrderItemCount;
       		newSortDirection = EGlxFilterSortDirectionDescending;	        		
       		handledCommand = ETrue;
            break;
            }            
        default:
        	{
        	break;
        	}
		}
		
	// Check if the existing and the new sort order are different, the change the filter in MediaList
	if( newSortOrder != EGlxFilterSortOrderNotUsed )
		{
		GLX_LOG_INFO("CGlxCommandHandlerSortOrder::DoExecuteL::Create a new sort order");

		TGlxFilterProperties filterProperties;
    	filterProperties.iSortOrder = newSortOrder;
    	filterProperties.iSortDirection = newSortDirection;
    	
    	CMPXFilter* newFilter = TGlxFilterFactory::CreateCombinedFilterL(filterProperties, 
    	                                                                    aList.Filter());
    	CleanupStack::PushL(newFilter);

    	aList.SetFilterL(newFilter);
    	// Update central repository with new sort order
    	CGlxSettingsModel::SetValueL(iKey,newSortOrder);
    	CleanupStack::PopAndDestroy(newFilter);
		}
	
	return handledCommand;
	}

// -----------------------------------------------------------------------------
// DoIsDisabled
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerSortOrder::DoIsDisabled(TInt aCommandId, 
													MGlxMediaList& aList) const
	{
    TRACER("CGlxCommandHandlerSortOrder::DoIsDisabled");
	
    __ASSERT_DEBUG( NULL != &aList, Panic(EGlxPanicIllegalArgument) );
    
	// Get the existing filter & try to find out what is the current sort order
	GLX_LOG_INFO("CGlxCommandHandlerSortOrder::DoIsDisabled::Get the existing filter");
    TGlxFilterSortOrder currentSortOrder = CurrentSortOrder(aList);		
	
	GLX_LOG_INFO1("CGlxCommandHandlerSortOrder::DoIsDisabled::Command Id (%d)",aCommandId);
	TBool disabled = EFalse;
	
	TInt count(aList.Count());
    switch( aCommandId)
    	{
        case EGlxCmdSortByAlphabetical:
            {
            GLX_LOG_INFO("CGlxCommandHandlerSortOrder::DoIsDisabled::Sort Alphabetically");
            
            // If the filter is already Alphabetical sort then hide the menu
            if( count )
            	{
            	disabled = ( currentSortOrder == EGlxFilterSortOrderAlphabetical);
            	}
            else
            	{
            	disabled = ETrue; // Menu options should appear only if the list count is non-zero
            	}
        	
            break;
            }
        case EGlxCmdSortByCount:
            {
    		GLX_LOG_INFO("CGlxCommandHandlerSortOrder::DoIsDisabled::Sort by count");
    		
    		// If the filter is already Frequency sort then hide the menu
            if( count )
            	{
            	disabled = ( currentSortOrder == EGlxFilterSortOrderItemCount);
            	}
            else
            	{
            	disabled = ETrue; // Menu options should appear only if the list count is non-zero            	
            	}
            break;
            }            
        default:
        	{
        	break;
        	}
		}
	
	return disabled;
	}

// -----------------------------------------------------------------------------
// GetSortOrder
// -----------------------------------------------------------------------------
//
TGlxFilterSortOrder	CGlxCommandHandlerSortOrder::CurrentSortOrder(MGlxMediaList& aList) const
    {
    TRACER("CGlxCommandHandlerSortOrder::CurrentSortOrder");
    
    __ASSERT_DEBUG( NULL != &aList, Panic(EGlxPanicIllegalArgument) );
    
    TGlxFilterSortOrder currentSortOrder = EGlxFilterSortOrderNotUsed;
	
	CMPXFilter* filter = aList.Filter();
	if ( filter )
		{
		GLX_LOG_INFO("CGlxCommandHandlerSortOrder::DoIsDisabled::Find the existing sort order");
        currentSortOrder = filter->ValueTObjectL<TGlxFilterSortOrder>(KGlxFilterGeneralSortType);
		}
		
	return currentSortOrder;
    }
	
//End of file
