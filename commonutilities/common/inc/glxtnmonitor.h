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
* Description:  
*
*/


#ifndef PLUGINSCREENSAVERMONITORAO_H_
#define PLUGINSCREENSAVERMONITORAO_H_

#include <e32base.h>
#include <e32property.h>

class MGlxTNObserver
{
public :
    /*
     * Constructor
     */
    MGlxTNObserver() {}
    /*
     * Call back to get the number is left to create.
     */
    virtual void updateTNCount(int & count) = 0;
    /*
     * Destructor
     */
    virtual ~MGlxTNObserver() {}    
};


/**
* Class for monitoring harvester pause.
*/
class CGlxTNMonitor: public CActive
{
    public:
        /** Construction */
    	IMPORT_C static CGlxTNMonitor* NewL( MGlxTNObserver *observer );

        /**
         * Destructor - virtual and class not intended
         * for derivation, so not exported.
         */
    	virtual ~CGlxTNMonitor();

    protected:

    	/**
    	 * RunL
    	 * From CActive.
    	 */
        virtual void RunL();

    	/**
    	 * DoCancel
    	 * From CActive.
    	 */
        virtual void DoCancel();

    	/**
    	 * RunError. Error handler in case RunL() leaves.
    	 * From CActive.
    	 * @param aError  Leave code from RunL.
    	 * @return  Error code. KErrNone if error was handled.
    	 */
        virtual TInt RunError( TInt aError );
        
    private:
        /**
         * constructor
         */
        CGlxTNMonitor( MGlxTNObserver *observer );

        /**
         * 2nd phase construction.
         */    
        void ConstructL();

    private:
        RProperty iScreenSaverProperty;
//		TTime iStartTime;
//		TTime iStopTime;
		MGlxTNObserver *iObserver;
};

	
#endif

