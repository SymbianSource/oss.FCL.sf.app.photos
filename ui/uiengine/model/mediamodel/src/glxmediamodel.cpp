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



#include <glxmediamodel.h>
#include <hbicon.h>
#include <glxmodelparm.h>
#include <QCache>
#include <QDebug>
#include <hal.h>
#include <hal_data.h>

#include <glxfiltergeneraldefs.h>

//#define GLXPERFORMANCE_LOG  
#include <glxperformancemacro.h>

#include "glxicondefs.h" //Contains the icon names/Ids

GlxMediaModel::GlxMediaModel(GlxModelParm & modelParm)
{
	qDebug("GlxMediaModel::GlxMediaModel");
	
	mMLWrapper = new GlxMLWrapper(modelParm.collection(),0,EGlxFilterImage);
	mMLWrapper->setContextMode( modelParm.contextMode() );
	mContextMode = modelParm.contextMode( ) ; 
	
	int err = connect(mMLWrapper, SIGNAL(updateItem(int, GlxTBContextType)), this, SLOT(itemUpdated1(int, GlxTBContextType)));
	qDebug("updateItem() connection status %d", err);
	err = connect(mMLWrapper, SIGNAL(itemCorrupted(int)), this, SLOT(itemCorrupted(int)));
	qDebug("itemCorrupted() connection status %d", err);
	err = connect(mMLWrapper, SIGNAL(insertItems(int, int)), this, SLOT(itemsAdded(int, int)));
	qDebug("updateItem() connection status %d", err);
	err = connect(mMLWrapper, SIGNAL(removeItems(int, int)), this, SLOT(itemsRemoved(int, int)));
	qDebug("updateItem() connection status %d", err);
	err = connect(this, SIGNAL(iconAvailable(int, HbIcon*, GlxTBContextType)), this, SLOT(updateItemIcon(int, HbIcon*, GlxTBContextType)));
	qDebug("iconAvailable() connection status %d", err);
	//itemadded.resize(mMLWrapper->getItemCount());
	
	itemIconCache.setMaxCost(50);
	itemFsIconCache.setMaxCost(5);
	itemExternalIconCache.setMaxCost(0);
	
	//todo get this Default icon from some generic path and not directly.
	m_DefaultIcon = new HbIcon(GLXICON_DEFAULT);
	mExternalItems = NULL;
	externalDataCount = 0;
	mFocusIndex = -1;
	mSubState = -1;
}

GlxMediaModel::~GlxMediaModel()
{
    //itemadded.clear();
	qDebug("GlxMediaModel::~GlxMediaModel");
	int freeMemory = 0;
	int err1 = HAL::Get( HALData::EMemoryRAMFree, freeMemory );
	qDebug("####mediaModel : Memory available before cache cleanup  = %d and error is = %d ", freeMemory , err1 );
	itemIconCache.clear();
	itemFsIconCache.clear();
	delete m_DefaultIcon;
	m_DefaultIcon = NULL;
	clearExternalItems();
	err1 = HAL::Get( HALData::EMemoryRAMFree, freeMemory );
	qDebug("####mediaModel : Memory available after cache cleanup  = %d and error is = %d ", freeMemory , err1 );
    int err = disconnect(mMLWrapper, SIGNAL(updateIcon(int, HbIcon*)), this, SLOT(itemUpdated1(int, HbIcon*)));
	err = disconnect(mMLWrapper, SIGNAL(itemCorrupted(int)), this, SLOT(itemCorrupted(int)));
	err = disconnect(mMLWrapper, SIGNAL(insertItems(int, int)), this, SLOT(itemsAdded(int, int)));
	err = disconnect(mMLWrapper, SIGNAL(removeItems(int, int)), this, SLOT(itemsRemoved(int, int)));
	err = disconnect(this, SIGNAL(iconAvailable(int, HbIcon*, GlxTBContextType)), this, SLOT(updateItemIcon(int, HbIcon*, GlxTBContextType)));
    delete mMLWrapper;

}

void GlxMediaModel::setContextMode(GlxContextMode contextMode)
{
	if(mContextMode != contextMode)	{
		itemFsIconCache.clear();
		mMLWrapper->setContextMode(contextMode);
		mContextMode = contextMode;
		if ( mContextMode == GlxContextLsGrid || mContextMode == GlxContextPtGrid ) {
            itemIconCache.clear();
            emit dataChanged( index( 0, 0), index( rowCount() - 1, 0) );
        }
	}
}

//to add external data to the model
void GlxMediaModel::addExternalItems(GlxExternalData* externalItems)
{
	externalDataCount = externalItems->count();
	beginInsertRows(QModelIndex(), 0, externalDataCount);
	mExternalItems = externalItems;
	itemExternalIconCache.setMaxCost(15);
	endInsertRows();
	setFocusIndex(index(0,0));
}


/*
*for removing all external image data from the model. 
*/
void GlxMediaModel::clearExternalItems()
{
	beginRemoveRows(QModelIndex(), 0, externalDataCount);
	if(mExternalItems) {
		delete mExternalItems;
		mExternalItems = NULL;
		externalDataCount = 0;
	}
	itemExternalIconCache.clear();
	endRemoveRows();
}

int GlxMediaModel::rowCount(const QModelIndex &parent ) const
{
	Q_UNUSED(parent);
	qDebug("GlxMediaModel::rowCount %d", mMLWrapper->getItemCount());
    return (mMLWrapper->getItemCount() + externalDataCount);
}

int GlxMediaModel::columnCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex GlxMediaModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    //returns always invalid model index
    return QModelIndex();
}


//todo refactor this whole function ... too many return statements are not good
QVariant GlxMediaModel::data(const QModelIndex &index, int role) const
{
    HbIcon* itemIcon = NULL;
    int itemIndex = index.row();
    qDebug("GlxMediaModel::data buffer concept index %d role %d", index.row(), role);
    if ((!index.isValid()) || (index.row() > rowCount()-1)) {
         return QVariant();
    }
    
//external data are always placed at the beginning of the Media List
//Check if the index can be mapped to the external data
//if not then map the index to Ml Index
    if(itemIndex < externalDataCount) {
        if(role == Qt::DecorationRole || role == GlxFsImageRole){
            return *(GetExternalIconItem(itemIndex,GlxTBContextExternal));
        }
    }
    else {
        itemIndex -=  externalDataCount;
    }
    
//retrieve Data from Media List		
    if (role == Qt::DecorationRole) {
        itemIcon = GetGridIconItem(itemIndex,GlxTBContextGrid);
        if(itemIcon == NULL || itemIcon->isNull() ){
            itemIcon = m_DefaultIcon;
        // }
        }
        return *itemIcon;
    }
		
    if (role == GlxFsImageRole){
        if(mContextMode == GlxContextLsFs){
            itemIcon = GetFsIconItem(itemIndex,GlxTBContextLsFs);
        }
        else if (mContextMode == GlxContextPtFs){
            itemIcon = GetFsIconItem(itemIndex,GlxTBContextPtFs);
        }
        
        if ( itemIcon == NULL) {
            //itemIcon = GetGridIconItem(itemIndex,GlxTBContextGrid);    
            HbIcon* tempIcon = GetGridIconItem( itemIndex, GlxTBContextGrid );
            if (tempIcon && !tempIcon->isNull()) {
                qDebug("GlxMediaModel::scaling thumbnail");
                QPixmap tempPixmap = tempIcon->qicon().pixmap(128, 128);
                QSize sz = ( mContextMode == GlxContextLsFs ) ? QSize ( 640, 360) : QSize ( 360, 640 );
                tempPixmap = tempPixmap.scaled(sz, Qt::KeepAspectRatio );
                HbIcon tmp = HbIcon( QIcon(tempPixmap)) ;
				if(!tmp.isNull()){
					return tmp;
				}
            }
        }
		  
        if ( itemIcon == NULL || itemIcon->isNull() ) {
            itemIcon = m_DefaultIcon;
        }
        return *itemIcon;
    }
	
    QModelIndex idx;
    if ( GlxFocusIndexRole == role ) {
        idx = getFocusIndex();
        return idx.row();
    }
    if(role == GlxUriRole) {
        return (mMLWrapper->retrieveItemUri(itemIndex));
    }
    if(role == GlxDimensionsRole) {
        return (mMLWrapper->retrieveItemDimension(itemIndex));
    }
    
    if(role == GlxDateRole ) {
    	qDebug("GlxMediaModel::data GlxDateRole ");
    	return (mMLWrapper->retrieveItemDate(itemIndex));
    }
    
    if (role == GlxFrameCount) {
    qDebug("GlxMediaModel:: GlxFrameCount ");
    return (mMLWrapper->retrieveItemFrameCount(itemIndex));
    }
    
    if ( role == GlxSubStateRole ) {
        return mSubState;
    }
	    
    return QVariant();
}

//GetGridIconItem
HbIcon* GlxMediaModel::GetGridIconItem(int itemIndex, GlxTBContextType tbContextType) const
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

HbIcon* GlxMediaModel::GetFsIconItem(int itemIndex, GlxTBContextType tbContextType) const
{
	HbIcon* itemIcon = itemFsIconCache[itemIndex];  //Initialize icon from the Cache will be NULL if Item not present
	if (!itemIcon) {
		itemIcon =  mMLWrapper->retrieveItemIcon(itemIndex, tbContextType);
		if(itemIcon){
            emit iconAvailable(itemIndex,itemIcon, tbContextType);
		}
		itemIcon = itemFsIconCache[itemIndex];
		/*if(!itemIcon){
		itemIcon = GetGridIconItem(itemIndex, GlxTBContextGrid);
		}*/
	}
	return itemIcon;

}

HbIcon* GlxMediaModel::GetExternalIconItem(int itemIndex,GlxTBContextType tbContextType) const
{
	//todo add asserts here as the cache and the array is not preinitialized
	HbIcon* itemIcon = itemExternalIconCache[itemIndex];  //Initialize icon from the Cache will be NULL if Item not present
	if (!itemIcon) {
		QPixmap* itemPixmap = mExternalItems->at(itemIndex).image;
		itemIcon = new HbIcon(QIcon(*itemPixmap));
		emit iconAvailable(itemIndex,itemIcon, tbContextType);
	}
	return itemIcon;
}


void GlxMediaModel::itemUpdated1(int mlIndex,GlxTBContextType tbContextType  )
{
	qDebug("GlxMediaModel::itemUpdated %d", mlIndex);
	//clear the grid and FS cache if they have any icons with them for that index
	if(tbContextType == GlxTBContextGrid) {
		itemIconCache.remove(mlIndex);
	}
	if(tbContextType == GlxTBContextLsFs || tbContextType == GlxTBContextPtFs) {
		itemFsIconCache.remove(mlIndex);
	}
	emit dataChanged(index(mlIndex+externalDataCount,0),index(mlIndex+externalDataCount,0));
	
}

void GlxMediaModel::itemCorrupted(int itemIndex)
{
	qDebug("GlxMediaModel::itemCorrupted %d", itemIndex);
	emit dataChanged(index(itemIndex+externalDataCount,0),index(itemIndex+externalDataCount,0));	
}

void GlxMediaModel::itemsAdded(int startIndex, int endIndex)
{
	qDebug("GlxMediaModel::itemsAdded %d %d", startIndex, endIndex);
	mFocusIndex = -1;
	//emit rowsAboutToBeInserted(index(startIndex,0), startIndex, endIndex );
	beginInsertRows(QModelIndex(), startIndex+externalDataCount, endIndex+externalDataCount);
    //itemadded.insert(startIndex,(endIndex - startIndex),NULL );
	itemIconCache.clear();
	itemFsIconCache.clear();
    endInsertRows();
	//emit rowsInserted(index(startIndex,0), startIndex, endIndex );
	
}

void GlxMediaModel::itemsRemoved(int startIndex, int endIndex)
{
	qDebug("GlxMediaModel::itemsRemoved %d %d", startIndex, endIndex);
	mFocusIndex = -1;
	//emit rowsAboutToBeRemoved(index(startIndex,0), startIndex, endIndex );
	beginRemoveRows(QModelIndex(), startIndex+externalDataCount, endIndex+externalDataCount);
	//itemadded.remove(startIndex,(endIndex - startIndex) );
	itemIconCache.clear();
	itemFsIconCache.clear();
	endRemoveRows();
	//emit rowsRemoved(index(startIndex,0), startIndex, endIndex );
	
}

void GlxMediaModel::updateItemIcon(int itemIndex, HbIcon* itemIcon, GlxTBContextType tbContextType)
{
	qDebug("GlxMediaModel::updateItemIcon %d", itemIndex);
	if(tbContextType == GlxTBContextGrid){
		itemIconCache.insert(itemIndex, itemIcon);
	}
	else if(tbContextType == GlxTBContextLsFs || tbContextType == GlxTBContextPtFs){
		itemFsIconCache.insert(itemIndex, itemIcon);
	}
	else if (tbContextType == GlxTBContextExternal){
		itemExternalIconCache.insert(itemIndex, itemIcon);	
	}
}


void GlxMediaModel::setFocusIndex(const QModelIndex &index)
{
	qDebug("GlxMediaModel::setFocusIndex%d", index.row());
	int itemIndex = index.row();
	
	if(itemIndex >=externalDataCount)
	{
		int mlIndex = itemIndex - externalDataCount;
		mMLWrapper->setFocusIndex(mlIndex);
		mFocusIndex = -1;
	}
	else{
		mFocusIndex = itemIndex;
		if(rowCount() > externalDataCount) {
			mMLWrapper->setFocusIndex(0);
		}
		
	}

}

QModelIndex GlxMediaModel::getFocusIndex() const
{
	int itemIndex = 0;
	//return index(itemIndex+externalDataCount,0);
	if(mFocusIndex == -1){
		int mlIndex = mMLWrapper->getFocusIndex();
		itemIndex = mlIndex + externalDataCount;
	}
	else {
		itemIndex = mFocusIndex;
	}
	return index(itemIndex,0);
}

void GlxMediaModel::setSelectedIndex(const QModelIndex &index)
{
    int itemIndex = index.row();
    qDebug("GlxMediaModel::setSelectedIndex()%d", itemIndex);    
    mMLWrapper->setSelectedIndex(itemIndex);
}

bool GlxMediaModel::setData ( const QModelIndex & idx, const QVariant & value, int role )
{
    Q_UNUSED( idx )
    
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
    
    if ( GlxVisualWindowIndex == role ) {
        if ( value.isValid() &&  value.canConvert <int> () ) {
            mMLWrapper->setVisibleWindowIndex(  value.value <int> () );
            return TRUE;
        }
    }

    if ( GlxSelectedIndexRole == role ) {
        qDebug("GlxMediaModel::setData::GlxSelectedIndexRole()::BEFORE%d", idx.row());    
        if ( value.isValid() &&  value.canConvert <int> () ) {
        qDebug("GlxMediaModel::setData()::GlxSelectedIndexRole%d", idx.row());    
            setSelectedIndex( index( value.value <int> (), 0) );
            return TRUE;
        }
    }
    
    if ( role == GlxSubStateRole && value.isValid() &&  value.canConvert <int> ()) {
        mSubState = value.value <int> () ;
        return TRUE;
    }

    return FALSE;
}

QModelIndex GlxMediaModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    qDebug("GlxMediaModel::index row %d column %d", row, column);
    
    if ( ( row < 0 )
        || ( row >= rowCount() )
        || ( column < 0 )
        || ( column >= columnCount() ) ) {
        return QModelIndex();
    }
    
    return QAbstractItemModel::createIndex(row, column);           
}

