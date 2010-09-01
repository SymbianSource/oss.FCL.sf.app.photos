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
* Description:    Utility for thumbnail tasks handling files
*
*/




/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef GLXTNFILEUTILITY_H
#define GLXTNFILEUTILITY_H

#include <e32base.h>
#include <f32file.h>

/**
 * Utility for thumbnail tasks handling files.
 *
 * @author Dan Rhodes
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS(CGlxtnFileUtility) : public CBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CGlxtnFileUtility* NewL();

    /**
    * Destructor.
    */
    ~CGlxtnFileUtility();

    /**
    * Provide file server session for opening images from files.
    */
    RFs& FsSession();

    /**
    * Test whether a file is on the bad file list.  If so this method leaves.
    * If not the bad file marker is set to the filename, so that if a panic
    * occurs the file will be added to the bad file list.
    * @param aFilename Path of file for which a thumbnail is to be generated.
    */
    void CheckBadFileListL(const TDesC& aFilename);

    /**
    * Clear the bad file marker.  Called when processing a file is complete (no
    * panic occurred).
    */
    void ClearBadFileMarker();

    /**
    * Test whether a generated thumbnail should be stored in persistent storage.
    * @param aSize Requested size of thumbnail.
    */
    TBool IsPersistentSize(const TSize& aSize);

private:

    /**
    * C++ default constructor.
    */
    CGlxtnFileUtility();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * Read bad file list from file and add file from marker, if present.
    */
    void ReadBadFileListL();
    /**
    * Write bad file list to file.
    */
    void WriteBadFileListL();
	/**
    * calculates Grid Icon Size for landscape orientation and Fullscreen
    * Image size depending on Layout defines.
    */
	void LayoutSpecificDataL();
	
private:
    /** File server session for opening images from files */
    RFs iFs;
    /** Directory in which to store bad file list and marker */
    TFileName iBadFileDir;
    /** Filenames of media files which cause panics when decoding */
    RPointerArray<HBufC> iBadFileArray;
    /** Persistent thumbnail size classes */
    RArray<TSize> iPersistentSizeClasses;
    };

#endif  // GLXTNFILEUTILITY_H
