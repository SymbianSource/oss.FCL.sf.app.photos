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

#ifndef GLXCOMMONDIALOGS_H
#define GLXCOMMONDIALOGS_H

#include <QObject>

#include <hbinputdialog.h>

class GlxTextInputDialog : public QObject
{
	Q_OBJECT    
public:
    GlxTextInputDialog();
    ~GlxTextInputDialog();
    QString getText(const QString &label,const QString &text = QString(),bool *ok = 0);

public slots:
    void textChanged(const QString &text);

private:
    HbInputDialog* mDialog;
};

#endif // GLXCOMMONDIALOGS_H
