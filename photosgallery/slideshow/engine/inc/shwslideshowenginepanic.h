/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Panic codes for the slideshow engine
 *
*/




#ifndef __SHWSLIDESHOWENGINEPANIC_H__
#define __SHWSLIDESHOWENGINEPANIC_H__

//  Type definitions
namespace NShwEngine
	{
	/**
	 * Slideshow engine panic enumerations
	 */
	enum TShwEnginePanic
		{
		EVisualListFocusNotChanged,
		EEngineStartLCalledInWrongState,
		ENullHuiDisplay,
		ENullVisualList,
		ENullCurrentEffect,
		EIncorrectEffectIndex, // need to call SetDefaultEffectL to get rid of this
		EEngineFatalError // this means that engine got onto an non-recoverable state
		};
	
	/**
	 * This function is used to panic the slideshowengine
	 * @param aPanic, the enumeration for the panic @see TShwEnginePanic
	 */
	extern void Panic( TShwEnginePanic aPanic );
	}

#endif  // __SHWSLIDESHOWENGINEPANIC_H__


