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
* Description:    Implementation of Metadata view
*
*/



#ifndef _GLXMETADATABINDINGSETFACTORY_H_
#define _GLXMETADATABINDINGSETFACTORY_H_


#include <e32def.h>
#include <mul\mulvisualitem.h>
#include "glxmulbindingsetfactory.h"

NONSHARABLE_CLASS( TGlxMulTitleDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulTitleDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulTitleDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };

NONSHARABLE_CLASS( TGlxMulDateAndTimeDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulDateAndTimeDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulDateAndTimeDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };

NONSHARABLE_CLASS( TGlxMulDescriptionDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulDescriptionDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulDescriptionDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };

NONSHARABLE_CLASS( TGlxMulTagsDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulTagsDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulTagsDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };
    
NONSHARABLE_CLASS( TGlxMulAlbumsDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulAlbumsDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulAlbumsDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };
    
NONSHARABLE_CLASS( TGlxMulLocationDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulLocationDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulLocationDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };     
    
NONSHARABLE_CLASS( TGlxMulFileSizeDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulFileSizeDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulFileSizeDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };      


NONSHARABLE_CLASS( TGlxMulResolutionDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulResolutionDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulResolutionDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };    

NONSHARABLE_CLASS( TGlxMulDurationDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulDurationDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulDurationDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };
    
NONSHARABLE_CLASS( TGlxMulUsageRightsDetailsBindingSetFactory ) : public TGlxMulBindingSetFactory
    {
public:
	IMPORT_C TGlxMulUsageRightsDetailsBindingSetFactory();
	IMPORT_C ~TGlxMulUsageRightsDetailsBindingSetFactory();
private:
	/**
	* Add different types of binding to the Binding set	
	* @param aCommand command associated with option menu item/items
	*/
	void AddBindingsL() const;
    };

#endif // _GLXMETADATABINDINGSETFACTORY_H_   