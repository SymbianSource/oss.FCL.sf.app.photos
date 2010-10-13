/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Stub file for managing effect in slideshow
 *
*/




#ifndef __TMSHWEFFECTMANAGER_ADAPTER_H__
#define __TMSHWEFFECTMANAGER_ADAPTER_H__

//  EXTERNAL INCLUDES


//  INTERNAL INCLUDES
#include "mshweffectmanager_observer.h"
#include "shweffectmanager.h"
#include "shweffect.h"

//  FORWARD DECLARATIONS

//  CLASS DEFINITION
/**
 * Stub implementation of effect manager and effect.
 */
class TMShwEffectManager_Adapter
    : public MShwEffectManager, public MShwEffect
    {
    public:     // Constructors and destructors

        /**
         * Construction
         */
        TMShwEffectManager_Adapter( MShwEffectManager_Observer* aObserver );
        void SetAdaptee( MShwEffectManager* aAdaptee );        

    public:	// From MShwEffectManager

        void AddEffectL( MShwEffect* aEffect );
        MShwEffect* CurrentEffect();
        MShwEffect* Effect( TInt aDirection );
        void ProceedToEffect( TInt aDirection );
        void SetEffectOrder( TShwEffectOrder aOrder );
        TShwEffectOrder EffectOrder();
        void SetProgrammedEffects( RArray< TShwEffectInfo >& aEffects );
        TInt ProgrammedEffects( RArray< MShwEffect* >& aEffects );
		void SetDefaultEffectL(TShwEffectInfo aInfo);
		void GetActiveEffectsL( RPointerArray< MShwEffect >& aEffects );
         
    public: // From MShwEffect
    
		MShwEffect* CloneLC();
		void InitializeL( 
    		CHuiEnv* aHuiEnv,
		    MGlxVisualList* aVisualList,
		    MGlxMediaList* aMediaList,
		    TSize aScreenSize );
		TSize PrepareViewL( CHuiVisual* aVisual, TSize aSize );
		MGlxLayout* EnterViewL( CHuiVisual* aVisual, TInt aDuration, TInt aDuration2 );
		void ExitView( CHuiVisual* aVisual );
		MGlxLayout* EnterTransitionL( CHuiVisual* aVisual, TInt aDuration );
		void ExitTransition( CHuiVisual* aVisual );
		void PauseL();
		void Resume();
		TShwEffectInfo EffectInfo();
                
    public:		// Data (for verification)

    	TInt iViewDuration;
    	TInt iTransitionDuration;
    	CHuiVisual* iPrepareVisual;
    	CHuiVisual* iViewVisual;
    	CHuiVisual* iTransitionVisual;
    	
    private:    // Data

        MShwEffectManager_Observer* iMShwEffectManager_Observer;
        MShwEffectManager* iMShwEffectManager;

    };

#endif      //  __TMSHWEFFECTMANAGER_ADAPTER_H__

// End of file
