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
* Description:    Cloud view implementation
 *
*/




#ifndef C_GLXBUBBLETIMER_H
#define C_GLXBUBBLETIMER_H

// INCLUDES
#include <e32base.h>

/**
 *MGlxTimerNotifier is a calss to get timer delay functionality
 */
class MGlxTimerNotifier
	{
public:
	/** 	
	 *  @function TimerComplete
	 *  @since S60 3.2
	 *  Function is used to get timer notification
	 */
	virtual void TimerComplete() = 0;
	};

//Forward Declaration
class MGlxTimerNotifier;

/**
 * CGlxBubbleTimer is a clsss derived from CActive.
 *It provides active objects functionality.
 */
NONSHARABLE_CLASS(CGlxBubbleTimer): public CActive
	{
public:

	/** 	
	 *  @function NewL
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aNotifier - MGlxTimerNotifier class pointer
	 *  @return Pointer to newly created object
	 */
	static CGlxBubbleTimer *NewL(MGlxTimerNotifier *aNotifier);

	/**
	 * Destructor.
	 */
	~CGlxBubbleTimer();

	/** 	
	 *  @function SetDelay
	 *  @since S60 3.2
	 *  @param aDelay -Time period for timer
	 */
	void SetDelay(TTimeIntervalMicroSeconds32 aDelay);

private:

	/** 	
	 *  @Default c++ contructor
	 *  @since S60 3.2
	 *  @aNotifier MGlxTimerNotifier class pointer
	 */
	CGlxBubbleTimer(MGlxTimerNotifier *aNotifier);

	/** 	
	 *  @function ConstructL
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 */
	void ConstructL();

	/** 	
	 *  @function RunL
	 *  @since S60 3.2
	 *  Executed when callback occurs
	 */
	void RunL();

	/** 	
	 *  @function DoCancel
	 *  @since S60 3.2
	 */
	void DoCancel();

private:

	RTimer iTimer; //Timer object
	MGlxTimerNotifier *iNotifier; //Notifier variable

	};

#endif // C_GLXBUBBLETIMER_H

// End of File
