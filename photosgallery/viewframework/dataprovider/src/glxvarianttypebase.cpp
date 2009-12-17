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
* Description:    base class of Alf data types 
*
*/




#include "glxvarianttypebase.h"

#include <alf/alfdataexception.h>         // Exception class used with invalid data exceptions
#include <glxlog.h>                   // Logging

using namespace Alf;
using namespace osncore;

// ----------------------------------------------------------------------------
// set
// ----------------------------------------------------------------------------
//
void IGlxVariantTypeBase::set( IAlfVariantType& /*aValue*/ )
    {
    // this interface is not possible to implement
    }
    
// ----------------------------------------------------------------------------
// expected to be overridden by base class if a valid type
// ----------------------------------------------------------------------------
//
bool IGlxVariantTypeBase::boolean() const
    {
    TRACER("IGlxVariantTypeBase::boolean");
    ALF_THROW( AlfDataException, EInvalidVariantDataType, "GlxVariantTypeBase" );
    }

// ----------------------------------------------------------------------------
// expected to be overridden by base class if a valid type
// ----------------------------------------------------------------------------
//
int IGlxVariantTypeBase::integer() const
    {
    TRACER("IGlxVariantTypeBase::integer");
    ALF_THROW( AlfDataException, EInvalidVariantDataType, "GlxVariantTypeBase" );
    }

// ----------------------------------------------------------------------------
// expected to be overridden by base class if a valid type
// ----------------------------------------------------------------------------
//
uint IGlxVariantTypeBase::uinteger() const
    {
    TRACER("IGlxVariantTypeBase::uinteger");
    ALF_THROW( AlfDataException, EInvalidVariantDataType, "GlxVariantTypeBase" );
    }

// ----------------------------------------------------------------------------
// expected to be overridden by base class if a valid type
// ----------------------------------------------------------------------------
//
double IGlxVariantTypeBase::real() const
    {
    TRACER("IGlxVariantTypeBase::real");
    ALF_THROW( AlfDataException, EInvalidVariantDataType, "GlxVariantTypeBase" );
    }

// ----------------------------------------------------------------------------
// expected to be overridden by base class if a valid type
// ----------------------------------------------------------------------------
//
const UString& IGlxVariantTypeBase::string() const
    {
    TRACER("IGlxVariantTypeBase::string");
    ALF_THROW( AlfDataException, EInvalidVariantDataType, "GlxVariantTypeBase" );
    }
 
// ----------------------------------------------------------------------------
// expected to be overridden by base class if a valid type
// ----------------------------------------------------------------------------
//  
IAlfContainer* IGlxVariantTypeBase::container()
    {
    TRACER("IGlxVariantTypeBase::container");
    ALF_THROW( AlfDataException, EInvalidVariantDataType, "GlxVariantTypeBase" );
    }

// ----------------------------------------------------------------------------
// expected to be overridden by base class if a valid type
// ----------------------------------------------------------------------------
//
IAlfMap* IGlxVariantTypeBase::map()
    {
    TRACER("IGlxVariantTypeBase::map");
    ALF_THROW( AlfDataException, EInvalidVariantDataType, "GlxVariantTypeBase" );
    }

// ----------------------------------------------------------------------------
// expected to be overridden by base class if a valid type
// ----------------------------------------------------------------------------
//
IAlfBranch* IGlxVariantTypeBase::branch()
    {
    TRACER("IGlxVariantTypeBase::branch");
    ALF_THROW( AlfDataException, EInvalidVariantDataType, "GlxVariantTypeBase" );
    }

// ----------------------------------------------------------------------------
// expected to be overridden by base class if a valid type
// ----------------------------------------------------------------------------
//
IAlfModelBase* IGlxVariantTypeBase::customData()
    {
    TRACER("IGlxVariantTypeBase::customData");
    ALF_THROW( AlfDataException, EInvalidVariantDataType, "GlxVariantTypeBase" );
    }

// ----------------------------------------------------------------------------
// type
// ----------------------------------------------------------------------------
//
IAlfVariantType::Type IGlxStringVariantTypeBase::type() const
    {
    TRACER("IGlxStringVariantTypeBase::type");
    return EString;
    }
    
// ----------------------------------------------------------------------------
// type
// ----------------------------------------------------------------------------
//
IAlfVariantType::Type IGlxIntVariantTypeBase::type() const
    {
    TRACER("IGlxIntVariantTypeBase::type");
    return EInt;
    }
