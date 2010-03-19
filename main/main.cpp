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
* Description:   ?Description
*
*/



#include <hbapplication.h>
#include <glxstatemanager.h>
#include <hbmainwindow.h>
#include <glxloggerenabler.h>

#include <hbstyle.h>
#include <hbstyleloader.h>
#include <QDebug>
#include <QTranslator>
#include <xqserviceutil.h>
#include <glxfetcher.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif

int main(int argc, char *argv[])
    {
    OstTrace0( TRACE_IMPORTANT, _MAIN, "::main" );
    QApplication::setGraphicsSystem("openvg");
    
    QTime localTime = QTime::currentTime();
    OstTraceExt3( TRACE_NORMAL, DUP1__MAIN, "::main Time at Launch HHMMSS =%d::%d::%d",
            localTime.hour(), localTime.minute(), localTime.second() );

    HbApplication app(argc, argv);	

    // TODO: this needs to be checked where translator _really_ should be installed
    QTranslator translator;
    translator.load("photos_" + QLocale::system().name());
    qApp->installTranslator(&translator);
    
    GlxStateManager* stateMgr = NULL;
    GlxFetcher* mainWindow = NULL;

    HbStyleLoader::load(":/data/photos.css");
    OstTraceEventStart0( EVENT_DUP1__MAIN_START, "launch" );

    if(!XQServiceUtil::isService()){
	stateMgr = new GlxStateManager();
    app.setApplicationName("Photos");          
    stateMgr->launchApplication();  
    }
    else
    {
	mainWindow = new GlxFetcher();
    mainWindow->show();
    }
    OstTraceEventStop( EVENT_DUP1__MAIN_STOP, "launch", EVENT_DUP1__MAIN_START );

	int ret = app.exec();
	delete stateMgr;
	delete mainWindow;
    return ret;
    }
