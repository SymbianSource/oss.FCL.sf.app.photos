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
* Description:    set the required attribute to the medialist
*
*/




#ifndef _GLXATTRIBUTEREQUIREMENT_H__
#define _GLXATTRIBUTEREQUIREMENT_H__

#include <glxattributecontext.h>   // Fetch context to retrieve thumbnails
#include <glxthumbnailcontext.h>   // Fetch context to retrieve thumbnails
#include <glxmedialistiterator.h>  // for Fetch contexts n the list in a non-sequential order

#include <mglxmedialist.h>         // Interface for reading lists of media items

/**
 * CGlxAttributeRequirements 
 * This class will set the required attribute to the medialist
 */
class CGlxAttributeRequirements: public CBase
    {

private:
     void ConstructL(); 
public:
    
    /**
     * NewL 
     * @param aMediaList the list to which attributes will be set
     */
    IMPORT_C static  CGlxAttributeRequirements* NewL( MGlxMediaList& aMediaList ) ;
	
	/**
     * NewLC 
     * @param aMediaList the list to which attributes will be set
     */
	static  CGlxAttributeRequirements*NewLC( MGlxMediaList& aMediaList ) ;
	
	/**
     * Default constructor 
     * @param aMediaList the list to which attributes will be set
     */
	CGlxAttributeRequirements( MGlxMediaList& aMediaList );
	
	/**
     * Destructor 
     */
	~CGlxAttributeRequirements();
	
	/**
     * AddAttributeL 
     * @param aAttribute 
     */
	void AddAttributeL( const TMPXAttribute& aAttribute );
	
	/**
     * AddThumbnailL 
     * @todo
     */
	void AddThumbnailL( const TSize& aSize );
    
    /**
     * SetIteratorIndex 
     * @todo
     */
	void SetIteratorIndex( TInt aIndex );
	
	
private:
	
	// medialist to which attribute is set
	MGlxMediaList& iMediaList;
	
    TGlxFromFocusOutwardIterator iFromFocusOutwardIterator;
    TGlxFromManualIndexBlockyIterator iBlockyIterator;
    RPointerArray<CGlxDefaultAttributeContext> iArributeContext;
    RPointerArray<CGlxThumbnailContext> iThumbnailContext;
    
    TGlxFromFocusOutwardIterator iFsFromFocusOutwardIterator;
	
	//Context variable to keep track of Fullscreenthumbnail requests
	//When the context is already created, upon orientation change, only the Default spec 
	//will be changed, rather than adding a new context
	//This is done to prioritize fetching of current orientation thumbnail
	CGlxThumbnailContext* iFsThumbnailContext;
    };


#endif //_GLXATTRIBUTEREQUIREMENT_H__
