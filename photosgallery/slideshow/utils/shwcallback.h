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
* Description:    Thin callback wrapper
 *
*/




// include guard for the whole file content
#ifndef __TSHWCALLBACK_H__
#define __TSHWCALLBACK_H__

/**
 * Helper class to construct a TCallBack from any 
 * public non static TInt (*method)() of a class
 * Usage:
 *      TShwCallBack< %ClassName%, %MethodName% >( 
 *          %ClassPointer% )
 * Example:
 *      TShwCallBack< CShwEngineImpl, DoStartL >( 
 *          this );
 */
template< class Object, TInt (Object::*Method)() >
NONSHARABLE_CLASS( TShwCallBack ) : public TCallBack
	{
	public:
		/**
		 * Constructor.
		 * @param aInstance the object instance of the class 
		 */
		inline TShwCallBack( Object* aInstance )
			: TCallBack( &DelegateL, aInstance )
			{}

	private:
		/**
		 * The callback delegator. Inlined as we dont want
		 * extra method to be created, static so that base
		 * class can call it without pointer to this.
		 * @param aInstance the instance to call back
		 * @return the return value for the TCallBack
		 */
		inline static TInt DelegateL( TAny* aInstance )
			{
			// Dispatch the call to the real objects method
			return ( static_cast< Object* >( aInstance )->*Method )();
			}
	};


#endif // __TSHWCALLBACK_H__
