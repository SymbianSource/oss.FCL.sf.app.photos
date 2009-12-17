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
* Description:    Navigational state of the app
*
*/




#ifndef __C_GLXNAVIGATIONALSTATE_H__
#define __C_GLXNAVIGATIONALSTATE_H__

#include <e32std.h>

#include <glxmediaid.h>
#include <vwsdef.h>
#include <glxnavigationalstatedefs.h>

class CGlxNavigationalStateImp;
class CMPXCollectionPath;
class MGlxNavigationalStateObserver;

/**
 * CGlxNavigationalState
 *
 * Class that stores the navigational state of the application.
 * Navigational state means the currently viewed node in UI hierarchy
 *
 * Design: CGlxNavigationalState delegates all operations to 
 * CGlxNavigationalStateImp, to avoid client having to include mpx dependencies.
 * CGlxNavigationalStateImp stores the navigational state in MPX collection 
 * server by owning an MMPXCollectionUtility. 
 * 
 * @author Aki Vanhatalo
 */
class CGlxNavigationalState : public CBase
    {
private:

public:
    /**
     * Return a (singleton) instance of the class
     * Close the instance by calling Close() when done with the instance
     */
    IMPORT_C static CGlxNavigationalState* InstanceL(); 
    
    /**
     * Close the instance and remove observer if any
     * See @ref MGlxActiveMediaListResolver::Close
     * @param aObserver Observer to remove
     */
    IMPORT_C void Close();
    
    /**
     * Add an observer
     * @param aObserver Observer of changes to the state
     */
    IMPORT_C void AddObserverL( MGlxNavigationalStateObserver& aObserver );
    
    /**
     * Remove an observer
     * @param aObserver Observer to remove. 
     */
    IMPORT_C void RemoveObserver( MGlxNavigationalStateObserver& aObserver );

    /**
     * @return the current navigational state. Caller gets ownership.
     */
    IMPORT_C CMPXCollectionPath* StateLC() const;
    
    /**
     * Navigate back in the UI hierarchy
     * Does nothing, if currently at root.
     */
    IMPORT_C void NavigateToParentL();
    
    /**
     * Navigate forward to provided item
     * @param aItemToOpen Id of item to navigate to
     */
    IMPORT_C void NavigateToChildL( const TGlxMediaId& aItemIdToOpen );
    
    /**
     * Navigate to provided position in hierarchy
     * @param aNewState Path object which describes the new position in hierarchy
     */
    IMPORT_C void NavigateToL( const CMPXCollectionPath& aNewState );

     /**
     * Activate previous view
     */
    IMPORT_C void ActivatePreviousViewL();
     
     /**
     * returns the current viewing mode of the application
     */
     IMPORT_C NGlxNavigationalState::TViewingMode ViewingMode();
     
     /**
     * Sets the mode to "view"
     */
     IMPORT_C void SetToViewMode();

     IMPORT_C void SetBackExitStatus(TBool aStatus);
     
     IMPORT_C TBool BackExitStatus();
  
private:
    /** Constructor for singleton store */
    static CGlxNavigationalState* NewL();
    
    /** Destructor */
    ~CGlxNavigationalState();

private:
    /// Implementation 
    CGlxNavigationalStateImp* iImp;
    
public:
    TBool iBackExitStatus;
    };
    
    
#endif // __C_GLXNAVIGATIONALSTATE_H__