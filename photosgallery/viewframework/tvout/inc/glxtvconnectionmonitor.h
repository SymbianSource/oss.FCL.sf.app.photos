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
* Description:    Monitoring class of the Tv Out Connection
*
*/



/**
 * @internal reviewed 24/08/2007 by D Holland
 */

#ifndef __GLX_WNDWCONNECTION_MONITOR_H__
#define __GLX_WNDWCONNECTION_MONITOR_H__

// External Includes
#include <e32base.h>                // for CActive
#include <AknWsEventObserver.h>     // for MAknWsEventObserver
#include <AccessoryServer.h>        // for RAccessoryServer
#include <AccessoryMode.h>          // for RAccessoryMode


// Forward Declarations
class MGlxTvConnectionObserver;       


/**
 * Class Description
 * An Active object derived class is used to monitor the TV out connection
 * @author Loughlin
 */        
NONSHARABLE_CLASS(CGlxTvConnectionMonitor) : public CActive 
    {
public:

    /**
     * Static Symbian 2 stage constructor.
     */
    static CGlxTvConnectionMonitor* NewL(
                            MGlxTvConnectionObserver& aConnectionObserver);
    
    /**
     * Destructor.
     */
    ~CGlxTvConnectionMonitor();
    
private:

    /**
     * Standard C++ constructor
     */
    CGlxTvConnectionMonitor( 
                        MGlxTvConnectionObserver& aConnectionObserver);
    
    /*
     * Symbian second stage construction
     */
    void ConstructL(); 

public: // class member functions

    /*
     * Provides the caller with the current TV connetion state
     * @return The TV connection state
     */
    TBool IsConnected() const;


private: // From CActive
    /**
     * @ref CActive::RunL
     */	
	void RunL();
	
    /**
     * @ref CActive::DoCancel
     */	
	void DoCancel();
    
    /**
     * @ref CActive::RunError
     */    
    TInt RunError( TInt aError );    
    
    
private:

    /**
     * Requests TV on/off events 
     */        
    void IssueRequest();

    /**
     * Sends notification to observers when TV Out cable is connected
     */      
    void IssueNotificationL();
    

private: // class member data
    
    // Not owned: TV connection observer
    MGlxTvConnectionObserver& iConnectionObserver;

    // The Connection state 
    TBool iConnectionState;
    
    // The (external device) Accessory Server
    RAccessoryServer iTvAccServer;
    
    // Accessory mode 
    RAccessoryMode iTvAccMode;
    
    // Accessory Mode structure - details the type of accessory
    TAccPolAccessoryMode iCurrentAccMode; 

    };


#endif // __GLX_WNDWCONNECTION_MONITOR_H__