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
        
		case DateLabelItem :
            {
            qDebug("GlxDetailsCustomWidgets::createCustomWidget:DateLabelItem");
            HbLabel *dateLabel = new HbLabel();
            return dateLabel;
            }
        


        default:
            return 0;

        }
    }

bool GlxDetailsCustomWidgets::canSetModelIndex(const QModelIndex &index) const
{
    HbDataFormModelItem::DataItemType itemType =
    static_cast<HbDataFormModelItem::DataItemType>(
            index.data(HbDataFormModelItem::ItemTypeRole).toInt());

    if(itemType >= ImageNameItem && 
            itemType <= CommentsItem )
        {
        return true;
        }
    return false;
}

//EOF
