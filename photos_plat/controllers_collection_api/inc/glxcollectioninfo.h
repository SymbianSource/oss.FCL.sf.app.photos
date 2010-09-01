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
* Description:    Collection info class
*
*/




#ifndef __C_GLXCOLLECTIONINFO_H__
#define __C_GLXCOLLECTIONINFO_H__

#include <e32std.h>
#include <e32base.h>

/**
 * Collection info 
 * This is intended to be an external API only
 */
NONSHARABLE_CLASS( CGlxCollectionInfo )
	: public CBase 
    {
public:
    /**
     * Two-phase constructor.
     * @param aCollectionId MDS ID of collection (album).
     */
    IMPORT_C static CGlxCollectionInfo* NewL(TUint32 aCollectionId);

    /**
     * Destructor.
     */
    IMPORT_C ~CGlxCollectionInfo();

    /**
     * Get the ID of the collection.
     * @return ID of the collection.
     */
    IMPORT_C TUint32 Id() const;

    /**
     * Determine if the collectino exists.
     * @return ETrue if collection exists, EFalse if does not.
     */
    IMPORT_C TBool Exists() const;

    /**
     * Get the title of the collection.
     * @return Title (name) of the album.
     */
    IMPORT_C const TDesC& Title() const;

// Management functions

    /**
     * Set if collection exists.
     * @param aExists ETrue if collection exists else EFalse.
     */
    void SetExists(TBool aExists);

    /**
     * Set title.
     * @param aTitle Title to set.
     */
    void SetTitleL(const TDesC& aTitle);
    
private:
    /**
     * Constructor
     * @param aCollectionId MDS ID of collection (album)
     */
    CGlxCollectionInfo(TInt aCollectionId);

    /**
     * Second-phase constructor
     */
    void ConstructL();

private:
    /**
     * Container id in MDS. 
     */
    TUint32 iItemId;

    /**
     * ETrue if the item exists in MDS. Only populated after the data has been retrieved.
     */
    TBool iExists;

    /**
     * Title (name) of the container. Empty object until the data has been retrieved. (Owned)
     */
    HBufC* iTitle;
    };

#endif // __C_GLXCOLLECTIONINFO_H__