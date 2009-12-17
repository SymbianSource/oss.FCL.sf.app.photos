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
* Description:    Upnp Command Handlers.
*
*/




#include "glxcommandhandlercopytohomenetwork.h"

//Includes
#include <eikmenup.h>
#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxcommandhandlers.hrh>
#include <glxtracer.h>
#include <glxmedialistiterator.h>
#include <mglxmedialist.h>
#include <mpxmediadrmdefs.h>


#include <upnpcopycommand.h>

	/**
	 * Retrieves the required attributes from the collection
	 * to execute the command. They are:
	 * KMPXMediaGeneralUri and KMPXMediaDrmProtected
	 * The attributes are retrieved for the selected items
	 * in the media list. If no items are selected, the attributes
	 * are retrieved for the focused item.
	 * 
	 * @param aList media list with selected or focused item.
	 */
	void RetrieveAttributesL(MGlxMediaList& aList);
	
	/**
	 * Builds an array of URIs (files names) of the selected
	 * items in a media list that are not DRM protected. 
	 * If no items are selected the uri of the focused item 
	 * will be added to the array (provided that it is not DRM
	 * protected)
	 * 
	 * The URI and DRM protected attributes must be retrieved 
	 * before calling this method.
	 * See @ref CGlxCommandHandlerCopyToHomeNetwork::RetrieveAttributesL
	 * 
	 * @param aList media list from which an array of URIs should 
	 * be built.
	 * 
	 * @return an array of URIs (files names) of the selected
	 * items in a media list.
	 * 
	 */
	CDesCArrayFlat* BuildSelectionArrayLC(MGlxMediaList& aList);  

//-----------------------------------------------------------------------------------------
// Two phased Constructor
//-----------------------------------------------------------------------------------------

EXPORT_C CGlxCommandHandlerCopyToHomeNetwork* CGlxCommandHandlerCopyToHomeNetwork::
            NewL(MGlxMediaListProvider& aMediaListProvider)
    {  
    TRACER( "CGlxCommandHandlerCopyToHomeNetwork::NewL");
    CGlxCommandHandlerCopyToHomeNetwork* self =new(ELeave) 
              CGlxCommandHandlerCopyToHomeNetwork(aMediaListProvider);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//-----------------------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------------------
CGlxCommandHandlerCopyToHomeNetwork::~CGlxCommandHandlerCopyToHomeNetwork()
    {
    }
  
//-----------------------------------------------------------------------------------------
// Executing the command handler
//-----------------------------------------------------------------------------------------
TBool CGlxCommandHandlerCopyToHomeNetwork::DoExecuteL(TInt aCommandId, MGlxMediaList& aList)
    {
    
    TRACER( "CGlxCommandHandlerCopyToHomeNetwork::DoExecuteL");
    
    if (aCommandId == EGlxCopyToHomeNetwork)
        {        
        RetrieveAttributesL(aList);
        CUpnpCopyCommand * copyCommand = CUpnpCopyCommand::NewL();
        CleanupStack::PushL(copyCommand);
        CDesCArrayFlat* selectionArray = BuildSelectionArrayLC(aList);
        copyCommand->CopyFilesL(selectionArray);
        CleanupStack::PopAndDestroy(selectionArray);
        CleanupStack::PopAndDestroy(copyCommand);        
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

//-----------------------------------------------------------------------------------------
// Disable the command handler
//-----------------------------------------------------------------------------------------
TBool CGlxCommandHandlerCopyToHomeNetwork::DoIsDisabled(TInt aCommandId, MGlxMediaList& /*aList*/) const
    {
    TRACER("CGlxCommandHandlerCopyToHomeNetwork::DoIsDisabled");    
    
    TBool isDisabled = ETrue;
    
    if (aCommandId == EGlxCopyToHomeNetwork)
        {
        // If a leave occurs then isDisabled will remain false. 
        TRAP_IGNORE(isDisabled = !CUpnpCopyCommand::IsAvailableL());
        }
    return isDisabled;
    }

//-----------------------------------------------------------------------------------------
//  CGlxCommandHandlerCopyToHomeNetwork::CGlxCommandHandlerCopyToHomeNetwork
//-----------------------------------------------------------------------------------------  
CGlxCommandHandlerCopyToHomeNetwork::CGlxCommandHandlerCopyToHomeNetwork(
MGlxMediaListProvider& aMediaListProvider)
  : CGlxMediaListCommandHandler(&aMediaListProvider)
    {
    TRACER(" CGlxCommandHandlerCopyToHomeNetwork::CGlxCommandHandlerCopyToHomeNetwork");
    }

//-----------------------------------------------------------------------------------------
// CGlxCommandHandlerCopyToHomeNetwork::ConstructL
//-----------------------------------------------------------------------------------------  
void CGlxCommandHandlerCopyToHomeNetwork::ConstructL()  
    {
    TRACER(" CGlxCommandHandlerCopyToHomeNetwork::ConstructL");
    AddCopyToHomeNetworkCommandL();
    }

//-----------------------------------------------------------------------------------------
// Executing the command handler
//-----------------------------------------------------------------------------------------
void CGlxCommandHandlerCopyToHomeNetwork::AddCopyToHomeNetworkCommandL()
    {
    // Add the copy to home Network Command        
    TCommandInfo commandInfo(EGlxCopyToHomeNetwork);                          
    commandInfo.iMinSelectionLength = 1;    
    commandInfo.iDisallowDRM = ETrue;
    AddCommandL(commandInfo);
    }

//-----------------------------------------------------------------------------------------
// BuildSelectionArrayLC
//-----------------------------------------------------------------------------------------
CDesCArrayFlat* BuildSelectionArrayLC(MGlxMediaList& aList)
    {
    TInt selectionCount = aList.SelectionCount();

    TInt granularity = selectionCount > 0 ? selectionCount : 1;
    CDesCArrayFlat* desArray = new(ELeave) CDesCArrayFlat(granularity);
    CleanupStack::PushL(desArray);

    if (aList.Count())
    	{
	    if (selectionCount > 0)
	        {
	        for (TInt i = 0; i < selectionCount; i++)
	            {
	            const TGlxMedia& item = aList.Item(aList.SelectedItemIndex(i));
	            if (!item.IsDrmProtected())
	                {
	                desArray->AppendL(item.Uri());
	                }
	            }
	        }
	    else
	        {
	        const TGlxMedia& item = aList.Item(aList.FocusIndex());
	        if (!item.IsDrmProtected())
	            {
	            desArray->AppendL(item.Uri());
	            }
	        }
    	}
    
  return desArray;
  }

//-----------------------------------------------------------------------------------------
// RetrieveAttributesL
//-----------------------------------------------------------------------------------------
void RetrieveAttributesL(MGlxMediaList& aList)
   {
   TGlxSelectionIterator iterator;
   iterator.SetRange(KMaxTInt);
   CGlxAttributeContext* attributeContext = new(ELeave) CGlxAttributeContext(&iterator);
   CleanupStack::PushL(attributeContext);
   attributeContext->AddAttributeL(KMPXMediaGeneralUri);
   attributeContext->AddAttributeL(KMPXMediaDrmProtected);
    
   User::LeaveIfError(GlxAttributeRetriever::RetrieveL(*attributeContext, aList ,EFalse));
   CleanupStack::PopAndDestroy(attributeContext);
   }

