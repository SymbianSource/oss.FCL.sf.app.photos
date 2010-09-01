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
* Description:    Details commmand handler
*
*/




#ifndef __C_GLXCOMMANDHANDLERDETAILS_H__
#define __C_GLXCOMMANDHANDLERDETAILS_H__

#include <e32base.h>
#include <glxmedialistcommandhandler.h>

class MGlxMediaListProvider;

class CGlxUiUtility;

/**
 * Command handler for viewing Details
 */
NONSHARABLE_CLASS (CGlxCommandHandlerDetails)
    : public CGlxMediaListCommandHandler
	{
	public:
	/** 
	 * Create command handler for launcing Metadata view or Tag manager
	 * use above method for creating command handler
	 * 
	 * @param aCommandId Command id for command handler
	 * @param aMediaListProvider media list owner
	 * @param aFileName resource file
	 */
	IMPORT_C static CGlxCommandHandlerDetails* NewL(
			MGlxMediaListProvider* aMediaListProvider, const TDesC& aFileName);

	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerDetails();
	
	private:

	/** Second phase constructor 
	 * 
	 * @param aCommandId Command id for command handler
	 * @param aFileName resource file
	 */
	void ConstructL(const TDesC& aFileName);

	/**
	 * Constructor
	 * @param aMediaListProvider The owner of the media list to use
	 */
	CGlxCommandHandlerDetails(MGlxMediaListProvider* aMediaListProvider);
		
	private: // From CGlxMediaListCommandHandler

    	/** See CGlxCommandHandler::DoActivateL */
	    virtual void DoActivateL(TInt aViewId);
		
		/**
	     * Execute the command, if applicable. Called after basic filtering
    	 * @param aCommand The command to handle
	     * @return ETrue if command was handled
    	 */
		virtual TBool DoExecuteL( TInt aCommandId , MGlxMediaList& aList);

	    /** @see CGlxMediaListCommandHandler::DoIsDisabled */
	    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
	    
	    /**
	     * It checks whether full screen thumbnail for 
	     * focus item is available or not
    	 * @param aList Medialist to use
    	 * @return ETrue if any full screen thumbnail available
	     */
	    TBool IsThumbnailAvailable(MGlxMediaList& aList) const;
	
	private: // implementation
	
		void LaunchViewL(TUid aViewPluginId,const TDesC& aURI);

	private: //data

	    // Resource file offset
		TInt iResourceOffset;
		
		/// Ref: HUI utility
        CGlxUiUtility* iUiUtility;
	};

#endif // __C_GLXCOMMANDHANDLERDETAILS_H__

//End of file


