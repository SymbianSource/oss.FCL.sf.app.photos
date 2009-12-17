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
* Description:    Thumbnail Composer  plugin
*
*/




/**
 * @internal reviewed 12/07/2007 by Simon Brooks
 */

#ifndef __CTHUMBNAILCOMPOSERPLUGIN_H__
#define __CTHUMBNAILCOMPOSERPLUGIN_H__

#include <ComposerPlugin.h>
#include <MdEObject.h>
#include <MpxCollectionObserver.h>
class MMPXCollectionUtility;
class CMPXCollectionPath;
class CGlxThumbnailComposerPlugin;

/**
 * Timer to allow inactivity delay if background generation task is cancelled
 */
class CGlxThumbnailCompeserInactivityTimer : public CTimer
    {
    public:
        ~CGlxThumbnailCompeserInactivityTimer();
        static CGlxThumbnailCompeserInactivityTimer* NewL(TInt aPriority, CGlxThumbnailComposerPlugin& aCallback);
        void After(TTimeIntervalMicroSeconds32 aInterval);
    private:
        CGlxThumbnailCompeserInactivityTimer(TInt aPriority, CGlxThumbnailComposerPlugin& aCallback);
    private: // from CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
    private:
        CGlxThumbnailComposerPlugin& iCallback;
    };


/**
 * Composer plugin to initiate background generation when new items are
 * harvested by the MDS harvester.
 *
 * @author David Holland
 */
NONSHARABLE_CLASS( CGlxThumbnailComposerPlugin ) :
                  public CComposerPlugin, MMPXCollectionObserver 
    {
    public:
        /**
        * Construction
        */
        static CGlxThumbnailComposerPlugin* NewL();

        /**
        * Destruction
        */
        ~CGlxThumbnailComposerPlugin();

    private:    // From CComposerPlugin

        TBool IsComposingComplete();
        void SetObservers();
        void RemoveObservers();

    private:    // From MMdEObjectObserver

#ifdef RD_MDS_2_0
        void HandleObjectAdded(CMdESession& aSession,
                                const RArray<TItemId>& aObjectIdArray);
        void HandleObjectModified(CMdESession& aSession,
                                const RArray<TItemId>& aObjectIdArray);
        void HandleObjectRemoved(CMdESession& aSession,
                                const RArray<TItemId>& aObjectIdArray);
#else
        /**
         * See @ref MMdEObjectObserver::HandleObjectNotification
         */
        void HandleObjectNotification(CMdESession& aSession, 
		    				TObserverNotificationType aType,
				    		const RArray<TItemId>& aObjectIdArray);
#endif

    private:    // From MMPXCollectionObserver

        void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );
        void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );
        void HandleOpenL( const CMPXMedia& aEntries,
                            TInt aIndex, TBool aComplete, TInt aError );
        void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,
                            TInt aError );
        void HandleCommandComplete( CMPXCommand* aCommandResult,
                                    TInt aError );

    private:

        /**
        * Private constructor
        */	
        CGlxThumbnailComposerPlugin();

        /**
        * 2nd phase construction
        */	
        void ConstructL();

        /**
        * Add observers to MdE session.
        */
        void SetObserversL();

        void DoHandleCollectionMessageL( const CMPXMessage& aMsg );

        /**
        * Send next media request to the collection.
        */
        void RequestItemL();

    public:
        /**
        * Open "all" collection with thumbnail loadability filter.
        */
        void DoOpenCollectionL();

        /**
        * Send next media request to the collection.
        */
        void DoRequestItemL();

        /**
        * Free resources and go to idle state.
        */
        void ReturnToIdle();

        /**
        * Delay display state check, if screen is on. 
        */
        void DelayDisplayStateCheck();
        
        /**
        * Callback for iCollectionCloser
        */
        void CloseCollection();

    public:
        /**
        * Enumeration of states for the plugin.
        */
        enum TComposerState
            {
            EStateIdle,
            EStateFirstOpening,
            EStateOpening,
            EStateActiveLarge,
            EStateActiveSmall,
            EStateCleanup,
            EStateClosing
            };

        /** Current state of the plugin */
        TComposerState iState;

    private:
        /** Isolated collection utility (owned) */
        MMPXCollectionUtility* iCollectionUtility;

        /** Path from collection (owned) */
        CMPXCollectionPath* iPath;

        /** Index of item to thumbnail */
        TInt iCurrentIndex;

        /** Timer to restart Background Harvesting once inactive */
        CGlxThumbnailCompeserInactivityTimer* iInactivityTimer;
        
        TInt iLastError;
    };

#endif // __CTHUMBNAILCOMPOSERPLUGIN_H__
