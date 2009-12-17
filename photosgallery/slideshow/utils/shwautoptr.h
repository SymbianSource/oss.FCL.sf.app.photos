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
#ifndef __SHWAUTOPTR_H__
#define __SHWAUTOPTR_H__

/**
 * TShwAutoPtr, implementation of auto_ptr
 */
template< class T >
class TShwAutoPtr
	{
	public:

		/**
		 * Default constructor
		 */
		TShwAutoPtr()
			: iPtr( NULL )
			{}
	
		/**
		 * Constructor
		 * @param aPtr the pointer to own
		 */
		TShwAutoPtr( T* aPtr = 0 )
			: iPtr( aPtr )
			{}

		/**
		 * Copy constructor, transfers ownership
		 * @param aPtr the pointer to transfer
		 */
		TShwAutoPtr( TShwAutoPtr< T >& aPtr )
			: iPtr( aPtr.Release() )
			{}

		/**
		 * Destructor, releases the pointer
		 */
		~TShwAutoPtr()
			{
			delete iPtr;
			}

		/**
		 * Dereference
		 */
		T& operator*() const
			{ 
			return *iPtr;
			}

		/**
		 * Take address of pointed class
		 */
		T* operator &()
			{ 
			return iPtr;
			}
		
		/**
		 * Dereference
		 */
		T* operator->() const 
			{
			return iPtr;
			}

		/**
		 * Gives the pointer to the caller, ownership stays
		 * @return the pointer held, ownership does not transfer
		 */
		T* Pointer() const
			{
			return iPtr;
			}

		/**
		 * Assignment from one pointer to another, ownership transfers
		 * @param the auto pointer to hold
		 * @return reference to this so that assignments can be chained
		 */
		TShwAutoPtr<T>& operator=( TShwAutoPtr<T>& aCopy )
			{
			if( this != &aCopy )
				{
				// transfer the pointer
				SetPointer( aCopy.Release() );
				}
			return *this; 
			}

		/**
		 * Sets the pointer, ownership is taken
		 * @param the pointer to hold
		 */
		void SetPointer( T* aPointer )
			{
			// release the old (possible) pointer
			delete iPtr;
			iPtr = aPointer;
			}

		/**
		 * Release, returns the ownership to the caller
		 * @return the pointer held, ownership transfers
		 */
		T* Release()
			{ 
			T* temp = iPtr;
			iPtr = 0;
			return temp;
			}

	private:
		
		/// Own: the pointer to hold
		T* iPtr;

	};

#endif // __SHWAUTOPTR_H__
