/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The pause handling control for the slideshow
 *
*/





#ifndef SHWTICKER_H
#define SHWTICKER_H


#include <e32base.h>
#include <e32std.h>



class MShwTickObserver;

NONSHARABLE_CLASS(CShwTicker) : public CActive
	{

public:
         
	/**
	 * NewL
	 * 2phased constructor
	 * @param MShwTickObserver&
	 */ 
	static CShwTicker* NewL(MShwTickObserver& aObserver);
         
	/**
	 * StartTicking
	 * set the ticker
	 * @param interval in seconds
	 */ 
	void StartTicking(TInt aAfter);
         
	/**
	 * CancelTicking
	 * Cancel a tick
	 */ 
	void CancelTicking();
         
	/**
	 * ~CShwTicker
	 * Standard c++ destructor
	 */ 
	~CShwTicker();
	
private:
         
	/**
	 * CShwTicker
	 * C++ constructor
	 */ 
	CShwTicker(MShwTickObserver& aObserver);
         
	/**
	 * ConstructL
	 * 2nd Phase constructor
	 */ 
	void ConstructL();
         
	/**
	 * RunL
	 * see @ref CActive
	 */ 
	void RunL();
         
	/**
	 * DoCancel
	 * see @ref CActive
	 */ 
	void DoCancel();
		
private:
	
	/** */
	MShwTickObserver& iObserver;
	
	RTimer iTimer;
	

	};
	
#endif//SHWTICKER_H


