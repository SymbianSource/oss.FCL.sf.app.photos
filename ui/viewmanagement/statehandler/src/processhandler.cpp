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


#include "processhandler.h"
#include <QDebug>
#include <QtDebug>

ProcessHandler::ProcessHandler(QObject *parent)
: QObject(parent)
    {
    qDebug() << "ProcessHandler::ProcessHandler";
    process = new QProcess(this);
    // Start listening process state changes
    QObject::connect(process, SIGNAL(stateChanged(QProcess::ProcessState)),
                     this, SLOT(stateChanged(QProcess::ProcessState)));
    }

ProcessHandler::~ProcessHandler()
    {
    qDebug() << "ProcessHandler::~ProcessHandler";
    if (process->state() != QProcess::NotRunning)
        {
        // Close process if it is running
        process->close();
        }
    }

void ProcessHandler::StartCameraApp()
    {
    qDebug() << "ProcessHandler::StartCameraApp";
    QString program = "cxui.exe";
    process->start(program);    
    }

void ProcessHandler::error(QProcess::ProcessError error)
    {
    switch (error)
        {
        case QProcess::FailedToStart:
            {
            qDebug() << "FailedToStart";
            break;
            }
        case QProcess::Crashed:
            {
            qDebug() << "Crashed";
            break;
            }
        case QProcess::Timedout:
            {
            qDebug() << "Timedout";
            break;
            }
        case QProcess::ReadError:
            {
            qDebug() << "ReadError";
            break;
            }
        case QProcess::WriteError:
            {
            qDebug() << "WriteError";
            break;
            }
        case QProcess::UnknownError:
            {
            qDebug() << "UnknownError";
            break;
            }
        default:
            {
            break;
            }
        };
    }

void ProcessHandler::stateChanged(QProcess::ProcessState state)
    {
    switch (state)
        {
        case QProcess::NotRunning:
            {
            qDebug() << "NotRunning";
            break;
            }
        case QProcess::Starting:
            {
            qDebug() << "Starting";
            break;
            }
        case QProcess::Running:
            {
            qDebug() << "Running";
            break;
            }
        default:
            {
            break;
            }
        };
    }
