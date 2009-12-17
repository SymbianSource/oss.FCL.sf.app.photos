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
* Description:    Texture Manager component
*
*/




#ifndef M_GLXTEXTUREOBSERVER_H
#define M_GLXTEXTUREOBSERVER_H

#include <e32std.h>
#include <alf/alftexturemanager.h>
/**
 * Observer interface used to be notified when texture content changes.
 *
 * @author Dan Rhodes
 */
NONSHARABLE_CLASS( MGlxTextureObserver )
    {
public:
    /**
    * Called when the content of the texture is changed.
    * @param aHasContent Whether the texture now has content.
    */
    virtual void TextureContentChangedL( TBool aHasContent , CAlfTexture* aNewTexture) = 0;
    };

#endif  // M_GLXTEXTUREOBSERVER_H
