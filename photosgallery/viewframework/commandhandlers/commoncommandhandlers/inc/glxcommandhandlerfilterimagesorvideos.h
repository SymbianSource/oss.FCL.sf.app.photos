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
* Description:    Show items command handler
*
*/




#ifndef C_glxcommandhandlerfilterimagesorvideos_H
#define C_glxcommandhandlerfilterimagesorvideos_H

// EXTERNAL INCLUDES
#include <e32base.h>

// INTERAL INCLUDES
#include <glxmedialistcommandhandler.h>
#include <glxfiltergeneraldefs.h>

// FORWARD DECLARATIONS
class MGlxMediaListProvider;
class CGlxUiUtility;

/**
 * @class CGlxCommandHandlerFilterImagesOrVideos
 *
 * Command handler providing the options "Show All", "Show Images" and "Show Videos".
 * Only 2 of the 3 will ever be shown at one time
 * @author Rowland Cook
 * @author Kimmo Hoikka (Release 5.5 backport)
 * @internal reviewed 25/10/2007 by David Holland
 * @internal reviewed 08/02/2008 by Rhodri Byles
 */
NONSHARABLE_CLASS( CGlxCommandHandlerFilterImagesOrVideos )
    : public CGlxMediaListCommandHandler
	{
    public:

        /**
         * Two-phase constructor
    	 *
    	 * @param aMediaListProvider media list owner
    	 */
    	IMPORT_C static CGlxCommandHandlerFilterImagesOrVideos* NewL(
    	    MGlxMediaListProvider* aMediaListProvider );

    	/** Destructor */
    	IMPORT_C ~CGlxCommandHandlerFilterImagesOrVideos();

    private:

        /** Constructor */
    	CGlxCommandHandlerFilterImagesOrVideos(
    	    MGlxMediaListProvider* aMediaListProvider, 
    	    CGlxUiUtility& aUiUtility );
        /** 2nd phase Constructor */
    	void ConstructL();

    private: // From CGlxMediaListCommandHandler

        /// @ref CGlxMediaListCommandHandler
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /// @ref CGlxMediaListCommandHandler
        void DoActivateL( TInt aViewId );

        /// @ref CGlxMediaListCommandHandler
        TBool DoExecuteL( TInt aCommandId, MGlxMediaList& aList );

    private:

        /**
         * Sets the item type in the media list must use
         * @param the new item type
         */
        void SetItemTypeInMediaListL( TGlxFilterItemType aItemType );

    private: // data

        /// Ref: HuiUtility
        CGlxUiUtility& iUiUtility;

        // boolean to record what screen mode is used.
        TBool iInFullScreen;

	};

#endif // C_glxcommandhandlerfilterimagesorvideos_H
