/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    CGlxMediaList unit tests
*
*/




//  CLASS HEADER
#include <mglxmedialist.h>

//
CGlxMedia::CGlxMedia(const TGlxMediaId& aId)
		: iId(aId)
    {
    }
    
CGlxMedia::~CGlxMedia()
    {
	Reset();
    iValues.Close(); 
    iAttributes.Close();
    
    }    
       	
class _CGlxTestMediaList: public CBase, public MGlxMediaList
	{
public:
	~_CGlxTestMediaList() 
	    {
	    iItems.Close();
        iMedia.ResetAndDestroy();
        iSelectedItems.Close();
        iItemListObservers.Close();
	    }
	    
	virtual void Close() 
	    {
	    };
	virtual TGlxMediaListId Id() const 
	    {
	    return KGlxIdNone;
	    }

	virtual TInt Count(NGlxListDefs::TCountType /*aType*/) const 
	    {
	    return iItems.Count();
	    }
	
	virtual TInt FocusIndex() const 
	    {
	    return iFocusIndex;
	    }

	virtual void SetFocusL(NGlxListDefs::TFocusSetType /*aType*/, TInt aValue) 
	    {
	    iFocusIndex = aValue;
	    }
	
	virtual const TGlxMedia& Item(TInt aIndex) const 
	    {
	    return iItems[aIndex];
	    }

	virtual TInt Index(const TGlxIdSpaceId& /* aIdSpaceId */, const TGlxMediaId& aId) const 
	    {
	    TInt count = iItems.Count();
	    for (TInt i = 0; i < count; i++)
	        {
	        if (iItems[i].Id() == aId)
	            {
	            return i;
	            }
	        }
	    return KErrNotFound;
	    }
	
	virtual void AddMediaListObserverL(MGlxMediaListObserver* aObserver) 
	    {
	    iItemListObservers.AppendL(aObserver);
	    }
	
	virtual void RemoveMediaListObserver(MGlxMediaListObserver* aObserver) 
	    {
	    TInt index = iItemListObservers.Find(aObserver);
  
        if (index != KErrNotFound) 
            {
            iItemListObservers.Remove(index);	
            }
	    }

    virtual void AddContextL(const MGlxFetchContext* /*aContext*/, TInt /*aPriority*/) 
        {
        }

	virtual void RemoveContext(const MGlxFetchContext* /*aContext*/) 
	    {
	    }
	
	virtual MMPXCollection& Collection() const 
	    {
	    // we know that this method is not called in our tests, this is just to make the code compile
	    MMPXCollection* empty = NULL;
	    return *empty;
	    }

	virtual TBool IsSelected(TInt aIndex) const 
	    {
	    TInt idx = iSelectedItems.Find(aIndex);
	    return (idx != KErrNotFound);
	    }

	virtual void SetSelectedL(TInt aIndex, TBool aSelected) 
	    {
	    if (aSelected)
	        {
            iSelectedItems.InsertInOrder(aIndex);
	        }
	    else 
	        {
	        iSelectedItems.Remove(iSelectedItems.Find(aIndex));
	        }
	    }

	virtual const TArray<TInt> Selection() const 
	    {
	    return iSelectedItems.Array();
	    }

	virtual void CommandL(CMPXCommand& /*aCommand*/) 
	    {
	    }

	virtual void CancelCommand() 
	    {
	    }

	virtual void SetFilterL(CMPXFilter* /*aFilter*/) 
	    {
	    }

	virtual CMPXFilter* Filter() const 
	    {
	    return NULL;
	    }

    virtual TGlxIdSpaceId IdSpaceId(TInt /*aIndex*/) const 
        {
        return KGlxIdNone;
        }
    
    void AppendL(TInt aId, TBool aCreateMedia) 
        {
        if (aCreateMedia) 
            {
            CGlxMedia* media = new (ELeave) CGlxMedia(TGlxMediaId(aId));
            CleanupStack::PushL(media);
            iMedia.AppendL(media);
            CleanupStack::Pop(media);
            iItems.AppendL(TGlxMedia(TGlxMediaId(aId), media));
            }
        else 
            {
            iItems.AppendL(TGlxMedia(TGlxMediaId(aId)));
            }
            
        TInt pos = iItems.Count() -1;
            
        TInt obCount = iItemListObservers.Count();
        for(TInt i = 0; i < obCount; i++)
            {
            iItemListObservers[i]->HandleItemAddedL(pos, pos, this );
            }
        }
        
    void AppendL(TInt aId, CGlxMedia* aMedia) 
        {
        iMedia.AppendL(aMedia);   
        iItems.AppendL(TGlxMedia(TGlxMediaId(aId), aMedia));
        
        TInt pos = iItems.Count() -1;
            
        TInt obCount = iItemListObservers.Count();
        for(TInt i = 0; i < obCount; i++)
            {
            iItemListObservers[i]->HandleItemAddedL(pos, pos, this );
            }
        }
        
    void HandleAttributesAvailableL() 
        {
        RArray<TMPXAttribute> attrArray;
        TInt obCount = iItemListObservers.Count();
        for(TInt i = 0; i < obCount; i++)
            {
            iItemListObservers[i]->HandleAttributesAvailableL(0, attrArray, this );
            }
        attrArray.Close();
        }    
        
    CGlxMedia* MediaObj(TInt aIndex) const 
        {
        return const_cast<CGlxMedia*>(static_cast<const CGlxMedia*>(iItems[aIndex].Properties()));
        }
        
    CMPXCollectionPath* PathLC(NGlxListDefs::TPathType /*aType*/) const
    	{
    	return NULL;
    	}
    
    TInt SelectionCount() const
    	{
    	return iSelectedItems.Count();
    	}
    	
    TInt SelectedItemIndex(TInt /*aSelectionIndex*/) const
    	{
    	return 0;
    	}
    	
    TBool IsPopulated() const
        {
        return ETrue;
        }
    
    void AddStaticItemL( CGlxMedia* /*aStaticItem*/,
        NGlxListDefs::TInsertionPosition /*aTargetPosition*/ ) {};
    
    void RemoveStaticItem(const TGlxMediaId& /*aItemId*/) {};
    
    void SetStaticItemsEnabled( TBool aEnabled ) { iStaticItemsEnabled = aEnabled; };
    
    TBool IsStaticItemsEnabled() const { return iStaticItemsEnabled; };
    
    void SetFocusInitialPosition(NGlxListDefs::TFocusInitialPosition aFocusInitialPosition){};

    void ResetFocus(){};        

    void SetVisibleWindowIndexL( TInt aIndex ){};
    TInt VisibleWindowIndex() const {};
    virtual void CancelPreviousRequests() {};
    RArray<TGlxMedia> iItems;
    RPointerArray<CGlxMedia> iMedia;
    RArray<TInt> iSelectedItems;
    TInt iFocusIndex;
    
    RPointerArray<MGlxMediaListObserver> iItemListObservers;
    TBool iStaticItemsEnabled;
	};    

//  END OF FILE
