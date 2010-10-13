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
* Description:    Cloud view implementation
 *
*/




#ifndef C_GLXCLOUDVIEWIMP_H
#define C_GLXCLOUDVIEWIMP_H

// INCLUDES
#include <AknLayout2Def.h>
#include <alf/alfanchorlayout.h>
#include "mglxcloudviewmskobserver.h"
#include "mglxenterkeyeventobserver.h"

#include "glxcloudview.h"  //base class
#include "mglxcloudviewlayoutobserver.h"

#include "glxtagscontextmenucontrol.h"  // MGlxItemMenuObserver
#include "glxmmcnotifier.h"

// FORWARD DECLARATIONS
class CGlxCloudViewControl;


// CLASS DECLARATION

/**
 *  CGlxCloudViewImp
 *  Implementation class for Cloud control
 */
namespace Alf
{
class IAlfScrollBarWidget;
class IAlfScrollBarDefaultBaseElement;

 
NONSHARABLE_CLASS(CGlxCloudViewImp): public CGlxCloudView,
									public MGlxCloudViewMskObserver,				
									public MGlxEnterKeyEventObserver,
									public MGlxCloudViewLayoutObserver,
									public MGlxItemMenuObserver,
									public MStorageNotifierObserver
	{
public:

	/** 	
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aMediaListFactory - Handle to the medialist provider
	 *  @param aViewResId - Resource id of the View
	 *  @param aTitleResId - Resource id for the Title text
	 *  @param aEmptyListResId - Resource id for the empty listbox text
	 *  @param aSoftKeyResId - Resource id for the softkey texts       
	 *  @param aSoftkeyMskDisabledId -Respource id for Msk Disabled
	 *  @return Pointer to newly created object
	 */
	IMPORT_C static CGlxCloudViewImp *NewL(MGlxMediaListFactory
			*aMediaListFactory, const TDesC &aFileName, TInt aViewResId, 
			TInt aEmptyListResId, TInt aSoftKeyResId, TInt aSoftkeyMskDisabledId);

	/** 	
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aMediaListFactory - Handle to the medialist provider
	 *  @param aViewResId - Resource id of the View
	 *  @param aTitleResId - Resource id for the Title text
	 *  @param aEmptyListResId - Resource id for the empty listbox text
	 *  @param aSoftKeyResId - Resource id for the softkey texts
	 *  @param aSoftkeyMskDisabledId -Respource id for Msk Disabled
	 *  @return Pointer to newly created object
	 */
	IMPORT_C static CGlxCloudViewImp *NewLC(MGlxMediaListFactory
			*aMediaListFactory, const TDesC &aFileName, TInt aViewResId,
			 TInt aEmptyListResId, TInt aSoftKeyResId, TInt aSoftkeyMskDisabledId);

	/**
	 * Destructor.
	 */
	virtual ~CGlxCloudViewImp();

public: // MGlxCloudViewMskObserver

    /**
     * Called when msk status change is required
     * @param aDirection The direction of the focus change
     */
    void HandleMskChangedL(TBool aMskEnabled);
public: //  MGlxEnterKeyEventObserver   
    /**
     * Called when Key Event is handled.
     */
    
    void HandleEnterKeyEventL(TInt aCommand);
    
public: 
		//  MGlxCloudViewLayoutObserver   
    /**
     * Called when layout is changed
     */
	void HandleLayoutChanged();
public:
	//MGlxItemMenuObserver
	/**
	 * To Handle menu items in Grid layout
	 * @param aCommand command to be handled
	 */
	void HandleGridMenuListL(TInt aCommand);
public:    
    void HandleForegroundEventL(TBool aForeground); 
	
public:
   /**
     * Derived classes should use this to initialize the menus if required
     * @param aResourceId The menu resource
     * @param aMenuPane The menu pane to edit
     */
    void ViewDynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);

protected:
	// From CGlxViewBase

	/** 	
	 *  @since S60 3.2
	 *  @param aCommand command to be handled
	 */
	virtual TBool HandleViewCommandL(TInt aCommand);
	
	
private:

	/** 	
	 *  @function C++ default constructor
	 *  @since S60 3.2
	 */
	CGlxCloudViewImp(TInt aSoftKeyResId,TInt aSoftkeyMskDisabledId);

	/** 	
	 *  @function ControlGroupId
	 *  @since S60 3.2
	 *  @returns Control group id
	 */
	TInt ControlGroupId()const;

	/** 	
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aMediaListFactory - Handle to the medialist provider
	 *  @param aViewResId - Resource id of the View
	 *  @param aTitleResId - Resource id for the Title text
	 *  @param aEmptyListResId - Resource id for the empty listbox text
	 *  @param aSoftKeyResId - Resource id for the softkey texts
	 */
	void ConstructL(MGlxMediaListFactory *aMediaListFactory, const TDesC
			&aFileName, TInt aViewResId, TInt aEmptyListResId, TInt aSoftKeyResId);

	// from base class CAknView

	/** 
	 *  @since S60 3.2
	 *  Returns the unique identifier of the view
	 *  @return Unique identifier
	 */
	TUid Id()const;

	/**
	 *  @since S60 3.2
	 *  This will be called when the view is activated
	 *  @param aPrevViewId Specifies the view previously active.
	 *  @param aCustomMessageId Specifies the message type.
	 *  @param aCustomMessage The activation message.
	 */
	void DoMLViewActivateL(const TVwsViewId &aPrevViewId, TUid
			aCustomMessageId, const TDesC8 &aCustomMessage);

	/**
	 *  @since S60 3.2
	 *  This will be called when the view is deactivated
	 */
	void DoMLViewDeactivate();

	/**
	 * Cleanup the visuals: called after deactivate animation is complete,
	 * and when the view is destroyed
	 */
    void CleanupVisuals();

private:

//added for scrollbar widget implementation
    void ConstructCloudControlL();
    
    //set the rect for scrollbar
    void SetScrollBarRect();

private:
    /**
     * handle mmc insertion event
     */
    void HandleMMCInsertionL();
    /**
     * handle mmc removal event
     */
    void HandleMMCRemovalL();

private:
	// Data
	/**used for resource fetching */
	TInt iResourceOffset;
	
	/**For Cloud View Control */
	CGlxCloudViewControl *iCloudControl;
	
	/**variable for Alf display*/
	CAlfDisplay* iDisplay;
	
	/** Empty listbox text */
	HBufC* iEmptyListText;
	
	/** The animation being applied to the view */
	MGlxAnimation* iViewAnimation;
		
	/**Resource id for MSk */
	TInt iSoftKeyResId;
	
	/**Resource id for MSk Disabled */
	TInt iSoftkeyMskDisabledId;
	
	//view widget
	IAlfViewWidget *iViewWidget; 
	
	//scrollbar widget
	IAlfScrollBarWidget* iScrollWidget;
	
	IAlfScrollBarDefaultBaseElement* iScrollbarDefaultBaseElement;
	
	//scrollpane layout
	TAknWindowLineLayout iScrollPaneHandle;
	
	//anchor layout from the view widget
	CAlfAnchorLayout* iAnchorlayout;

	//mmc notifier
	CGlxMMCNotifier* iMMCNotifier;
	
	TBool iMMCState;
	};
}
#endif // C_GLXTESTTILEVIEWIMP_H

// End of File
