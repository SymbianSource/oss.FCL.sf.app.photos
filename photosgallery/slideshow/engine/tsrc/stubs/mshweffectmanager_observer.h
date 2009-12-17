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
* Description:   Stub file
 *
*/





#ifndef __MSHWEFFECTMANAGER_OBSERVER_H__
#define __MSHWEFFECTMANAGER_OBSERVER_H__

//  EXTERNAL INCLUDES
#include "shweffectmanager.h"

/**
 * EUnit Wizard generated adapter observer interface. 
 * The test class is informed about any calls to the 
 * corresponding adapter using this interface. 
 */
NONSHARABLE_CLASS( MShwEffectManager_Observer )
    {
    public:     // enums
    
		enum TMShwEffectManagerMethodId
			{
			E_NotCalled,
			E_void_AdoptEffectsL_TArrayMShwEffect_p,
			E_MShwEffect_p_PreviousEffect,
			E_MShwEffect_p_CurrentEffect,
			E_MShwEffect_p_NextEffect,
			E_void_ProceedToNextEffect,
			E_MGlxLayout_InitializeL,
			E_MGlxLayout_PrepareViewL,
			E_MGlxLayout_EnterViewL_TInt,
			E_void_ExitView,
			E_MGlxLayout_EnterTransitionL_TInt,
			E_void_ExitTransition
			};
        
    public:     // Destructor

        /**
         * Destructor
         */
        virtual ~MShwEffectManager_Observer() {}

    public:     // Abstract methods

        virtual void MShwEffectManager_MethodCalled(TMShwEffectManagerMethodId aMethodId)=0;

    };

#endif      //  __MSHWEFFECTMANAGER_OBSERVER_H__

// End of file
