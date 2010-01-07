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
* Description: Image Viewer Manager
*
*/



// INCLUDES

EXPORT_C HBufC* CGlxImageViewerManager::ImageUri( void )
    {
    return iImageUri;
    }

EXPORT_C RFile64& CGlxImageViewerManager::ImageFileHandle() const
    {
    return *iFile;
    }

EXPORT_C TBool CGlxImageViewerManager::IsPrivate()
    {
    return iIsPrivate;
    }

EXPORT_C void CGlxImageViewerManager::IncrementRefCount()
    {
    iRefCount++;
    }
