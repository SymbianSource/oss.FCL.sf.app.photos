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
* Description: This file contains testclass implementation.
*
*/

// INCLUDE FILES
#include <Stiftestinterface.h>
#include "ui_metaDataDialog.h"
#include <SettingServerClient.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cui_metaDataDialog::Cui_metaDataDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
Cui_metaDataDialog::Cui_metaDataDialog( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// Cui_metaDataDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Cui_metaDataDialog::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(Kui_metaDataDialogLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(Kui_metaDataDialogLogFile);
        }

    iLog = CStifLogger::NewL( Kui_metaDataDialogLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// Cui_metaDataDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Cui_metaDataDialog* Cui_metaDataDialog::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    Cui_metaDataDialog* self = new (ELeave) Cui_metaDataDialog( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
Cui_metaDataDialog::~Cui_metaDataDialog()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

//-----------------------------------------------------------------------------
// Cui_metaDataDialog::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void Cui_metaDataDialog::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("ui_metaDataDialog.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) Cui_metaDataDialog::NewL( aTestModuleIf );

    }

//  End of File
