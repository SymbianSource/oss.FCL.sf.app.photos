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
* Description:    View Base 
*
*/




#ifndef C_GLXVIEWBASE_H
#define C_GLXVIEWBASE_H

// INCLUDES


#include <aknview.h>
#include  <akntitle.h>
#include <alf/alftimedvalue.h>
#include <alf/ialfviewwidget.h>
#include <glxanimationfactory.h>
#include "glxkeyeventreceiver.h"
#include <mglxanimationobserver.h>
#include <glxlog.h>
#include <akntoolbar.h>
#include <akntoolbarobserver.h>
#include "mglxsoftkeyhandler.h"
#include <mul/mulevent.h>
#include <alf/ialfwidgetfactory.h>
class CGlxCommandHandler;
class CGlxUiUtility;
class CAlfControlGroup;
class CGlxKeyMonitor;

// CLASS DECLARATION

/**
 *  View base
 *
 *  @lib glxviewbase.lib
 */
class CGlxViewBase : public CAknView, 
                     public MGlxAnimationObserver,
                     public MAknToolbarObserver
    {
public:
    /**
     * Constructor
     */
    IMPORT_C CGlxViewBase(TBool aSyncActivation = ETrue);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CGlxViewBase();

    /**
     * Add a command handler to the view (ownership transferred and the
     * command handler is deleted in the event of a leave).
     * @param aCommandHandler The command handler to add.
     */
    IMPORT_C void AddCommandHandlerL(CGlxCommandHandler* aCommandHandler);

	/**
	 * Enable and set a Title in screen furniture
	 * @param aTitleText The text to be displayed
	 * @param aAnimate Indicates if the change is to be animated
	 */
    IMPORT_C void SetTitleL(const TDesC& aTitleText);

	/**
	 * Disable a Title in screen furniture
	 * @param aAnimate Indicates if the change is to be animated
	 */
    IMPORT_C void DisableTitle();
	
	/**
     * Handles commands
	 * @param aCommand The command to respond to
	 * @return ETrue iff the command has been handled.
     */
    IMPORT_C virtual TInt DoHandleCommandL(TInt aCommand);    
    
	/**
     * Sets the grid tool.
	 * @param grid toolbar handle. 
     */
    IMPORT_C void SetGridToolBar(CAknToolbar* aToolbar);
    
	/**
     * Returns the current active toolbar
     * Needed as grid toolbar is created dynamically
     */
    IMPORT_C CAknToolbar* GetToolBar();

protected: // From MCoeView.

    /** 
     * See @ref MCoeView. 
     */
	IMPORT_C void ViewActivatedL(const TVwsViewId& aPrevViewId,
	                             TUid aCustomMessageId,
	                             const TDesC8& aCustomMessage);

    /** 
     * See @ref MCoeView
     */
	IMPORT_C void ViewDeactivated();


protected: // From MGlxAnimationObserver 
    IMPORT_C virtual void AnimationComplete(MGlxAnimation* aAnimation); 

protected:
    //From MAknToolbarObserver
    IMPORT_C void OfferToolbarEventL( TInt aCommand );
    
    IMPORT_C void SetToolbarObserver(MAknToolbarObserver* aObserver);
    
protected:
	/**
	 * IMPORTANT: Call this from the ConstructL of any derived class
	 */
	IMPORT_C void ViewBaseConstructL();

	/**
	 * Derived classes should use this to deal with any view-specific commands
	 * @param aCommand The command to respond to
	 * @return ETrue iff the command has been handled.
	 */
	IMPORT_C virtual TBool HandleViewCommandL(TInt aCommand);
	
	/**
	 * Derived classes should use this to initialize the menus if required
	 * @param aResourceId The menu resource
	 * @param aMenuPane The menu pane to edit
	 */
	IMPORT_C virtual void ViewDynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	
	/**
	 * This will be called on derived classes when the view is activated
     * @param aPrevViewId Specifies the view previously active.
     * @param aCustomMessageId Specifies the message type.
     * @param aCustomMessage The activation message.
	 */
	virtual void DoViewActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage) = 0;
	
	/**
	 * This will be called on derived classes when the view is deactivated
	 */
    virtual void DoViewDeactivate() = 0;

    /**
	 * This is called on derived classes before the options menu
	 * is opened. The derived classes should use it to ensure that 
	 * all required attributes have been retrieved in order to determine
	 * if the command should be displayed on the options menu and.
	 */
    IMPORT_C virtual void FetchAttributesL();    
	
    /**
   	 * This is called on derived classes before commands are executed.
   	 * The derived classes should use it to ensure that all the required
   	 * attributes have been tetrieved in order to execute the command.
   	 */
    IMPORT_C virtual void FetchAttributesForCommandL(TInt aCommand);
    
	/**
     * Report whether the given type of animation is supported by the class
     * @return ETrue iff the animaiton is implemented
     */
    IMPORT_C virtual TBool ViewAnimationSupported(TGlxViewswitchAnimation aType); 
    
    /**
     * Start the given type of view-switch animation
     * @param aType The type of animation (e.g. Entry / Exit)
     * @param aDirection The direction of the animation (e.g. Forwards / Backwards)
     */
    IMPORT_C virtual void DoViewAnimationL(TGlxViewswitchAnimation aType, TGlxNavigationDirection aDirection);
    
    /**
     * Sets the toolbar state after command execution.
     * @ return ETrue If item is a system item. Else EFalse 
     */
    IMPORT_C virtual void SetToolbarStateL();

    
private:	
	// From MEikMenuObserver
	IMPORT_C virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	
	// From MGlxUiCommandHandler
	IMPORT_C void HandleCommandL(TInt aCommand);

	// From CAknView
    IMPORT_C void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
    IMPORT_C void DoDeactivate();

private:

    /**
     * Initialize a viewswitch animation
     * @param aType The type of animation to start
     */
    void InitAnimationL(TGlxViewswitchAnimation aType); 
    
    /**
     * Cancel the current viewswitch animation, if any
     */
    void CancelViewAnimation(); 

    /**
     * Called when the view-switch animation is complete or interrupted
     * @param aType The type of animation (e.g. Entry / Exit)
     */
    void ViewAnimationComplete(TGlxViewswitchAnimation aType); 
    
    /**
     * Gets the title pane instance.
     */
    CAknTitlePane* GetTitlePaneL();

    /*
     * Set toolbar items dimmed.
     * @param aDimmed Flag for the toolbar items to be dimmed.
     */
    void SetToolbarItemsDimmed(TBool aDimmed);
    
protected:
	/**
	 * Optionally implemented by sub-classes to prepare command handlers.
	 * Called once for every command handler added to the view.
	 * @param aCommandHandler command handler added to the view.
	 */
    virtual void DoPrepareCommandHandlerL(CGlxCommandHandler* /*aCommandHandler*/) {};

protected:
	/// The list of command handlers owned by the view
	RPointerArray<CGlxCommandHandler> iCommandHandlerList;    

protected:
	CGlxUiUtility* iUiUtility; ///< The UiUtility in use
	
	/// The view animation time
    TInt iViewAnimationTime; 
private:
    /**
     * Functions to handle view activate asynchronously
     */
    static TBool ViewActivateCallbackL(TAny* aPtr);
    inline void ViewActivateL();

private:
    /// Resource file offset
    TInt iViewBaseResourceOffset;

    /// Ui Utility resource file offset
    TInt iUiUtilitiesResourceOffset;
    /// The type of viewswitch animation in progress, if any
    TGlxViewswitchAnimation iViewAnimationInProgress; 
    
    /// Flag to determine if this view is activated synchronously
    const TBool iSyncActivation;

    //This is a Kind of Hack to prevent Photos to act on any command's till 
    //Corresponding view is activated
    //This Should be fixed with Proper Way
    TBool iViewActivated;

    /// View activate attributes for callback
    TVwsViewId iPrevViewId;
    TUid iCustomMessageId;
    HBufC8* iCustomMessage;

    /// Callback for view activate
	CAsyncCallBack* iViewActivateCallback;
		
	/// Status pane for title
	CAknTitlePane* iTitlePane;
	//used store the grid toolbar
	CAknToolbar* iToolbar;
    };


#endif // C_GLXVIEWBASE_H
