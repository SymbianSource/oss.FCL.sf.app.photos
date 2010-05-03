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

#include <glxcommondialogs.h>

#include <hbaction.h>

GlxTextInputDialog::GlxTextInputDialog()
    {
    }

GlxTextInputDialog::~GlxTextInputDialog()
    {
    }

QString GlxTextInputDialog::getText(const QString &label,
        const QString &text, bool *ok)
    {
    mDialog = new HbInputDialog();
    mDialog->setPromptText(label);
    mDialog->setInputMode(HbInputDialog::TextInput);
    mDialog->setValue(text);
    connect(mDialog->lineEdit(0), SIGNAL( textChanged (const QString &) ),
            this, SLOT( textChanged (const QString &)));
    HbAction* action = mDialog->exec();
    QString retText = NULL;
    if (action == mDialog->secondaryAction())
        { //Cancel was pressed
        if (ok)
            {
            *ok = false;
            }
        }
    else
        { //OK was pressed
        if (ok)
            {
            *ok = true;
            }
        retText = mDialog->value().toString().trimmed();
        }
    disconnect(mDialog->lineEdit(0), SIGNAL( textChanged (const QString &) ),
            this, SLOT( textChanged (const QString &)));
    delete mDialog;
    mDialog = NULL;
    return retText;
    }

void GlxTextInputDialog::textChanged(const QString &text)
    {
    if (text.trimmed().isEmpty())
        {
        mDialog->primaryAction()->setEnabled(false);
        }
    else
        {
        mDialog->primaryAction()->setEnabled(true);
        }
    }
