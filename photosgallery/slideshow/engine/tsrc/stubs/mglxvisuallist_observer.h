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
* Description:   Stub file for visual list observer
 *
*/




#ifndef __MGLXVISUALLIST_OBSERVER_H__
#define __MGLXVISUALLIST_OBSERVER_H__

//  EXTERNAL INCLUDES
#include "mglxvisuallist.h"

/**
 * EUnit Wizard generated adapter observer interface. 
 * The test class is informed about any calls to the 
 * corresponding adapter using this interface. 
 */
NONSHARABLE_CLASS( MGlxVisualList_Observer )
    {
    public:     // enums
    
		enum TMGlxVisualListMethodId
			{
			E_NotCalled,
			E_TGlxVisualListId_Id_,
			E_CHuiVisual_p_Visual_TInt_,
    E_CGlxVisualObject_p_Item_TInt_,
			E_TInt_ItemCount_,
			E_TInt_FocusIndex_,
			E_CHuiControlGroup_p_ControlGroup_,
			E_void_AddObserverL_MGlxVisualListObserver_p_,
			E_void_RemoveObserver_MGlxVisualListObserver_p_,
			E_void_AddLayoutL_MGlxLayout_p_,
			E_void_RemoveLayout_const_MGlxLayout_p_,
			E_TGlxViewContextId_AddContextL_TInt_TInt_,
			E_void_RemoveContext_const_TGlxViewContextId_r_,
			E_void_NavigateL_TInt_,
			E_TSize_Size_,
			E_void_BringVisualsToFront_
			};
        
    public:     // Destructor

        /**
         * Destructor
         */
        virtual ~MGlxVisualList_Observer() {}

    public:     // Abstract methods

        virtual void MGlxVisualList_MethodCalled(TMGlxVisualListMethodId aMethodId)=0;

    };

#endif      //  __MGLXVISUALLIST_OBSERVER_H__

// End of file
