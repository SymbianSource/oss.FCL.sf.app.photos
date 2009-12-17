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
* Description:   Main application class
*
*/


// INCLUDE FILES

// Command Line and APA
#include <apgtask.h>
#include <apgcli.h>
#include <apaid.h>
#include <apacmdln.h>

#include <glxlog.h>

const TInt KPhotosSuiteUid ( 0x101f4cd2 );

// -----------------------------------------------------------------------------
// LaunchMenuViewL
// -----------------------------------------------------------------------------
//
void LaunchMenuViewL( const TDesC8& aMessage )
	{
	GLX_LOG_INFO("PhotosSuiteLauncher LaunchMenuViewL");
	RWsSession wssession;
	User::LeaveIfError(wssession.Connect());
	CleanupClosePushL(wssession);
	
  TApaTaskList taskList( wssession );    
  TApaTask task = taskList.FindApp( TUid::Uid( KPhotosSuiteUid ) );
  
  if ( task.Exists() )
      {
      TInt err = task.SendMessage( TUid::Uid( KUidApaMessageSwitchOpenFileValue ), 
        					aMessage );
	    }
	else
	    { // app not yet running
	    RApaLsSession appArcSession;
	    CleanupClosePushL( appArcSession );
	    User::LeaveIfError( appArcSession.Connect() ); 	    
      TApaAppInfo appInfo;
      TInt err = appArcSession.GetAppInfo( appInfo, 
      								TUid::Uid( KPhotosSuiteUid ) );
      if( err == KErrNone )
          {
          CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
          cmdLine->SetExecutableNameL( appInfo.iFullName );
          cmdLine->SetCommandL( EApaCommandRun );
          cmdLine->SetTailEndL( aMessage );

          appArcSession.StartApp( *cmdLine );
          CleanupStack::PopAndDestroy( cmdLine );
          }
	    CleanupStack::PopAndDestroy( &appArcSession ); 
	    }
  CleanupStack::PopAndDestroy( &wssession ); 
	}

// -----------------------------------------------------------------------------
// DoItL
// -----------------------------------------------------------------------------
//
void DoItL()
    {
    GLX_LOG_INFO("PhotosSuiteLauncher DoItL");	
    LaunchMenuViewL(_L8("mm://photossuite?exit=hide"));
    }


// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
    TRAPD(error,DoItL()); // more initialization, then do example 
    if(error != KErrNone)   
	    {
	    GLX_LOG_INFO1("PhotosSuiteLauncher DoItL Error %d",error);	
	    }
    delete cleanup; // destroy clean-up stack
    __UHEAP_MARKEND;
    return 0; // and return
    }


