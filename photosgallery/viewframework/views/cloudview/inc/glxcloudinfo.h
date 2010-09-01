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
* Description:    Cloud view implementation
 *
*/




#ifndef GLXCLOUDINFO_H
#define GLXCLOUDINFO_H

// CLASS DECLARATION

/**
 *  Cloud info stores information about cloud view items
 *  @lib glxcloudview.lib
 */
class TGlxCloudInfo
	{
public:

	TInt iRowNum; //Rownum keeps track of the Cloud View Row number  //this can be removed
	TInt iStartIndex; //StartIndex of row
	TInt iEndIndex; //End Index of row //can be retrieved from a separate function
	};

#endif // GLXCLOUDINFO_H

// End of File
