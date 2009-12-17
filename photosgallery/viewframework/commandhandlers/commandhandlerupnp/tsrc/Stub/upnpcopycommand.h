/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Upnp copy to home network command stub
*
*/


#ifndef __C_GLXUPNPCOPYCOMMAND_H__
#define __C_GLXUPNPCOPYCOMMAND_H__


/*Class declaration
* The UPNP Framwork Stub
*/


// CLASS DEFINITION
class CUpnpCopyCommand: public CBase
    {

public: // Construction/destruction methods

        inline static CUpnpCopyCommand* NewL()
        	{
        	CUpnpCopyCommand* self = new (ELeave) CUpnpCopyCommand();
        	return self;
        	}

       inline ~CUpnpCopyCommand()
        	{
        	
        	}

    public: // Business logic methods

      inline static TBool IsAvailableL()
        	{
        	return gUpnpCopyCommandEnabled;
        	}

      inline void CopyFilesL( CDesCArrayFlat* /*aFiles*/ )
        	{
        	
        	}

public:
    static TBool gUpnpCopyCommandEnabled;
    };
 

    
#endif
