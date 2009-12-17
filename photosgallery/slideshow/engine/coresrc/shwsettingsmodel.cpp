/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The settings model for the slideshow
 *
*/



  

//  CLASS HEADER
#include "shwsettingsmodel.h"

//  EXTERNAL INCLUDES
#include <centralrepository.h>

//  INTERNAL INCLUDES
#include <glxlog.h>
#include <glxtracer.h>
#include "shwconstants.hrh"
#include "shwconstants.h"					

// LOCAL CONSTANTS NAMESPACE
namespace
	{
	// CenRep Key Ids
    const TUint32 KShwMusicOnOffKey 	= 0x00000001;
    const TUint32 KShwMusicNamePathKey 	= 0x00000002;
    const TUint32 KShwTransitionDelayKey= 0x00000003;
    const TUint32 KShwTransitionTypeKey	= 0x00000004;
    const TUint32 KShwTransitionIndexKey= 0x00000005;
    const TUint32 KShwPlayOrder			= 0x00000006;
	}

/**
 *  CShwCenRepWatcher
 *  CShwSettingsModel containted class for observing changes in central 
 *					 repository values
 * @internal reviewed 08/06/2007 by Kimmo Hoikka
 */
NONSHARABLE_CLASS( CShwSettingsModel::CShwCenRepWatcher )
	: public CActive
	{
	public:  // Constructors and destructor

	    /**
	     * Symbian Constructor.
	     * @param aRepositoryUid uid of Central Repository file
	     * @param aId id of the Central Repository key
	     * @param aObserver obersver - only supplied when the owner is 
	     *  interested in changes in key value
	     * @return contructed object
	     */
	    static CShwCenRepWatcher* NewL(const TUid& aRepositoryUid,
	                     		       TUint32 aId );
        /**
         * Destructor.
         */
        ~CShwCenRepWatcher();
	    
	public: // API

        /**
         * Get current value as integer.
         * @return Current value of the key as a TInt
         */
        TInt KeyValueL() const;

        /**
         * Get current value as 16 bit descriptor.
         * @param return value of the key in the descriptor
         */
        void KeyValueL(TDes& aKeyValue) const;

        /**
         * Set integer value
         * @param aNewValue New value for the key
         */
        void SetKeyValueL(const TInt aNewValue);

        /**
         * Set string value
         * @param aNewValue New 16 bit descriptor value for the key
         */
        void SetKeyValueL(const TDesC& aNewValue);

	private: // Implementation

        /**
         * C++ constructor.
         * @param aRepositoryUid uid of CenRep file
         * @param aId id of the key
         * @param aObserver obersver
         * @return contructed object
         */
        CShwCenRepWatcher(const TUid& aRepositoryUid, TUint32 aId );

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

	private: // Implementation

        // Own: the central repository API
        CRepository* iRepository;       
        // the central repository identifier
        TUid iRepositoryUid;
        // the key id
        TUint32 iId;
	    
	}; 

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
CShwSettingsModel::CShwCenRepWatcher* CShwSettingsModel::CShwCenRepWatcher::NewL
										(const TUid& aRepositoryUid,
                                        TUint32 aId )
    {
    TRACER("CShwSettingsModel::CShwCenRepWatcher::NewL");
	GLX_LOG_INFO("CShwSettingsModel::CShwCenRepWatcher::NewL");
			    
    CShwCenRepWatcher* self = 
    	new(ELeave) CShwCenRepWatcher( aRepositoryUid, aId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

inline CShwSettingsModel::CShwCenRepWatcher::CShwCenRepWatcher(
		const TUid& aRepositoryUid, TUint32 aId )
    : CActive(EPriorityStandard), 
    iRepositoryUid(aRepositoryUid), 
    iId(aId)
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor 
// ----------------------------------------------------------------------------
void CShwSettingsModel::CShwCenRepWatcher::ConstructL()
    {
    TRACER("CShwSettingsModel::CShwCenRepWatcher::ConstructL");
    GLX_LOG_INFO("CShwSettingsModel::CShwCenRepWatcher::ConstructL");
    iRepository = CRepository::NewL(iRepositoryUid);
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CShwSettingsModel::CShwCenRepWatcher::~CShwCenRepWatcher()
    {
    TRACER("CShwSettingsModel::CShwCenRepWatcher::~CShwCenRepWatcher");
	GLX_LOG_INFO("CShwSettingsModel::CShwCenRepWatcher::~CShwCenRepWatcher");
    Cancel();
    delete iRepository;
    }

// ----------------------------------------------------------------------------
// CShwCenRepWatcher::KeyValueL
// ----------------------------------------------------------------------------
TInt CShwSettingsModel::CShwCenRepWatcher::KeyValueL() const
    {
    TRACER("CShwSettingsModel::CShwCenRepWatcher::KeyValueL() const");
	GLX_LOG_INFO("CShwSettingsModel::CShwCenRepWatcher::KeyValueL() const");
    TInt retVal = KErrNone;
    User::LeaveIfError( iRepository->Get(iId, retVal) );
    return retVal;
    }
    
// ----------------------------------------------------------------------------
// CShwCenRepWatcher::KeyValueL
// ----------------------------------------------------------------------------
void CShwSettingsModel::CShwCenRepWatcher::KeyValueL(TDes& aKeyValue) const
    {
    TRACER("CShwSettingsModel::CShwCenRepWatcher::KeyValueL");
	GLX_LOG_INFO("CShwSettingsModel::CShwCenRepWatcher::KeyValueL");
    User::LeaveIfError( iRepository->Get(iId, aKeyValue) );
    }

// ----------------------------------------------------------------------------
// CShwCenRepWatcher::SetKeyValueL
// ----------------------------------------------------------------------------
void CShwSettingsModel::CShwCenRepWatcher::SetKeyValueL(const TInt aNewValue)
    {
    TRACER("CShwSettingsModel::CShwCenRepWatcher::SetKeyValueL(const TInt aNewValue)");
    GLX_LOG_INFO("CShwSettingsModel::CShwCenRepWatcher::SetKeyValueL(const TInt aNewValue)");
    User::LeaveIfError( iRepository->Set(iId, aNewValue) );
    }    

// ----------------------------------------------------------------------------
// CShwCenRepWatcher::SetKeyValueL
// ----------------------------------------------------------------------------
 void CShwSettingsModel::CShwCenRepWatcher::SetKeyValueL(const TDesC& aNewValue)
    {
    TRACER("CShwSettingsModel::CShwCenRepWatcher::SetKeyValueL");
    GLX_LOG_INFO("CShwSettingsModel::CShwCenRepWatcher::SetKeyValueL");
    User::LeaveIfError( iRepository->Set(iId, aNewValue) );
    }

// ----------------------------------------------------------------------------
// CShwCenRepWatcher::RunL
// ----------------------------------------------------------------------------
void CShwSettingsModel::CShwCenRepWatcher::RunL()
    {
    TRACER("CShwSettingsModel::CShwCenRepWatcher::SetKeyValueL");
    GLX_LOG_INFO("CShwSettingsModel::CShwCenRepWatcher::RunL");    
    User::LeaveIfError(iRepository->NotifyRequest(iId, iStatus));
    SetActive();
    }

// ----------------------------------------------------------------------------
// CShwCenRepWatcher::DoCancel
// ----------------------------------------------------------------------------
void CShwSettingsModel::CShwCenRepWatcher::DoCancel()
    {
    TRACER("CShwSettingsModel::CShwCenRepWatcher::DoCancel");
    GLX_LOG_INFO("CShwSettingsModel::CShwCenRepWatcher::DoCancel");
    iRepository->NotifyCancel( iId );
    }

// ----------------------------------------------------------------------------
// Symbian 2 phase constructor 
// ----------------------------------------------------------------------------
EXPORT_C CShwSettingsModel* CShwSettingsModel::NewL()
	{
	TRACER("CShwSettingsModel::NewL");
	GLX_LOG_INFO("CShwSettingsModel::NewL");

    CShwSettingsModel* self = new(ELeave) CShwSettingsModel();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
	}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CShwSettingsModel::~CShwSettingsModel()
	{
	TRACER("CShwSettingsModel::~CShwSettingsModel");
	GLX_LOG_INFO("CShwSettingsModel::~CShwSettingsModel");
	delete iPlayOrder;
	delete iMusicOnOffWatcher;
	delete iMusicNamePathWatcher;
	delete iTransDelayWatcher;
	delete iTransTypeWatcher;
	delete iTransIndexWatcher;
	}
   
// ----------------------------------------------------------------------------
// C++ default constructor 
// ----------------------------------------------------------------------------
inline CShwSettingsModel::CShwSettingsModel()
	{
	}

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
void CShwSettingsModel::ConstructL()
	{
	TRACER("CShwSettingsModel::ConstructL");
	GLX_LOG_INFO("CShwSettingsModel::ConstructL");
	const TInt KEngineId = 0x200071D3;	
	const TUid KEngineUid = TUid::Uid( KEngineId );
	iPlayOrder			 = CShwCenRepWatcher::NewL(KEngineUid,
							KShwPlayOrder );
	iMusicOnOffWatcher    = CShwCenRepWatcher::NewL(KEngineUid, 
							KShwMusicOnOffKey );
	iMusicNamePathWatcher = CShwCenRepWatcher::NewL(KEngineUid, 
							KShwMusicNamePathKey );
	iTransDelayWatcher    = CShwCenRepWatcher::NewL(KEngineUid, 
							KShwTransitionDelayKey );
	iTransTypeWatcher     = CShwCenRepWatcher::NewL(KEngineUid, 
							KShwTransitionTypeKey );
	iTransIndexWatcher    = CShwCenRepWatcher::NewL(KEngineUid, 
							KShwTransitionIndexKey );
	}
	
// ----------------------------------------------------------------------------
// CShwSettingsModel::PlayOrderL
// returns persisted play order
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CShwSettingsModel::PlayOrderL()
	{
	TRACER("CShwSettingsModel::PlayOrderL");
    GLX_LOG_INFO("CShwSettingsModel::PlayOrderL");
    TInt playState = iPlayOrder->KeyValueL();
    ASSERT(playState == 0 || playState == 1);
    return playState;
	}

// ----------------------------------------------------------------------------
// CShwSettingsModel::SavePlayOrderL
// Persist the play order state
// ----------------------------------------------------------------------------
//
EXPORT_C void CShwSettingsModel::SavePlayOrderL(TInt aPlayOrder)
	{
	TRACER("CShwSettingsModel::SavePlayOrderL");
	GLX_LOG_INFO("CShwSettingsModel::SaveMusicStateL");
   
    ASSERT(aPlayOrder == 0 || aPlayOrder == 1);
    iPlayOrder->SetKeyValueL(aPlayOrder);
	}
	
// ----------------------------------------------------------------------------
// CShwSettingsModel::SaveMusicOnOff
// Persist the music on/off state
// ----------------------------------------------------------------------------
EXPORT_C void CShwSettingsModel::SaveMusicStateL(TInt aMusicState)
    {
    TRACER("CShwSettingsModel::SaveMusicStateL");
	GLX_LOG_INFO("CShwSettingsModel::SaveMusicStateL");
   
    ASSERT(aMusicState == 0 || aMusicState == 1);
    iMusicOnOffWatcher->SetKeyValueL(aMusicState);
    }

// ----------------------------------------------------------------------------
// CShwSettingsModel::MusicOnL
// returns persisted the music on/off state
// ----------------------------------------------------------------------------
EXPORT_C TInt CShwSettingsModel::MusicOnL() const
    {
    TRACER("CShwSettingsModel::MusicOnL");
    GLX_LOG_INFO("CShwSettingsModel::MusicOnL");
    TInt musicState = iMusicOnOffWatcher->KeyValueL();
    ASSERT(musicState == 0 || musicState == 1);
    return musicState;
    }

// ----------------------------------------------------------------------------
// CShwSettingsModel::SaveMusicNamePathL
// Persist the name & path to the music file
// ----------------------------------------------------------------------------
EXPORT_C void CShwSettingsModel::SaveMusicNamePathL(const TDesC& aMusicNamePath)
    {
    TRACER("CShwSettingsModel::SaveMusicNamePathL");
    GLX_LOG_INFO("CShwSettingsModel::SaveMusicNamePathL");
    // allow setting also empty value (there used to be an if here...)
    iMusicNamePathWatcher->SetKeyValueL( aMusicNamePath );
    }
	
// ----------------------------------------------------------------------------
// CShwSettingsModel::MusicNamePathL
// returns persisted value of the music file name and path
// ----------------------------------------------------------------------------
EXPORT_C void CShwSettingsModel::MusicNamePathL(TDes& aMusicNamePath) const
    {
    TRACER("CShwSettingsModel::MusicNamePathL");
    GLX_LOG_INFO("CShwSettingsModel::MusicNamePathL");
    iMusicNamePathWatcher->KeyValueL(aMusicNamePath);
    }

// ----------------------------------------------------------------------------
// CShwSettingsModel::SaveTransDelayL
// Persist the value for transition delay between slide
// ----------------------------------------------------------------------------
EXPORT_C void CShwSettingsModel::SaveTransDelayL( TInt aTransDelay )
    {
    TRACER("CShwSettingsModel::SaveTransDelayL");
    GLX_LOG_INFO1("CShwSettingsModel::SaveTransDelayL %d", aTransDelay );
    // check we are inside bounds
    aTransDelay = Max( aTransDelay, KMinTransDelay );
    aTransDelay = Min( aTransDelay, KMaxTransDelay );
    iTransDelayWatcher->SetKeyValueL(aTransDelay);
    }

// ----------------------------------------------------------------------------
// CShwSettingsModel::TransDelayL
// returns persisted transition delay value
// ----------------------------------------------------------------------------
EXPORT_C TInt CShwSettingsModel::TransDelayL() const
    {
    TRACER("CShwSettingsModel::TransDelayL");
    TInt transDelay = iTransDelayWatcher->KeyValueL();
    GLX_LOG_INFO1( "CShwSettingsModel::TransDelayL %d", transDelay );
    return transDelay;
    }

// ----------------------------------------------------------------------------
// CShwSettingsModel::SaveTransitionType
// Persist the transition effect type ECOM UID and index
// ----------------------------------------------------------------------------
EXPORT_C void CShwSettingsModel::SaveTransitionTypeL(const TUid aEffectUid,
										   const TUint aEffectIndex)
    {
    TRACER("CShwSettingsModel::SaveTransitionTypeL");
    GLX_LOG_INFO("CShwSettingsModel::SaveTransitionTypeL");
	iTransTypeWatcher->SetKeyValueL(aEffectUid.iUid);
	iTransIndexWatcher->SetKeyValueL(aEffectIndex);
    }


// ----------------------------------------------------------------------------
// CShwSettingsModel::TransitionTypeL
// returns persisted transition effect type ECOM UID and 
// index
// ----------------------------------------------------------------------------
EXPORT_C void CShwSettingsModel::TransitionTypeL(TUid& aEffectUid, 	
													TUint& aEffectIndex) const
    {
    TRACER("CShwSettingsModel::TransitionTypeL");
    GLX_LOG_INFO("CShwSettingsModel::TransitionTypeL");
	aEffectUid.iUid = iTransTypeWatcher->KeyValueL();
	aEffectIndex 	= iTransIndexWatcher->KeyValueL();
    }
    
// End of File




