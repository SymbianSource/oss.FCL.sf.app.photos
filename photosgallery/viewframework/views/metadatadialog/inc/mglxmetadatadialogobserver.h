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
* Description:    Metadata dialog implementation
*
*/




#ifndef MGLXMETADATADIALOGOBSERVER_H_
#define MGLXMETADATADIALOGOBSERVER_H_

/**
 *  MGlxMetadataDialogObserver
 *  List event notification interface
 */
class MGlxMetadataDialogObserver
	{
	public:
		/* OnLocationEditL
		 * On Lacation Pane select for launching optons 
		 **/
		virtual void OnLocationEditL() = 0;
		virtual void AddTagL() = 0;
		virtual void AddAlbumL() = 0;
	};

#endif /*MGLXMETADATADIALOGOBSERVER_H_*/

//EOF