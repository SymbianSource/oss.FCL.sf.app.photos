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
* Description:    Upload cenrep watcher
*
*/

#ifndef __GLXUPLOADCENREPWATCHER_H__
#define __GLXUPLOADCENREPWATCHER_H__


class CGlxCommandHandlerUpload;

/**
 *  CGlxUploadCenRepWatcher
 *  CGlxCommandHandlerUpload containted class for observing changes in central 
 *                   repository values
 */
NONSHARABLE_CLASS( CGlxUploadCenRepWatcher ) : public CActive
    {
    public:  // Constructors and destructor

        /**
         * Symbian Constructor.
         * @param aObserver obersver - only supplied when the owner is 
         *  interested in changes in key value
         * @param aRepositoryUid uid of Central Repository file
         * @param aId id of the Central Repository key
         * @return contructed object
         */
        static CGlxUploadCenRepWatcher* NewL(MGlxUploadIconObserver& aUploadIconObserver,
                                             const TUid& aRepositoryUid,
                                             TUint32 aId );
        /**
         * Destructor.
         */
        ~CGlxUploadCenRepWatcher();
        
    public: // API

        /**
         * Get current value as 16 bit descriptor.
         * @param return value of the key in the descriptor
         */
        void KeyValueL(TDes& aKeyValue) const;
 
    private: // Implementation

        /**
         * C++ constructor.
         * @param aObserver obersver		 
         * @param aRepositoryUid uid of CenRep file
         * @param aId id of the key
         * @return contructed object
         */
        CGlxUploadCenRepWatcher(MGlxUploadIconObserver& aUploadIconObserver,
                                const TUid& aRepositoryUid, TUint32 aId );

        /**
         * 2nd phase constructor
         */
        void ConstructL();

    protected: // from CActive

        /**
         * @ref CActive::RunL
         */
        void RunL();

        /**
         * @ref CActive::DoCancel
         */
        void DoCancel();
        
        /**
         * @ref CActive::RunError
         */
        TInt RunError( TInt aError );

    private: // Implementation
        // Not Owned: the upload icon observer
        MGlxUploadIconObserver& iUploadIconObserver;
        // Own: the central repository API
        CRepository* iRepository;       
        // the central repository identifier
        TUid iRepositoryUid;
        // the key id
        TUint32 iId;
        
    };

#endif //__GLXUPLOADCENREPWATCHER_H__
