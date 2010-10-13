/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Command object factory
*
*/




#ifndef SHWMEDIAKEYUTILITY_H
#define SHWMEDIAKEYUTILITY_H

#include <remconcoreapitargetobserver.h>    // link against RemConCoreApi.lib
#include <remconcoreapitarget.h>            // and
#include <remconinterfaceselector.h>        // RemConInterfaceBase.lib


/**
 *
 * This is a utility class that simplifies the use of RemCon and the ViaPlayer when
 * used together. A problem results when an application instantiates more than one
 * Instance of RemCon.
 *
 * This class enables other components to use the same instance on RemCon that the 
 * ViaPlayer uses.
 * It inherits from MViaPlayerMediaKeyListener and links with the ViaPlayerUtilities.lib
 * It interperets the keys from RemCon via the MediaKeyEventL method and translates them
 * to standard key which get added to the application event queue for normal processing
 * (Via OfferKeyEventL).
 *
 * Note that TranslateKeyEvent and DispatchKeyEvent methods are virtual.
 * So if different behaviour is required just create a Sub class and overide
 * those metheds are needed.
 * 
 */

class CShwMediaKeyUtility : public CBase, public MRemConCoreApiTargetObserver
    {
public:
    /**
    * Standard NewLC class used for Instantiation
    
    * @return Instance of Itself, also on the cleanup stack
    */
    static CShwMediaKeyUtility* NewLC();
    
    /**
    * Standard NewL class used for Instantiation
    
    * @return Instance of Itself
    */
    static CShwMediaKeyUtility* NewL();
    
    /**
    * Destructor
    */
    virtual ~CShwMediaKeyUtility();

protected:
    /**
    * Creates KeyEvent and places it on the application event queue.
    *
    * @param aKeyCode this is the action (up, down or click).
    * @param aScanCode this is the TStdScanCode for the Key Event.
    */
    void DoSimulateKeyEvent( TRawEvent::TType aKeyCode, TStdScanCode aScanCode );

    /**
    * Converts RemCon key types to standard key types
    *
    * @param aOperationId the RemCon key type
    *
    * @return the standard key type
    */
    virtual TStdScanCode TranslateKeyEvent(TRemConCoreApiOperationId aOperationId);
    
    /**
    * Places the Key Event in the applications quere acording to aButtonAct (up, down, click)
    *
    * @param aButtonAct (up, down or click)
    * @param aScanCode the standard scan code for a key
    */
    virtual void DispatchKeyEvent( TRemConCoreApiButtonAction aButtonAct, TStdScanCode aScanCode);

private:
    CShwMediaKeyUtility();
    void ConstructL();
    
public: // from MViaPlayerMediaKeyListener

   void MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
                         TRemConCoreApiButtonAction aButtonAct);    
private:

    CRemConInterfaceSelector* iInterfaceSelector;
    CRemConCoreApiTarget*     iCoreTarget;
    };

#endif//


