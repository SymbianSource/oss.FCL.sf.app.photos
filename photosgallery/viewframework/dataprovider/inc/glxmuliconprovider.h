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
* Description:    Provide Icon File Considering Requirement
*
*/




#ifndef GLXMULICONPROVIDER_H_
#define GLXMULICONPROVIDER_H_

#include <e32base.h>                  // Container Base Class
#include "glxmedia.h"                 // Reference to a media item in the item pool.
#include "mpxmediageneraldefs.h"      // Content ID identifying general category of content provided

class TGlxMedia;                      // Reference to a media item in the item pool.     
class CGlxDRMUtility;                 // DRM utility class to provide DRM-related functionality used in many places

NONSHARABLE_CLASS( CGlxMulIconProvider ) : public CBase
{
public:
	/**
	* Virtual Destructor
	*/	
	virtual ~CGlxMulIconProvider() {};
	
	/**
	* IconIdL Returns the icon id 
	* @param aMedia Media item 
	*/
	virtual TInt IconIdL( const TGlxMedia& aMedia ) const = 0;
	
	/**
	* TwoPhaseConstruction
	*/
	void ConstructL( );
	
	/**
	* ResourceFileName returns the filename of the resource
	*/
	virtual const TDesC& ResourceFileName() const ;
private:
    // Stores the resource filename
    TFileName iResFileName;
};

// -------------------------------------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS( CGlxMulLocationIconProvider ) : public CGlxMulIconProvider
{
public:
	/**
	* TwoPhaseConstruction
	*/
	IMPORT_C static CGlxMulLocationIconProvider* NewL( );
	
	/**
	* Destructor
	*/
	~CGlxMulLocationIconProvider();
	
	/**
	* IconIdL Returns the icon id 
	* @param aMedia Media item 
	*/
	TInt IconIdL( const TGlxMedia& aMedia ) const;
};

// -------------------------------------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS( CGlxMulDriveIconProvider ) : public CGlxMulIconProvider
{
public:
	/**
	* TwoPhaseConstruction
	*/
    IMPORT_C static CGlxMulDriveIconProvider* NewL( );
	
	/**
	* Destructor
	*/
	~CGlxMulDriveIconProvider();
	
	/**
	* IconIdL Returns the icon id 
	* @param aMedia Media item 
	*/
	TInt IconIdL( const TGlxMedia& aMedia ) const;
};

//-----------------------------------------------------------------------------

NONSHARABLE_CLASS( CGlxMulStaticIconProvider ) : public CGlxMulIconProvider
{
public:

	/**
	* TwoPhaseConstruction
	*/
	IMPORT_C static CGlxMulStaticIconProvider* NewL( TInt aResourceIconId );
		
	/**
	* Destructor
	*/
	CGlxMulStaticIconProvider( TInt aResourceIconId );
	
	/**
	* IconIdL Returns the icon id 
	* @param aMedia Media item 
	*/
	TInt IconIdL( const TGlxMedia& aMedia ) const;

private:
   // Stores the resource id 
   TInt iIconId;  
  	
};

//-----------------------------------------------------------------------------

NONSHARABLE_CLASS( CGlxMulDRMIconProvider ) : public CGlxMulIconProvider
{
public:

	/**
	* TwoPhaseConstruction
	*/
	IMPORT_C static CGlxMulDRMIconProvider* NewL( );
		
	/**
	* Contructor
	*/
	CGlxMulDRMIconProvider(  );
	
	/**
	  * Destructor
	  */
	~CGlxMulDRMIconProvider();    // Memory Leak Fix
	
	/**
	* IconIdL Returns the icon id 
	* @param aMedia Media item 
	*/
	TInt IconIdL( const TGlxMedia& aMedia ) const;
    
    /**
	* TwoPhaseConstruction
	*/
	void ConstructL( );
	
	
    /**
	* ResourceFileName
	*/
	const TDesC& ResourceFileName() const;
	
    
private:
   // Stores the resource id 
   TInt iIconId;  
   
   // The Resource file name from which icon to be read
   TFileName iMifFileName;	
   
   // Provides DRM related functionality
   CGlxDRMUtility* iDRMUtility;
};

//-----------------------------------------------------------------------------

NONSHARABLE_CLASS( CGlxMulVideoIconProvider ) : public CGlxMulIconProvider
{
public:

    /**
    * TwoPhaseConstruction
    */
    IMPORT_C static CGlxMulVideoIconProvider* NewL(TBool aIsFullscreen=EFalse);
        
    /**
    * Contructor
    */
    CGlxMulVideoIconProvider(TBool aIsFullscreen);
    
    /**
      * Destructor
      */
    ~CGlxMulVideoIconProvider();    
    
    /**
    * IconIdL Returns the icon id 
    * @param aMedia Media item 
    */
    TInt IconIdL( const TGlxMedia& aMedia ) const;
    
    /**
    * TwoPhaseConstruction
    */
    void ConstructL( );
    
    
    /**
    * ResourceFileName
    */
    const TDesC& ResourceFileName() const;
    
    
private:
   // Stores the resource id 
   TInt iIconId;  

   // Flag to identify fullscreen icon 
   TBool iIsFullscreen;  
   
   // The Resource file name from which icon to be read
   TFileName iMifFileName;  
};

#endif //GLXMULICONPROVIDER_H_
