/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
#ifndef NEWALLOCATOR_INL
#define NEWALLOCATOR_INL

inline RNewAllocator::RNewAllocator()
    {}

/**
@return The maximum length to which the heap can grow.

@publishedAll
@released
*/
inline TInt RNewAllocator::MaxLength() const
    {return iMaxLength;}

inline void RNewAllocator::operator delete(TAny*, TAny*) 
/**
Called if constructor issued by operator new(TUint aSize, TAny* aBase) throws exception.
This is dummy as corresponding new operator does not allocate memory.
*/
    {}


inline TUint8* RNewAllocator::Base() const
/**
Gets a pointer to the start of the heap.
    
Note that because of the small space overhead incurred by all allocated cells, 
no cell will have the same address as that returned by this function.
    
@return A pointer to the base of the heap.
*/
    {return iBase;}


inline TInt RNewAllocator::Align(TInt a) const
/**
@internalComponent
*/
    {return _ALIGN_UP(a, iAlign);}




inline const TAny* RNewAllocator::Align(const TAny* a) const
/**
@internalComponent
*/
    {return (const TAny*)_ALIGN_UP((TLinAddr)a, iAlign);}



inline void RNewAllocator::Lock() const
/**
@internalComponent
*/
    {((RFastLock&)iLock).Wait();}




inline void RNewAllocator::Unlock() const
/**
@internalComponent
*/
    {((RFastLock&)iLock).Signal();}


inline TInt RNewAllocator::ChunkHandle() const
/**
@internalComponent
*/
    {
    return iChunkHandle;
    }


#endif // NEWALLOCATOR_INL
