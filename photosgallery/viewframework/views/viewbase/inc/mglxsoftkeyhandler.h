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
* Description:    Interface for softkey handling
*
*/




#ifndef M_GLXSOFTKEYHANDLER_H
#define M_GLXSOFTKEYHANDLER_H

/**
 * Available softkeys
 */
enum TGlxSoftkey 
	{
	EGlxLeftSoftkey,
	EGlxMiddleSoftkey,
	EGlxRightSoftkey
	};

/**
 * Allow components other than views to update softkeys if necessary
 */
class MGlxSoftkeyHandler
    {
public:
    /**
     * Store copy of current softkeys to allow original config
     * to be restored
     */
    virtual void StoreCurrentSoftKeysL() = 0;
    
    /**
     * Set specified softkey with command and text
     * @param aSoftkey softkey to set (EGlxLeftSoftkey/EGlxRighttSoftkey)
     * @param aCommand id of command to associate with softkey
     * @param aSoftkeyText text for softkey (softkey takes ownership)
     */
    virtual void ChangeSoftkeyL(TGlxSoftkey aSoftkey, TInt aCommand, 
                                                   const TDesC& aSoftkeyText) = 0;
    /**
     * Restore original softkeys and title
     */
    virtual void RestoreSoftKeysAndTitleL() = 0;

    /**
     * Enables / disables a softkey
     * @param aEnable Whether to enable or disable the softkey
     * @param aSoftkey The affected softkey
     */
    virtual void EnableSoftkey( TBool aEnable, TGlxSoftkey aSoftkey ) = 0;
    };

#endif // M_GLXSOFTKEYHANDLER_H
