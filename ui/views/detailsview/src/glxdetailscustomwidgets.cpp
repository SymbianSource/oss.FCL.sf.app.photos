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

#include "glxdetailscustomwidgets.h"
#include <hbdataform.h>
#include <hbdataformmodelitem.h> 
#include <hbdataformmodel.h> 
#include <hbdataformviewitem.h>

#include <hblineedit.h>
#include <hblabel.h>
//#include "hbsettingformitem_p.h"
#include <hbpushbutton.h>
#include <hbiconitem.h>
#include <QDir>
#include <QGraphicsLinearLayout>

GlxDetailsCustomWidgets::GlxDetailsCustomWidgets(QGraphicsItem *parent):
HbDataFormViewItem(parent)
     {
     }

GlxDetailsCustomWidgets::~GlxDetailsCustomWidgets()
    {
    }

HbAbstractViewItem* GlxDetailsCustomWidgets::createItem()
    {
    return new GlxDetailsCustomWidgets(*this);
    }

HbWidget* GlxDetailsCustomWidgets::createCustomWidget()
    {
    qDebug("GlxDetailsCustomWidgets::createCustomWidget");
    HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>(
            modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
    switch(itemType)
        {
        case ImageNameItem:
            {
            qDebug("GlxDetailsCustomWidgets::createCustomWidget:ImageNameItem");
            HbLineEdit *lineEdit = new HbLineEdit();
            lineEdit->setGeometry(25,25,280,120);
            return lineEdit;
            }

        case DateLabelItem :
            {
            qDebug("GlxDetailsCustomWidgets::createCustomWidget:DateLabelItem");
            HbLabel *dateLabel = new HbLabel();
            return dateLabel;
            }

        case LocationTagItem:
            {
            qDebug("GlxDetailsCustomWidgets::createCustomWidget:LocationTagItem");
            HbLabel *locationLabel = new HbLabel();
            return locationLabel;
            }
            
        case DurationItem:
            {
            qDebug("GlxDetailsCustomWidgets::createCustomWidget:DurationLabelItem");
            HbLabel *durationLabel = new HbLabel();
            return durationLabel;
            }

        case ShareWithItem:
            {
            //To be added later.
            }
         
        case CommentsItem:
            {
            qDebug("GlxDetailsCustomWidgets::createCustomWidget:CommentsItem");
            HbLineEdit *lineEdit = new HbLineEdit();
            lineEdit->setGeometry(25,25,280,120);
            return lineEdit;
            }

        default:
            return 0;

        }
    }


void  GlxDetailsCustomWidgets::SetImageName(const QString &label)
    {
    qDebug("GlxDetailsCustomWidgets::SetImageName:mLabel ");
    mLabel->setPlainText(label);
    }

//EOF
