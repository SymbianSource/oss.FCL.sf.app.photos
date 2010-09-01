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
* Description:    Interface for displaying/using the popup to select collections
*
*/




#ifndef __T_GLXCCOLLECTIONSELECTIONPOPUP_H__
#define __T_GLXCCOLLECTIONSELECTIONPOPUP_H__

#include <e32base.h>

/**
 * Collection (container) types
 * These are defined as TUint32 instead of enum to allow the definition
 * of these to be spread across multiple headers (and MPX collection plugin
 * id to be used directly as the type id)
 */
// Album
const TUint32 KGlxCollectionTypeIdAlbum = 0x01;
// Tag
const TUint32 KGlxCollectionTypeIdTag = 0x02;

/**
 * @class TGlxCollectionSelectionPopup
 *
 * Collection selection popup external interface
 * 
 * Shows a popup that contains a selectable list of media collections, 
 * such as albums or tags
 * 
 *  @lib glxcollectionmanager.lib
 */
class TGlxCollectionSelectionPopup
{
public:
    /**
     * Displays the list of selectable collections in a popup. Allows creation of a new collection.
     * @ param aSelectedIds list of the selected item id
     * @ param aType Type of the collection to be shown (albums, tags, locations, etc).
     *               KGlxCollectionTypeIdAlbum and KGlxCollectionTypeIdTag.
     * @param aAllowMultipleSelection If ETrue, popup allows selecting multiple collections
     *                                If EFalse, only a single collection may be selected
     * @param aEnableContaierCreation If ETrue, popup allows the creation of a new collection.
     *                                If EFalse, popup does not allow the creation of a new collection.
     * @ return Standard error code, e.g., KErrCancel
     */
    IMPORT_C static TInt ShowPopupL(RArray<TUint32>& aSelectedIds, 
        TUint32 aCollectionType, TBool aAllowMultipleSelection, TBool aEnableContainerCreation = ETrue);
};

#endif // __T_GLXCCOLLECTIONSELECTIONPOPUP_H__