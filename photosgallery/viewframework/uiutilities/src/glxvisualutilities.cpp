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
* Description:    ALF visual utilities
*
*/




// CLASS HEADER
#include "glxvisualutilities.h"

// EXTERNAL HEADERS
#include <uiacceltk/huicontrol.h>
#include <uiacceltk/huivisual.h>

namespace NGlxVisualUtilities
    {
    /**
     * TGlxVisualTransferCleanup
     * This class handles the awkward situation in visual ownership transfer
     * that some of the visuals are transferred to new parent and a leave occurs.
     * In leave case we need to undo the ownership transfer completely.
     * This class also removes the ownership from the old control in case
     * there was no leave.
     * @usage:
     * \code
     *      RArray<CHuiVisual*> allvisuals;
     *      TGlxVisualTransferCleanup cleanup( allvisuals );
     *       // set the parent to remove from to be the new one
     *      cleanup.iParentToRemoveFrom = aNewParent;
     *      CleanupClosePushL( cleanup );
     *      // start to append the visuals to the new parent
     *      // in case of leave they are removed from new parent
     *      RecurseAndTransferVisualsL( visualcleanup, aVisual, aNewParent );
     *      // set the parent to remove to be the old one
     *      cleanup.iParentToRemoveFrom = oldParent;
     *      // remove the item from cleanupstack,
     *      // this removes the visuals from their old parent 
     *      CleanupStack::PopAndDestroy( &cleanup );
     * \endcode
     */
    NONSHARABLE_CLASS( TGlxVisualTransferCleanup )
    	{
    	public:

            /**
    		 * Constructor.
    		 * @param aArray, the array of Visuals 
    		 * @param aOldParent, the old parent of visuals
    		 * @param aNewParent, the new parent for visuals
    		 */
    		inline TGlxVisualTransferCleanup( 
                RArray<CHuiVisual*>& aArray )
                : iArray( aArray )
    			{
                }

            /**
             * Close. Put this class to Cleanupstack with 
             * CleanupClosePushL so that this gets called in case of a leave
             */
            inline void Close()
                {
                // need to remove all visuals from the given parent
                // loop through all the CHuiVisuals
                for( TInt i = 0; i < iArray.Count(); ++i )
                    {
                    // remove from the given parent
                    iParentToRemoveFrom->Remove( iArray[ i ] );
                    }
                // reset the array
                iArray.Reset();
                }

            /// Ref: the parent where to remove
            MHuiVisualOwner* iParentToRemoveFrom;

        private: // Implementation

            /// Ref: the array containing all the visuals
            RArray<CHuiVisual*>& iArray; 

    	};

    /**
     * Recursive helper method to transfer the ownership of visuals
     * @param aVisualCleanup an array containing all visuals in the hierarchy
     * @param aVisual the visual to move to new control
     * @param aNewParent the new parent for the visual
     */
    void RecurseAndTransferVisualsL(
        RArray<CHuiVisual*>& aVisualCleanup,
        CHuiVisual& aVisual, CHuiControl& aNewParent )
        {
        // make room to the pointer so that we always succeed in appending
        // Note that we need to make room for one new item, thus count + 1
        aVisualCleanup.ReserveL( aVisualCleanup.Count() + 1 );
        // append us to the new parent, Note that for a while we are
        // owner by two controls but thats not a problem as the other parent 
        // is removed by either TGlxVisualTransferCleanup.Close() by CleanupStack
        // or destructor of TGlxVisualTransferCleanup and there is no other way
        // of getting out from this method (of course panic is ;)
        aNewParent.AppendL( &aVisual );
        // add us in the cleanuparray so that we can be removed from the 
        // new parent in case some of the following AppendLs leave
        // this does not fail as reserve was called so no AppendL needed
        aVisualCleanup.Append( &aVisual );
        // check if the visual has childs
        TInt childCount = aVisual.Count();
        // transfer all the childs
        while( childCount-- > 0 )
            {
            // get the child
            CHuiVisual& childVisual = aVisual.Visual( childCount );
            // call transfer recursively on the child
            RecurseAndTransferVisualsL( 
                aVisualCleanup, childVisual, aNewParent );
            }
        }

    // -------------------------------------------------------------------------
	// TransferVisualsL
	// -------------------------------------------------------------------------
    EXPORT_C void TransferVisualsL( 
        CHuiVisual& aVisual, CHuiControl& aNewParent )
        {
        // check if parent is already correct
        if( &( aVisual.Owner() ) == 
            static_cast< MHuiVisualOwner* >( &aNewParent ) )
            {
            // nothing else to be done
            return;
            }
        // create an array for visual pointers on the stack
        RArray< CHuiVisual* > visualcleanup;
        // create the cleanup item from stack as well
        TGlxVisualTransferCleanup removeParent( visualcleanup );
        // set the parent to remove from to be the new one
        removeParent.iParentToRemoveFrom = &aNewParent;
        // need to remember the old parent
        MHuiVisualOwner* oldParent = &( aVisual.Owner() );
        // put it to cleanupstack so that close gets called in case of leave
        CleanupClosePushL( removeParent );
        // run the recursive loop, if it leaves the visuals are removed from
        // new parent by the Close method of cleanParents
        RecurseAndTransferVisualsL( visualcleanup, aVisual, aNewParent );
        // set the parent to remove to be the old one
        removeParent.iParentToRemoveFrom = oldParent;
        // remove the item from cleanupstack,
        // this removes the visuals from the old parent 
        CleanupStack::PopAndDestroy( &removeParent );
        // close the array
        visualcleanup.Close();
        }
    }
