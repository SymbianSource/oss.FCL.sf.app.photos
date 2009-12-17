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
* Description:    Implementation of Metadata dialog
*
*/

#include "glxmetadataviewutility.h"

#include <eikappui.h>
#include <eikenv.h>
#include <glxtracer.h>
#include "glxmetadatadialog.h"
#include <glxuiutility.h>


// ======================= MEMBER FUNCTIONS ==================================

// ---------------------------------------------------------------------------
// ActivateViewL
// ---------------------------------------------------------------------------
//
EXPORT_C void GlxMetadataViewUtility::ActivateViewL( const TDesC& aURI )
	{
	TRACER("GlxMetadataViewUtility::ActivateViewL");
	CGlxMetadataDialog* dialog  = CGlxMetadataDialog::NewL( aURI );
	dialog->ExecuteLD();
	}
	
//End of file
