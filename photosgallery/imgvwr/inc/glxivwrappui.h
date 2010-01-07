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




#ifndef C_GLXIVIEWERAPPUI_H
#define C_GLXIVIEWERAPPUI_H

#include <aknViewAppUi.h>
#include <mpxcollectionobserver.h>
#include <mglxnavigationalstateobserver.h>
#include <mpxcollectionpath.h>
#include "glxiadupdate.h"

class MMPXViewUtility;
class MMPXCollectionUtility;
class CGlxUiUtility;
class CGlxNavigationalState;
class CAknGlobalNote;

class CGlxBackServiceWrapper;

/**
 *  CGlxIVwrAppUi
 *
 *  @lib ViewerApplication
 */

class CGlxIVwrAppUi : public CAknViewAppUi, public MGlxNavigationalStateObserver
    {
public:
    CGlxIVwrAppUi();
    void ConstructL();
    ~CGlxIVwrAppUi();

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

private:    // From CEikAppUi

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
     * close photos app.
     */
    void CloseImgVwr();

private:
    MMPXViewUtility* iViewUtility;

	/// Singleton that stores gallery's navigational state
    CGlxNavigationalState* iNavigationalState;

    /** Optional view plugin UID to activate when the collection is opened */
    TUid iStartupViewUid;

    /** Optional activation parameter for the next view */
    HBufC* iActivationParam;


    TBool iStartUpSequence;

    ///Ui utility
    CGlxUiUtility* iUiUtility;

    CGlxBackServiceWrapper* iBSWrapper;
    };


#endif // C_GLXIVIEWERAPPUI_H

