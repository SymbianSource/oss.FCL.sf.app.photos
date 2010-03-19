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

#include <QtGui>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbstyle.h>
#include <hbstyleloader.h>

#include <glxstatemanager.h>
#include <glximageviewermanager.h>


int main(int argc, char *argv[])
{
    // Initialization
    HbApplication app(argc, argv);
    QStringList args = QApplication::arguments();

#ifdef _DEBUG	
    foreach (QString arg, args)
    {
	qDebug() << "GlxImageViewer: cmdline arg=" << qPrintable(arg);
    }
#endif	

    CGlxImageViewerManager* imageViewerInstance = CGlxImageViewerManager::InstanceL();
	
	QString arg = args[1];
	arg.replace(QString("/"), QString("\\"));
	
	TPtrC16 str(reinterpret_cast<const TUint16*>(arg.utf16()));
	HBufC* uri = str.Alloc();
	
	bool isPrivatepath = arg.contains("\\private\\", Qt::CaseInsensitive);  ;

	RFile fileHandle;
    RFs fs;
    

    if(!isPrivatepath){
        imageViewerInstance->SetImageUriL(*uri);
    }
   else{
       fs.Connect();
       TInt err = fileHandle.Open( fs, *uri, EFileShareReadersOnly );
       if ( err )
           {
           User::LeaveIfError( fileHandle.Open( fs, *uri, EFileShareAny ) );
           }
       imageViewerInstance->SetImageFileHandleL(fileHandle);
    }
    
    GlxStateManager mStateMgr;
    mStateMgr.launchFromExternal();
    
    // Enter event loop
    int ret = app.exec();
    
    fileHandle.Close();
    fs.Close();

    imageViewerInstance->Close();
    return ret;
}
