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
* Description:    Header for class to implement Single line metapane
*
*/

#ifndef GLXSLMPFAVMLOBSERVER_H_
#define GLXSLMPFAVMLOBSERVER_H_

#include <mglxmedialistobserver.h>
#include <glxsinglelinemetapanecontrol.h>

class MGlxMediaList;
class CGlxDefaultAttributeContext;

class CGlxSLMPFavMLObserver : public CBase,
                              public MGlxMediaListObserver
    {
public:
    /*
     * Newl()
     */
    static CGlxSLMPFavMLObserver* NewL( CGlxSingleLineMetaPane& aSingleLineMetaPane,
            TInt aFocusIndex,MGlxMediaList* aList);
    
    /*
     * Destructor 
     */
    ~CGlxSLMPFavMLObserver();

public: // from MGlxMediaListObserver
    void HandleItemAddedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/ );
    void HandleItemRemovedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList */);
    void HandleAttributesAvailableL( TInt /*aItemIndex*/, 
        const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList */);    
    void HandleFocusChangedL( NGlxListDefs::TFocusChangeType /*aType*/, 
            TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/ );
    void HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, MGlxMediaList* /*aList*/ );
    void HandleMessageL( const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/ );
    void HandleError( TInt /*aError*/ );
    void HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, TInt /*aError*/, 
        MGlxMediaList* /*aList*/ );
    void HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ );
    void HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/ );

private:
    /*
     * Constructor
     */
    CGlxSLMPFavMLObserver(CGlxSingleLineMetaPane& aSingleLineMetaPane);
    
    /*
     * 2nd Phase Ctor
     * Create the medialist for the favourites icon here with 
     * fav collection path 
     */
    void ConstructL(TInt aFocusIndex, MGlxMediaList* aList);
    
private:
    CGlxSingleLineMetaPane&          iSingleLineMetaPane;        // SingleLineMetapane Reference
    MGlxMediaList*                  iFavMediaList;              // Favourite Medialist
    CGlxDefaultAttributeContext*    iFavAttribContext;          // Attribute context for favourites
    };


#endif /* GLXSLMPFAVMLOBSERVER_H_ */
