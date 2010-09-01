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
* Description:    collect binding handling here
*
*/




#ifndef _GLXCOMMANDVARIANTTYPE_H_
#define _GLXCOMMANDVARIANTTYPE_H_

#include <e32base.h>                   // Container Base Class
#include <mul/mulvarianttype.h>        // create data for data model

//Forward declaration
class MGlxBoundCommand;                // Handles user commands

namespace Alf
    {
    class MulVisualItem;               // Client need to use this class to add data in data model
    }
    
/**
 * CommandVariantType
 */
NONSHARABLE_CLASS( CommandVariantType ) :  public Alf::MulVariantType
    {
public:

	/**
	 * Constructor
	 */
    CommandVariantType( MGlxBoundCommand& aCommand );
    
   	/**
	 * Command
	 * @return MGlxBoundCommand the command binded
	 */
	MGlxBoundCommand& Command() const;
	
    /**
     * Destructor
     */	
	~CommandVariantType();
	
private: 
	// From MulVariantType
    /** Should not be called, but in case of misbehaving model code */
    int integer() const;
    
    TMulType Type() const; 
    
    std::auto_ptr< IMulVariantType > Clone();

private:
    MGlxBoundCommand& iCommand;
    };

/**
 * GlxCommandBindingUtility
 * collect binding handling here, so that nasty command attribute (tag) name usage remains in one class
 */
NONSHARABLE_CLASS( GlxCommandBindingUtility )
    {
public:

	/**
	 * SetT
	 * set the binding for the visual item
	 * @param MulVisualItem the visual item
	 * @param aCommand MGlxBoundCommand to set for visual item
	 */
    static void SetT( Alf::MulVisualItem& aItem, MGlxBoundCommand& aCommand );
    
   	/**
	 * Get
	 * Get BoundCommand for a visual item
	 * @param MulVisualItem the visual item
	 * @return MGlxBoundCommand
	 * returns NULL if command value was not added 
	 */
    static MGlxBoundCommand* Get( const Alf::MulVisualItem& aItem );
    };
    
#endif // _GLXCOMMANDVARIANTTYPE_H_
