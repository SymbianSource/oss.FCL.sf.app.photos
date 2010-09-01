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
 * Description: Image Viewer AppUi class 
 *
 */




#ifndef C_GLXIVIEWERAPPUI_H
#define C_GLXIVIEWERAPPUI_H

#include <aknViewAppUi.h>
#include <mglxnavigationalstateobserver.h>
#include <mpxcollectionpath.h>
#include <mglxcache.h>

class MMPXViewUtility;
class CGlxUiUtility;
class CGlxNavigationalState;

/**
 *  CGlxIVwrAppUi
 *
 *  @lib ViewerApplication
 */

class CGlxIVwrAppUi : public CAknViewAppUi,
        public MGlxNavigationalStateObserver
    {
public:
    CGlxIVwrAppUi();
    void ConstructL();
    ~CGlxIVwrAppUi();
    /**
     * Open the viewer on receving OpenFileL() callback in CViewerDocument
     */
    void HandleOpenFileL();

public:
    // from CAknAppUi
    /**
     * From CAknAppUi
     * Handle user menu selections
     * @param aCommand Id of the command
     */
    void HandleCommandL(TInt aCommand);

    // From MGlxNavigationalStateObserver
    void HandleNavigationalStateChangedL();
    
    //OOM Method
    void HandleApplicationSpecificEventL(TInt aEventType,
            const TWsEvent& aWsEvent);

protected:
    // From CEikAppUi
    void OpenFileL(const TDesC& aFileName);

    TBool ProcessCommandParametersL(TApaCommand aCommand,
            TFileName& aDocumentName, const TDesC8& aTail);

private:
    enum TEntryType
        {
        EEntryTypeStartUp = 0,
        EEntryTypeFocusGained
        };
    /**
     * Get view scoring ids based on current navigational state
     * (using Get in the name since the function does not return anything)
     * @param aIns list that will be populated with scoring ids
     */
    void GetViewScoringIdsL(RArray<TUid>& aIds) const;

    /**
     * Get view scoring id for collection plugin
     * @param naviState Path to represent navigational state
     * @return scoding id
     */
    TUid GetViewScoringIdForCollectionPlugin(
            const CMPXCollectionPath& aNaviState) const;

    /**
     * Get view scoring id for depth in ui hierarchy
     * @param naviState Path to represent navigational state
     * @return scoding id
     */
    TUid ViewScoringIdForNaviStateDepth(
                    const CMPXCollectionPath& aNaviState) const;

    /**
     * Requesting OOM to Free Some Memory, so that photos image viewer can start
     * @return Error if Memory Cannot be freed else Error None
     */
    TInt OOMRequestFreeMemoryL(TInt aBytesRequested);

    /**
     * Finds Current Memory Availability And Decides to Send a Request for freeing the memory 
     * @param aCriticalMemoryRequired Bare Minimum to start photos image viewer
     * @return Error Code from OOM
     */
    TInt ReserveMemoryL(TInt aCriticalMemoryRequired);

    /**
     * Finds Current Minimum Required memory to start photos image viewer
     * @param Type of application invokation 
     * @return Required Critical Memory
     */
    TInt RamRequiredInBytesL(TEntryType aType);

    /**
     * Reserve critical memory qequired to start photos image viewer
     * @param Type of application invokation 
     */
    void ReserveMemoryL(TEntryType aType);

    /**
     * Close Image Viewer App.
     */
    void CloseImgVwr();

private:
    /// MPX View Utility
    MMPXViewUtility* iViewUtility;

    /// Image Viewer's Navigational State
    CGlxNavigationalState* iNavigationalState;

    /// Optional view plugin UID to activate when the collection is opened
    TUid iStartupViewUid;

    /// Ui Utility
    CGlxUiUtility* iUiUtility;
    };


#endif // C_GLXIVIEWERAPPUI_H

