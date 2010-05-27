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

#ifndef GLXMAINWINDOWEVENTFILTER_H
#define GLXMAINWINDOWEVENTFILTER_H
#include <QObject>
#include <QEvent>
class GlxMainWindowEventFilter : public QObject {
    Q_OBJECT
public:

    GlxMainWindowEventFilter() {}
    virtual bool eventFilter(QObject *o, QEvent *e) {
        Q_UNUSED(o);
        switch(e->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
            e->accept();
            return true;
        default:
            break;
        }
        return false;
    }

};

#endif