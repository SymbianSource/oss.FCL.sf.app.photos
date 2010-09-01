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




#include "glxcommandbindingutility.h"        // collect binding handling here
#include <glxlog.h>                          // Logging
#include <memory>
#include <mul/mulvisualitem.h>                   // Client need to use this class to add data in data model
#include <glxtracer.h>
#include <mul/mulvisualitem.h>              // Client need to use this class to add data in data model 

const int KCommandTagName = 200;

// ----------------------------------------------------------------------------
// CommandVariantType Default Constructor
// ----------------------------------------------------------------------------
//
CommandVariantType::CommandVariantType( MGlxBoundCommand& aCommand )
    : iCommand( aCommand )
	{
	}

// ----------------------------------------------------------------------------
// CommandVariantType destructor
// ----------------------------------------------------------------------------
//
CommandVariantType::~CommandVariantType()
    {
    //no implementation
    }	
// ----------------------------------------------------------------------------
// CommandVariantType::Command
// ----------------------------------------------------------------------------
//
MGlxBoundCommand& CommandVariantType::Command() const
	{
	TRACER("CommandVariantType::Command");
	return iCommand;
	}
	
// ----------------------------------------------------------------------------
// CommandVariantType::integer
// ----------------------------------------------------------------------------
//
int CommandVariantType::integer() const
	{
	TRACER("CommandVariantType::integer");
	return 0; 
	}

// ----------------------------------------------------------------------------
// CommandVariantType::Type
// ----------------------------------------------------------------------------
//
Alf::MulVariantType::TMulType CommandVariantType::Type() const 
     {
     TRACER("CommandVariantType::Type");
     return MulVariantType::ECustomData;
     }

// ----------------------------------------------------------------------------
// CommandVariantType::Clone
// ----------------------------------------------------------------------------
// 
 std::auto_ptr< Alf::MulVariantType::IMulVariantType > CommandVariantType::Clone()
     {
     TRACER("CommandVariantType::Clone");
     std::auto_ptr<Alf::MulVariantType::IMulVariantType> clone( new (EMM) CommandVariantType(iCommand));
     return clone;
     }
 
// ----------------------------------------------------------------------------
// GlxCommandBindingUtility::SetT
// ------------------------------------------------------------`----------------
//
void GlxCommandBindingUtility::SetT( Alf::MulVisualItem& aItem, MGlxBoundCommand& aCommand )
    {
    TRACER("GlxCommandBindingUtility::SetT");
    std::auto_ptr< Alf::IMulVariantType > value( new ( EMM ) CommandVariantType( aCommand ) );
    aItem.SetAttribute( (Alf::mulvisualitem::TVisualAttribute)( KCommandTagName ), value.get() );
    value.release();
    }
    
// ----------------------------------------------------------------------------
// GlxCommandBindingUtility::Get
// ----------------------------------------------------------------------------
//
MGlxBoundCommand* GlxCommandBindingUtility::Get( const Alf::MulVisualItem& aItem )
    {
    TRACER("GlxCommandBindingUtility::Get");
    Alf::IMulVariantType* attr = 
        const_cast< Alf::MulVisualItem& >( aItem ).Attribute(
             (Alf::mulvisualitem::TVisualAttribute)( KCommandTagName ) );
    if ( attr )
        {
        // If this crashes, it means that the model has tried to copy CommandVariantType object (and hence converted it to AlfVariantType)
        // this is a bug in the mul model
        __ASSERT_DEBUG( dynamic_cast< CommandVariantType* >(attr), User::Invariant() ); /// @todo panic instead
        return &static_cast< CommandVariantType* >( attr )->Command();
        }
    return NULL;
    }
