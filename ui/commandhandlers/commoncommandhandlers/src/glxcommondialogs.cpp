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
#include <QEventLoop>

GlxTextInputDialog::GlxTextInputDialog() 
    : mDialog ( NULL ),
      mEventLoop ( 0 ),
      mResult ( false )
{
}

GlxTextInputDialog::~GlxTextInputDialog()
{
}

QString GlxTextInputDialog::getText(const QString &label,
        const QString &text, bool *ok)
{
    QEventLoop eventLoop;
    mEventLoop = &eventLoop;
    
    mDialog = new HbInputDialog();
    mDialog->setPromptText(label);
    mDialog->setInputMode(HbInputDialog::TextInput);
    mDialog->setValue(text);
    connect(mDialog->lineEdit(0), SIGNAL( textChanged (const QString &) ),
            this, SLOT( textChanged (const QString &)));
    
    mDialog->open( this, SLOT( dialogClosed( HbAction* ) ) ); 
    eventLoop.exec( );
    mEventLoop = 0 ;
    
    if ( ok ) {
        *ok = mResult ;
    }
    QString retText = NULL;
    if ( mResult ) {
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
    if (text.trimmed().isEmpty()) {
        mDialog->actions().first()->setEnabled(false);
    }
    else {
        mDialog->actions().first()->setEnabled(true);
    }
}

void GlxTextInputDialog::dialogClosed(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if( action == dlg->actions().first() ) {
        mResult = true ;
    }
    else {
        mResult = false ;
    }
    if ( mEventLoop && mEventLoop->isRunning( ) ) {
        mEventLoop->exit( 0 );
    }
}
