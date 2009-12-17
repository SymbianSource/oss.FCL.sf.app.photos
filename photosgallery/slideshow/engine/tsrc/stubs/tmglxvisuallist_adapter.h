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
* Description:   Stub file for visual list adapter
 *
*/




#ifndef __TMGLXVISUALLIST_ADAPTER_H__
#define __TMGLXVISUALLIST_ADAPTER_H__

//  EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include "MGlxVisualList_Observer.h"
#include <mglxvisuallist.h>

//  FORWARD DECLARATIONS

namespace TMGlxVisualList_Adapter_Config
	{
	const TInt KDefaultSize = 3;
	const TInt KDefaultFocus = 1;
	}

//  CLASS DEFINITION
/**
 * Stub version of visual list
 * for testing purposes
 */
class TMGlxVisualList_Adapter
    : public MGlxVisualList
	{
	public:     // Constructors and destructors

		/**
		 * Construction
		 */
		TMGlxVisualList_Adapter( MGlxVisualList_Observer* aObserver );
		~TMGlxVisualList_Adapter();
		void SetAdaptee( MGlxVisualList* aAdaptee );        

	public:	// from MGlxVisualList

		TGlxVisualListId Id() const;
		CHuiVisual* Visual(TInt aListIndex);
         CGlxVisualObject* Item(TInt aListIndex);
		TInt ItemCount() const;
		TInt FocusIndex() const;
		CHuiControlGroup* ControlGroup() const;
		void AddObserverL(MGlxVisualListObserver* aObserver);
		void RemoveObserver(MGlxVisualListObserver* aObserver);
		void AddLayoutL(MGlxLayout* aLayout);
		void RemoveLayout(const MGlxLayout* aLayout);
		TGlxViewContextId AddContextL(TInt aFrontVisibleRangeOffset, TInt aRearVisibleRangeOffset);
		void RemoveContext(const TGlxViewContextId& aContextId);
		void NavigateL(TInt aIndexCount);
		TSize Size() const;
		void BringVisualsToFront();
        void EnableAnimationL(TBool aAnimate, TInt aIndex);
        /// @ref MGlxVisualList::SetDefaultIconBehaviourL
        void SetDefaultIconBehaviourL( TBool aEnable );
        
        void AddIconL( TInt , const CHuiTexture& , 
            NGlxIconMgrDefs::TGlxIconPosition ,
            TBool , TBool ,  TInt, TReal32, TReal32  ) { }
            
        TBool RemoveIcon( TInt , const CHuiTexture&  ) { return ETrue;}
    
        void SetIconVisibility( TInt , const CHuiTexture&, TBool  ) { }

	public:		// Data

		TInt iSize;
		TInt iFocus;
		
	private:    // Data

		RArray<MGlxVisualListObserver*> iObservers;
		MGlxVisualList_Observer* iMGlxVisualList_Observer;
		MGlxVisualList* iMGlxVisualList;

	};

#endif      //  __TMGLXVISUALLIST_ADAPTER_H__

// End of file
