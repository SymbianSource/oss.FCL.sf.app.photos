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
* Description:    Thin cleanup wrapper
 *
*/




// include guard for the whole file content
#ifndef __TSHWCLEANUPWRAPPER_H__
#define __TSHWCLEANUPWRAPPER_H__

/**
 * Helper class to construct a TCleanupItem from any 
 * public non static void (*method)() of a class
 * Usage:
 *      TShwCleanupWrapper< %ClassName%, %MethodName% >( 
 *          %ClassPointer% )
 * Example:
 *      RPointerArray< MInterface > array;
 *      CleanupStack::PushL( 
 *          TShwCleanupWrapper< 
 *              RPointerArray<MInterface>,
 *              RPointerArray<MInterface>::ResetAndDestroy >( array ) );
 */
template< class Object, void (Object::*Method)()>
NONSHARABLE_CLASS( TShwCleanupWrapper ) : public TCleanupItem
	{
	public:
        /**
		 * Constructor.
		 * @param aInstance the object instance of the class 
		 */
		inline TShwCleanupWrapper( Object& aInstance )
			: TCleanupItem( &Cleanup, &aInstance )
			{}

    private:
        /**
		 * The cleanup callback delegator. Inlined as we dont want
		 * extra method to be created, static so that base
		 * class can call it without pointer to this.
		 * @param aInstance the instance of TShwCallBack
		 * @return the return value for the TCallBack
		 */
		inline static void Cleanup( TAny* aInstance )
			{
			( static_cast< Object* >( aInstance )->*Method )();
			}
	};

/**
 * Helper function to create cleanup item without ugly template notations
 * @param aObject the object to call ResetAndDestroy in case of leave
 * 
 * Usage:
 *      ShwCleanupResetAndDestroyPushL( array );
 * Example:
 *   RPointerArray< MShwEffect > effects;
 *   // put the array in cleanupstack in case CreateEffectsL leaves
 *    ShwCleanupResetAndDestroyPushL( effects );
 */
template< class T >
void ShwCleanupResetAndDestroyPushL( T& aObject )
	{
	CleanupStack::PushL( 
		TShwCleanupWrapper< T, &T::ResetAndDestroy >( aObject ) );
	}

#endif // __TSHWCLEANUPWRAPPER_H__
