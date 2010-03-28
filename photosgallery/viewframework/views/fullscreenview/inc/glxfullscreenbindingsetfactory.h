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
* Description:    Implementation of fULL-SCREEN view
*
*/




#ifndef _GLXFULLSCREENBINDINGSETFACTORY_H_
#define _GLXFULLSCREENBINDINGSETFACTORY_H_

#include <e32def.h>
#include <mul/mulvisualitem.h>
#include "glxmulbindingsetfactory.h"

NONSHARABLE_CLASS( TGlxMulFullScreenBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
    IMPORT_C TGlxMulFullScreenBindingSetFactory();
    IMPORT_C ~TGlxMulFullScreenBindingSetFactory();
private:
    void AddBindingsL() const;
    };
   
 NONSHARABLE_CLASS( TGlxMulFSMPTitleBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulFSMPTitleBindingSetFactory();
	IMPORT_C ~TGlxMulFSMPTitleBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };    
	
NONSHARABLE_CLASS( TGlxMulFSMPDateBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulFSMPDateBindingSetFactory();
	IMPORT_C ~TGlxMulFSMPDateBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };   

NONSHARABLE_CLASS( TGlxMulFSMPTimeBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulFSMPTimeBindingSetFactory();
	IMPORT_C ~TGlxMulFSMPTimeBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    }; 
NONSHARABLE_CLASS( TGlxMulFSMPMemoryBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulFSMPMemoryBindingSetFactory();
	IMPORT_C ~TGlxMulFSMPMemoryBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };   
	

NONSHARABLE_CLASS( TGlxMulFSMPAlbumBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulFSMPAlbumBindingSetFactory();
	IMPORT_C ~TGlxMulFSMPAlbumBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };   
	
NONSHARABLE_CLASS( TGlxMulFSMPLocationBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulFSMPLocationBindingSetFactory();
	IMPORT_C ~TGlxMulFSMPLocationBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };  
	
NONSHARABLE_CLASS( TGlxMulFSMPTagsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulFSMPTagsBindingSetFactory();
	IMPORT_C ~TGlxMulFSMPTagsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    }; 

#endif // _GLXFULLSCREENBINDINGSETFACTORY_H_
