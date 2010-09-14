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
* Description:    AppUi class 
*
*/




#ifndef C_GLXWERAPPUI_H
#define C_GLXWERAPPUI_H

#include <aknViewAppUi.h>
#include <mpxcollectionobserver.h>
#include <mglxnavigationalstateobserver.h>
#include <mpxcollectionpath.h>
#include "glxiadupdate.h"

class MMPXViewUtility;
class MMPXCollectionUtility;
class CGlxUiUtility;
class CRepository;
class CGlxNavigationalState;
class CAknGlobalNote;

/**
 *  CGlxAppUi
 *
 *  @lib ViewerApplication
 */

class CGlxAppUi : public CAknViewAppUi, public MGlxNavigationalStateObserver
	{
public:
	CGlxAppUi();
	void ConstructL();
	~CGlxAppUi();

public: // from CAknAppUi
	/**
	 * From CAknAppUi
     * Handle user menu selections
  	 * @param aCommand Id of the command
     */
	void HandleCommandL(TInt aCommand);

    // From MGlxNavigationalStateObserver
    void HandleNavigationalStateChangedL();
    //OOM Method
    void HandleApplicationSpecificEventL(TInt aEventType, const TWsEvent& aWsEvent);

    // Callback for periodic timer, static, 
    static TInt PeriodicCallback( TAny* aPtr );
    //nonstatic func called from periodic timer
    void PeriodicCallback();

private:    // From CEikAppUi
    MCoeMessageObserver::TMessageResponse HandleMessageL(
                            TUint32 aClientHandleOfTargetWindowGroup,
                            TUid aMessageUid, const TDesC8& aMessageParameters);
    TBool ProcessCommandParametersL(TApaCommand aCommand,
                            TFileName& aDocumentName, const TDesC8& aTail);

private: // From CCoeAppUi

    void HandleForegroundEventL( TBool aForeground );
 
private:
    enum TEntryType
        {
        EEntryTypeStartUp = 0,
        EEntryTypeFocusGained
        };
    

    /**
     * Handle an activation message.
     * @param aCommand command data sent from the activating application.
     * @param aDocumentName image file name data sent from the activating application.
     * @param aData Message data from activating application.
     */
    void HandleActivationMessageL(const TApaCommand& aCommand, const TFileName& aDocumentName, const TDesC8& aData);
	 /**
     * Handle an activation message.    
     * @param aData Message data from activating application.
     */
    void HandleActivationMessageL(const TDesC8& aData);
    /**
     * Set activation parameter for startup view, converting to 16-bit
     * descriptor used by MPX.
     * @param aParam Activation parameter.
     */
    void SetActivationParamL(const TDesC8& aParam);

    /**
     * Get view scoring ids based on current navigational state
     * (using Get in the name since the function does not return anything)
     * @param aIns list that will be populated with scoring ids
     */
    void GetViewScoringIdsL( RArray<TUid>& aIds ) const;

    /**
     * Get view scoring id for collection plugin
     * @param naviState Path to represent navigational state
     * @return scoding id
     */
    TUid GetViewScoringIdForCollectionPlugin( const CMPXCollectionPath& aNaviState ) const;

    /**
     * Get view scoring id for depth in ui hierarchy
     * @param naviState Path to represent navigational state
     * @return scoding id
     */
    TUid ViewScoringIdForNaviStateDepth( const CMPXCollectionPath& aNaviState ) const;

    /**
     * Requesting OOM to Free Some Memory, so that photos can Start
     * @return Error if Memory Cannot be freed else Error None
     */
    TInt OOMRequestFreeMemoryL( TInt aBytesRequested) ;

    /**
     * Finds Current Memory Availability And Decides to Send a Request for freeing the memory 
     * @param aCriticalMemoryRequired Bare Minimum to start photos 
     * @return Error Code from OOM
     */
    TInt ReserveMemoryL(TInt aCriticalMemoryRequired);
    
    /**
     * Finds Current Minimum Required memory to start photos 
     * @param Type of application invokation 
     * @return Required Critical Memory
     */
    TInt RamRequiredInBytesL(TEntryType aType);
    
    /**
     * @todo:
     * Get view scoring id for depth in ui hierarchy
     * @param naviState Path to represent navigational state
     * @return scoding id
     */
    void ReserveMemoryL(TEntryType aType);

    //OOM
    /**
     * Invoked by the OOM framework FreeRam event 
     * to free the memory for other applications.
     */
    void StartCleanupL();

    //OOM
    /**
     * Invoked by the OOM framework MemoryGood event 
     * to stop the memory clean-up operation.
     */
    void StopCleanupL();

    /**
     * Check for updates via IAD.
     */
    void DoCheckForIADUpdatesL();
    
    /**
     * close photos app.
     */
    void ClosePhotosL();

    /**
     * Open navigational state at root level
     */
    void NavigateToMainListL();

private:
    MMPXViewUtility* iViewUtility;
    
    CAknGlobalNote* iGlobalWaitNote;
    TInt iNoteId;
    HBufC* iFreeMemText;

    /// Singleton that stores gallery's navigational state
    CGlxNavigationalState* iNavigationalState;
    
    /** Optional view plugin UID to activate when the collection is opened */
    TUid iStartupViewUid;

    /** Optional activation parameter for the next view */
    HBufC* iActivationParam;
    
    TBool iFocusLostLowMemory;
    TBool iStartUpSequence;

    ///Ui utility
    CGlxUiUtility* iUiUtility;

    /// Central repository entry for app (owned)
    CRepository* iRepository;
    
    /**
     * IAD updater class
     */
    CGlxIadUpdate* iIadUpdate;

    /**
     * Timer to check for IAD updates 30 seconds after application startup.
     */
    CPeriodic* iPeriodic;

    /**
     * Flag to filter any spurious EPathChanged event from MPX Collection f/w.
     */
    TBool iStateChangeRequested;
    };

#endif // C_GLXWERAPPUI_H

