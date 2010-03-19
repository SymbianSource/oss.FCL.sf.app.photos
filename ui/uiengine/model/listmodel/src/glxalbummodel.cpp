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

#include <glxalbummodel.h>
#include <glxmodelparm.h>
#include <QDebug>
#include <hbicon.h>
#include <QStringList>

GlxAlbumModel::GlxAlbumModel(GlxModelParm & modelParm):mContextMode(GlxContextInvalid)
{
    qDebug("GlxAlbumModel::GlxAlbumModel()");
    
    mMLWrapper = new GlxMLWrapper(modelParm.collection(),modelParm.depth(),modelParm.filterType());
    // mMLWrapper->setContextMode(GlxContextPtList);
    // mContextMode = GlxContextPtList;
    mMLWrapper->setContextMode(GlxContextSelectionList);
    mContextMode = GlxContextSelectionList;
    
    //todo get this Default icon from some generic path and not directly.
    mDefaultIcon = new HbIcon(":/data/All_default.png");

	int err = connect(mMLWrapper, SIGNAL(updateItem(int, GlxTBContextType)), this, SLOT(itemUpdated1(int, GlxTBContextType)));
	qDebug("updateItem() connection status %d", err);
	err = connect(mMLWrapper, SIGNAL(insertItems(int, int)), this, SLOT(itemsAdded(int, int)));
	qDebug("insertItems() connection status %d", err);
	err = connect(mMLWrapper, SIGNAL(removeItems(int, int)), this, SLOT(itemsRemoved(int, int)));
	qDebug("removeItems() connection status %d", err);
    err = connect(this, SIGNAL(iconAvailable(int, HbIcon*, GlxTBContextType)), this, SLOT(updateItemIcon(int, HbIcon*, GlxTBContextType)));
	qDebug("iconAvailable() connection status %d", err);
    itemIconCache.setMaxCost(50);
}

GlxAlbumModel::~GlxAlbumModel()
{
    qDebug("GlxAlbumModel::~GlxAlbumModel()");
    delete mDefaultIcon;
    mDefaultIcon = NULL;
	int err = disconnect(mMLWrapper, SIGNAL(updateItem(int, GlxTBContextType)), this, SLOT(itemUpdated1(int, GlxTBContextType)));
	err = disconnect(mMLWrapper, SIGNAL(insertItems(int, int)), this, SLOT(itemsAdded(int, int)));
	err = disconnect(mMLWrapper, SIGNAL(removeItems(int, int)), this, SLOT(itemsRemoved(int, int)));
    err = disconnect(this, SIGNAL(iconAvailable(int, HbIcon*, GlxTBContextType)), this, SLOT(updateItemIcon(int, HbIcon*, GlxTBContextType)));
	delete mMLWrapper;
    mMLWrapper = NULL;
    itemIconCache.clear();
}

int GlxAlbumModel::rowCount(const QModelIndex &parent ) const
{
	Q_UNUSED(parent);
    qDebug("GlxAlbumModel::rowCount() %d", mMLWrapper->getItemCount());
    return ( mMLWrapper->getItemCount() );
}

int GlxAlbumModel::columnCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex GlxAlbumModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    //returns always invalid model index
    return QModelIndex();
}

QVariant GlxAlbumModel::data(const QModelIndex &index, int role) const
{
    qDebug("GlxAlbumModel::data %d", index.row());
    if(mContextMode == GlxContextInvalid)
        return QVariant();
    
    QStringList text;
    int rowIndex = index.row();
    HbIcon* itemIcon = NULL;
    QModelIndex idx;
    
    if ( (!index.isValid()) || (rowIndex >= rowCount()) ) {
        return QVariant();
    }
      
    switch ( role ) {
    case Qt::DisplayRole :
        text << mMLWrapper->retrieveListTitle(rowIndex) ;
        if(mContextMode != GlxContextSelectionList) {
            text<< mMLWrapper->retrieveListSubTitle(rowIndex) ; 
        }
        return text;
           
    case Qt::DecorationRole :
        if(mContextMode == GlxContextSelectionList){
            return HbIcon();
        }
        else
        {
            itemIcon = GetPreviewIconItem(rowIndex,GlxTBContextGrid);
            if(itemIcon){
                qDebug("GlxAlbumModel::data, Item Valid");
                return *itemIcon;
            }
            else {
                qDebug("GlxAlbumModel::data, Item inValid");
                itemIcon = mDefaultIcon;
                return *itemIcon;
            }
        }
		
    case GlxFocusIndexRole :
        idx = getFocusIndex();
        return idx.row();
           
    default :       
        return QVariant();
    }
}

bool GlxAlbumModel::setData ( const QModelIndex & idx, const QVariant & value, int role )
{
    Q_UNUSED( idx )
    qDebug("GlxAlbumModel::setData context role %d value %d", role, value.value <int> ());
    if ( GlxContextRole == role ) {
        if ( value.isValid() &&  value.canConvert <int> () ) {
            setContextMode( (GlxContextMode) value.value <int> () );
            return TRUE;
        }
    }
    
    if ( GlxFocusIndexRole == role ) {
        if ( value.isValid() &&  value.canConvert <int> () ) {
            setFocusIndex( index( value.value <int> (), 0) );
            return TRUE;
        }
    }
    
    if ( GlxSelectedIndexRole == role ) {
        if ( value.isValid() &&  value.canConvert <int> () ) {
            setSelectedIndex( index( value.value <int> (), 0) );
            return TRUE;
        }
    }
    return FALSE;
}

QModelIndex GlxAlbumModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    qDebug("GlxAlbumModel::index row %d column %d", row, column);
    
    if ( ( row < 0 )
        || ( row >= rowCount() )
        || ( column < 0 )
        || ( column >= columnCount() ) ) {
        return QModelIndex();
    }
    
    return QAbstractItemModel::createIndex(row, column);           
}


void GlxAlbumModel::setContextMode(GlxContextMode contextMode)
{
    if(mContextMode != contextMode) {
        mMLWrapper->setContextMode(contextMode);
        mContextMode = contextMode;
    }
}

void GlxAlbumModel::setFocusIndex(const QModelIndex &index)
{
    int itemIndex = index.row();
    qDebug("GlxAlbumModel::setFocusIndex()%d", itemIndex);    
    mMLWrapper->setFocusIndex(itemIndex);
}

void GlxAlbumModel::setSelectedIndex(const QModelIndex &index)
{
    int itemIndex = index.row();
    qDebug("GlxAlbumModel::setSelectedIndex()%d", itemIndex);    
    mMLWrapper->setSelectedIndex(itemIndex);
}

QModelIndex GlxAlbumModel::getFocusIndex() const
{
    return index(mMLWrapper->getFocusIndex(), 0);
}

void GlxAlbumModel::itemUpdated1(int mlIndex,GlxTBContextType tbContextType  )
{
	Q_UNUSED(tbContextType);
	qDebug("GlxAlbumModel::itemUpdated %d", mlIndex);
	
	emit dataChanged(index(mlIndex,0),index(mlIndex,0));
	
}

void GlxAlbumModel::itemsAdded(int startIndex, int endIndex)
{
    qDebug("GlxAlbumModel::itemsAdded %d %d", startIndex, endIndex);
    beginInsertRows(QModelIndex(), startIndex, endIndex);
    endInsertRows();	
}

void GlxAlbumModel::itemsRemoved(int startIndex, int endIndex)
{
	qDebug("GlxAlbumModel::itemsRemoved %d %d", startIndex, endIndex);
	beginRemoveRows(QModelIndex(), startIndex, endIndex);
   	endRemoveRows();
	//emit rowsRemoved(index(startIndex,0), startIndex, endIndex );
	
}

HbIcon* GlxAlbumModel::GetPreviewIconItem(int itemIndex, GlxTBContextType tbContextType) const
{
	HbIcon* itemIcon = itemIconCache[itemIndex];  //Initialize icon from the Cache will be NULL if Item not present
	if (!itemIcon)
	{
		itemIcon =  mMLWrapper->retrieveItemIcon(itemIndex, tbContextType);
		if(itemIcon){
            emit iconAvailable(itemIndex,itemIcon, GlxTBContextGrid);
		}
		itemIcon = itemIconCache[itemIndex];
	}
	return itemIcon;

}

void GlxAlbumModel::updateItemIcon(int itemIndex, HbIcon* itemIcon, GlxTBContextType tbContextType)
{
	qDebug("GlxAlbumModel::updateItemIcon %d", itemIndex);
	if(tbContextType == GlxTBContextGrid){
		itemIconCache.insert(itemIndex, itemIcon);
	}
}
