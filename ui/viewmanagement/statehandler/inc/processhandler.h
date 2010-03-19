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


#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

// INCLUDES
#include <QObject>
#include <QProcess>

// FORWARD DECLARATIONS

// CLASS DECLARATION
class ProcessHandler : public QObject
    {
    Q_OBJECT
    
    public:
        ProcessHandler(QObject *parent = 0);
        ~ProcessHandler();
    
    public:
        void StartCameraApp();
        
    public Q_SLOTS:
        void stateChanged(QProcess::ProcessState state);
        void error(QProcess::ProcessError error);
        
    private: // Data
        QProcess*   process;

    }; 

#endif //PROCESSHANDLER_H
