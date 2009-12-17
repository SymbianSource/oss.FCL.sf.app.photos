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
* Description:    ReferenceObserver for a callback from CGlxMediaUserTest to T_CGlxGarbageCollector
*
*/

#ifndef T_MGLXREFERENCEOBSERVER_H_
#define T_MGLXREFERENCEOBSERVER_H_


class MGlxReferenceObserver
    {
public:
    virtual void RemoveMediaReference(TInt aIndex) = 0;
    };


#endif /* T_MGLXREFERENCEOBSERVER_H_ */
