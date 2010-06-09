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




#include "glxnavigationalstate.h"

#include <glxassert.h>
#include <glxsingletonstore.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxviewutility.h> // MMPXViewUtility
#include "mglxnavigationalstateobserver.h"
#include <eikappui.h>
#include <eikenv.h>

#include <glxtracer.h>

#include <glxlog.h>

 /**
 * CGlxNavigationalStateImp
 *
 * Implementation of navigational state
 * Note: function entry-exit logging is in CGlxNavigationalState, 
 * not in CGlxNavigationalStateImp.
 *
 * @author Aki Vanhatalo
 */
class CGlxNavigationalStateImp : public CBase, public MMPXCollectionObserver
    {
public:
    /** 
     * Second-phase constructor 
     * inlined since used only once from CGlxNavigationalState
     */
    inline void ConstructL()
        {
        iCollectionUtility = MMPXCollectionUtility::NewL( this );
        iViewUtility = MMPXViewUtility::UtilityL(); 
        iViewingMode = NGlxNavigationalState::EBrowse;
        }
        
    /** Destructor */
    ~CGlxNavigationalStateImp()
        {
        GLX_LOG_ENTRY_EXIT( "CGlxNavigationalStateImp::~CGlxNavigationalStateImp");
        if ( iCollectionUtility )
            {
            iCollectionUtility->Close();
            }
        
        if ( iViewUtility )
            {
            iViewUtility->Close();
            }
        // Log a warning if observers remaining; they should have been removed by the client
        __ASSERT_DEBUG( !iObservers.Count(), 
            GLX_LOG_WARNING1( "CGlxNavigationalStateImp - %d observers not removed", iObservers.Count() ) );

        iObservers.Close();
        }
    
    /**
     * Add an observer
     * inlined since used only once from CGlxNavigationalState
     * @param aObserver Observer of changes to state
     */
    inline void AddObserverL( MGlxNavigationalStateObserver& aObserver )
        {
        GLX_LOG_INFO1("CGlxNavigationalStateImp::AddObserverL %x", &aObserver);
        __ASSERT_DEBUG( KErrNotFound == iObservers.Find( &aObserver ), 
            GLX_LOG_WARNING( "CGlxNavigationalStateImp - Observer already added" ) );

        // Add the observer, unless already added
        if ( KErrNotFound == iObservers.Find( &aObserver ) )
            {
            iObservers.AppendL( &aObserver );
            }
        }
    
    /**
     * Remove an observer
     * inlined since used only once from CGlxNavigationalState
     * @param aObserver Observer to remove
     */
    inline void RemoveObserver( MGlxNavigationalStateObserver& aObserver )
        {
        GLX_LOG_INFO1("CGlxNavigationalStateImp::RemoveObserver %x", &aObserver);

        // Remove the observer if exists
        TInt index = iObservers.Find( &aObserver );
        if ( KErrNotFound != index )
            {
            iObservers.Remove( index );
            }

        __ASSERT_DEBUG( KErrNotFound != index, 
            GLX_LOG_WARNING( "CGlxNavigationalStateImp - No such observer to remove" ) );
        }

    /**
     * @return the current navigational state. Caller gets ownership.
     */
    CMPXCollectionPath* StateLC() const 
        {
        GLX_LOG_ENTRY_EXIT("CGlxNavigationalStateImp::StateLC");
        
        // state is stored in the collection utility as a collection path
        CMPXCollectionPath* state = Collection().PathL();
        CleanupStack::PushL( state );
        
        // go up one level in hierarchy, since the path contains also the 
        // currently open level, not only the nodes to the level
        state->Back();
        
        return state;
        }
    
    /**
     * Navigate back in the UI hierarchy
     * Does nothing, if currently at root.
     */
    inline void NavigateToParentL()
        {
        TRACER("CGlxNavigationalStateImp::NavigateToParentL()");
        if ( ViewingMode()== NGlxNavigationalState::EView )
            {
            GLX_LOG_INFO("CGlxNavigationalStateImp::NavigateToParentL() -1");
            iViewingMode = NGlxNavigationalState::EBrowse;
            NotifyObserversOfStateChange();
            }
        else 
            {
            GLX_LOG_INFO("CGlxNavigationalStateImp::NavigateToParentL() -2");
            Collection().BackL();
            }
        }
    
    /**
     * Navigate forward to provided item
     * @param aItemToOpen Id of item to navigate to
     */
    void NavigateToChildL( const TGlxMediaId& aItemIdToOpen )
        {
        // there is no way to open MPX collection utility's current state with 
        // and id of a child item, so create a full path object and add the 
        // requested item's id as the new level
        iViewingMode = NGlxNavigationalState::EBrowse;
        CMPXCollectionPath* state = StateLC();
        state->AppendL( aItemIdToOpen.Value() );
        
        NavigateToL( *state );
        
        CleanupStack::PopAndDestroy( state );      
        }
    
    /**
     * Navigate to provided position in hierarchy
     * @param aNewState Path object which describes the new position in hierarchy
     */
    inline void NavigateToL( const CMPXCollectionPath& aNewState )
        {
        // (it does not matter that the root level is opened without 
        // gallery plugin filter (EGlxCollectionPluginGallery), as long as 
        // the root level is *rendered* with something that uses the filter, 
        // such as a media list)
        if(!iIsNavigating)
            {
        iViewingMode = NGlxNavigationalState::EBrowse;
        Collection().OpenL( aNewState );
            iIsNavigating = ETrue;
            }
        }
       
    // commented off by gopakumar , as this is no longer needed .    
    /**
    * Set the ID of the view that will be activated if there are no other
    * view's in the view history
    */
    /*inline void SetFirstViewId( TVwsViewId aViewId )
        {
        // Save the view Id
        iViewId = aViewId;
        }*/

    /**
    * Activate previous view
    */
    inline void ActivatePreviousViewL()
        {
        if (!iIsNavigating)
            {
            iViewingMode = NGlxNavigationalState::EBrowse;
            //go back one view
            if (iViewUtility->ViewHistoryDepth() > 1)
                {
                iViewUtility->ActivatePreviousViewL();
                }
            }
        }
     

    // From MMPXCollectionObserver
    void HandleCollectionMessageL( const CMPXMessage& aMessage )
        {
        TRACER( "CGlxNavigationalStateImp::HandleCollectionMessageL" );
        
        if ( IsPathChangedMessage( aMessage ) )
            {
            NotifyObserversOfStateChange();
            iIsNavigating = EFalse;
            }
        }
        
    // From MMPXCollectionObserver
    void HandleOpenL( const CMPXMedia& /*aEntries*/, TInt /*aIndex*/, 
            TBool /*aComplete*/, TInt /*aError*/ )
        {
        // do nothing
        }
        
    // From MMPXCollectionObserver
    void HandleOpenL( const CMPXCollectionPlaylist& /*aPlaylist*/, TInt /*aError*/ )    
        {
        // do nothing
        }
        
    // From MMPXCollectionObserver
    void HandleCollectionMediaL( const CMPXMedia& /*aMedia*/, TInt /*aError*/ )
        {
        // do nothing
        }
    void SetToViewMode()
        {
         iViewingMode = NGlxNavigationalState::EView;
         // inform the observers( only appui ) that the mode is view
         NotifyObserversOfStateChange();
         
        }
    NGlxNavigationalState::TViewingMode ViewingMode()
        {
         return iViewingMode;
        }

private:
    /** Notify observers of state change */
    inline void NotifyObserversOfStateChange()
        {
        GLX_LOG_ENTRY_EXIT( "CGlxNavigationalStateImp::NotifyObserversOfStateChange" );
        // iObservers::Count() used in loop: ok, since unlikely to have many observers
        for ( TInt i = 0; i < iObservers.Count(); i++ ) 
            {
            iObservers[ i ]->HandleNavigationalStateChangedL();
            }
        }
        
    /** @return MPX collection */
    inline MMPXCollection& Collection() const
        {
        return iCollectionUtility->Collection();
        }

    /**
     * @param aMessage message to test
     * @return ETrue if the message is a "path changed" message 
     */
    inline TBool IsPathChangedMessage( const CMPXMessage& aMessage )
        {
        // message is a "path changed" message if message id is KMPXMessageGeneral
        // and KMPXMessageGeneralEvent attribute is EPathChanged
        return ( aMessage.IsSupported(KMPXMessageGeneralId ) 
              && KMPXMessageGeneral == aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralId ) 
              && aMessage.IsSupported( KMPXMessageGeneralEvent )
              && TMPXCollectionMessage::EPathChanged == aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        }
    
private:
    /// Collection utility that stores the path to represent current navigational
    /// state
    MMPXCollectionUtility* iCollectionUtility;
    MMPXViewUtility*        iViewUtility; 
    NGlxNavigationalState::TViewingMode  iViewingMode; 
    /// List of observers
    RPointerArray< MGlxNavigationalStateObserver > iObservers;
    
    TBool iIsNavigating; // If ETrue it means Navigating is already happening, so not allowed to navigate before current navigation Completes.
                         // If EFalse it means Navigation can happen   
    
    };
    
// -----------------------------------------------------------------------------
// InstanceL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxNavigationalState* CGlxNavigationalState::InstanceL()
    {
    GLX_LOG_INFO( "CGlxNavigationalState::InstanceL" );
    return CGlxSingletonStore::InstanceL( &NewL );
    }
    
// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxNavigationalState* CGlxNavigationalState::NewL()
    {
    GLX_LOG_INFO( "CGlxNavigationalState::NewL" );
    CGlxNavigationalState* self = new ( ELeave ) CGlxNavigationalState;
    CleanupStack::PushL( self );

    // Construct implementation object
    self->iImp = new ( ELeave ) CGlxNavigationalStateImp;
    self->iImp->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }
    

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxNavigationalState::~CGlxNavigationalState()
    {
    delete iImp;
    }

// -----------------------------------------------------------------------------
// Close a reference
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxNavigationalState::Close()
    {
    GLX_LOG_ENTRY_EXIT( "CGlxNavigationalState::Close");

    // Decrease reference count, and delete if last one
    CGlxSingletonStore::Close( this );
    // Member variable access not safe after CGlxSingletonStore::Close()
    }

// -----------------------------------------------------------------------------
// Close a reference
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxNavigationalState::AddObserverL(  
        MGlxNavigationalStateObserver& aObserver )
    {
    GLX_LOG_INFO1( "CGlxNavigationalState::AddObserverL %x", &aObserver );
    iImp->AddObserverL( aObserver );
    }
    
// -----------------------------------------------------------------------------
// Close a reference
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxNavigationalState::RemoveObserver( 
        MGlxNavigationalStateObserver& aObserver )
    {
    GLX_LOG_INFO1( "CGlxNavigationalState::RemoveObserver %x", &aObserver );
    iImp->RemoveObserver( aObserver );
    }

// -----------------------------------------------------------------------------
// Close a reference
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXCollectionPath* CGlxNavigationalState::StateLC() const
    {
    GLX_LOG_ENTRY_EXIT( "CGlxNavigationalState::StateLC" );
    return iImp->StateLC();
    }
    
// -----------------------------------------------------------------------------
// Navigate to parent item
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxNavigationalState::NavigateToParentL()
    {
    GLX_LOG_ENTRY_EXIT( "CGlxNavigationalState::NavigateToParentL" );
    iImp->NavigateToParentL();
    }
    
// -----------------------------------------------------------------------------
// Navigate to a child item
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxNavigationalState::NavigateToChildL( 
        const TGlxMediaId& aItemIdToOpen )
    {
   // GLX_LOG_INFO1( "CGlxNavigationalState::NavigateToChildL %d", aItemIdToOpen.Value() );
    iImp->NavigateToChildL( aItemIdToOpen );
    }
    
// -----------------------------------------------------------------------------
// Navigate to a path
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxNavigationalState::NavigateToL( const CMPXCollectionPath& aState )
    {
    GLX_LOG_ENTRY_EXIT( "CGlxNavigationalState::NavigateToL");
    iImp->NavigateToL( aState );
    }
    
// -----------------------------------------------------------------------------
// Activate previous view
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxNavigationalState::ActivatePreviousViewL() 
    {
    GLX_LOG_ENTRY_EXIT( "CGlxNavigationalState::ActivatePreviousViewL");
    iImp->ActivatePreviousViewL();
    }

// -----------------------------------------------------------------------------
// Set the ViewingMode
// -----------------------------------------------------------------------------
//    
EXPORT_C void CGlxNavigationalState::SetToViewMode()
    {
    iImp->SetToViewMode();
    // inform the observers( only appui ) that the mode is view
    }

// -----------------------------------------------------------------------------
// Get the ViewingMode
// -----------------------------------------------------------------------------
//    
EXPORT_C  NGlxNavigationalState::TViewingMode CGlxNavigationalState::ViewingMode()
    {
    return iImp->ViewingMode();
    }
// -----------------------------------------------------------------------------
// Set the BackExitStatus
// -----------------------------------------------------------------------------
// 
EXPORT_C void CGlxNavigationalState::SetBackExitStatus(TBool aStatus)
    {
    iBackExitStatus = aStatus;
    }

// -----------------------------------------------------------------------------
// Get the BackExitStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxNavigationalState::BackExitStatus()
    {
    return iBackExitStatus;
    }

// -----------------------------------------------------------------------------
// Set the Starting navigation Level
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxNavigationalState::SetStartingLevel(TInt aLevel)
    {
    iStartingLevel = aLevel;
    }
    
// -----------------------------------------------------------------------------
// Get the Starting navigation Level
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxNavigationalState::StartingLevel()
    {
    return iStartingLevel;
    }

// EOF

