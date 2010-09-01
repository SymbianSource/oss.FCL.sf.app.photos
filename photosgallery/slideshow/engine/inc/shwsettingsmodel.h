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




#ifndef __CSHWSETTINGSMDL_H__
#define __CSHWSETTINGSMDL_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATIONS

/**
 *  CShwSettingsModel
 *  Stores and reads user definable settings for the slideshow
 * @internal reviewed 08/06/2007 by Kimmo Hoikka
 */
NONSHARABLE_CLASS( CShwSettingsModel ) : public CBase
	{
	public:  // Constructors and destructor
	    /**
	    * Symbian two stage Constructor.
	    * @return contructed object
	    */
	    IMPORT_C static CShwSettingsModel* NewL();

	    /**
	    * Destructor.
	    */
	    IMPORT_C ~CShwSettingsModel();

	private: // member functions
		/**
		* C++ constructor.
		*/
		CShwSettingsModel();

		/**
		* 2nd stage constructor
		*/
		void ConstructL();	    

	public: // member funtions

   		
   		/**
        * Retrieve the persisted play order setting value.
        * @retrun the play order
        */
		IMPORT_C TInt PlayOrderL();
		
		/**
        * Persist the play order setting value.
        * @param [in] the play order to be persisted
        */
		IMPORT_C void SavePlayOrderL(TInt aOrder);
		
		/**
        * Persist the music on/off setting value.
        * @param [in] the music state to be persisted
        */
		IMPORT_C void SaveMusicStateL(TInt aMusicState);
   		
   		/**
        * Retrieve the persisted music on/off setting value.
        * @retrun the music on/off state
        */		
		IMPORT_C TInt MusicOnL() const;
		
		/**
        * Persist the music name and path value.
        * @param [in] the music name and path to be persisted
        */
		IMPORT_C void SaveMusicNamePathL(const TDesC& aMusicNamePath);
		
		/**
        * Retrieve the persisted music name and path.
        * @param [out] the music name and path
        */		
		IMPORT_C void MusicNamePathL(TDes& aMusicNamePath) const;

		/**
        * Persist the transition delay between images.
        * @param [in] the transition delay value to be persisted
        */
		IMPORT_C void SaveTransDelayL(TInt aTransDelay);

		/**
        * Retrieve the persisted transition delay between images.
        * @return the transition delay value
        */
		IMPORT_C TInt TransDelayL() const;
		
		/**
        * Persist the transition effect type between images.
        * @param [in] aEffectUid the transition effect type ECom Uid to be persisted
        * @param [in] aEffectIndex the transition effect index to be persisted
        * @ref TTransEffectType
        */
        
		IMPORT_C void SaveTransitionTypeL(const TUid aEffectUid, const TUint aEffectIndex);
		/**
        * Retrieve the persisted the transition effect type between images.
        * @param [out] aEffectUid the transition effect type ECom Uid to be persisted
        * @param [out] aEffectIndex the transition effect index to be persisted
        * @ref TTransEffectType
        */
		IMPORT_C void TransitionTypeL(TUid& aEffectUid, TUint& aEffectIndex) const;

    private: // member variables

		/// The implementation of the class, hidden from the clients
		class CShwCenRepWatcher ;
		/// Owns: watcher for persisted play backward in time/forward in time value
		CShwCenRepWatcher* iPlayOrder;
		/// Owns: watcher for persisted music on/off value
		CShwCenRepWatcher* iMusicOnOffWatcher; 
		/// Owns: watcher for persisted music location & name value
        CShwCenRepWatcher* iMusicNamePathWatcher;
        /// Owns: watcher for persisted transition (between images) delay value
        CShwCenRepWatcher* iTransDelayWatcher; 
		/// Owns: watcher for persisted transition type value        
        CShwCenRepWatcher* iTransTypeWatcher;
        /// Owns: watcher for persisted transition index value        
        CShwCenRepWatcher* iTransIndexWatcher;
	}; 

#endif // __CSHWSETTINGSMDL_H__

// End of File
