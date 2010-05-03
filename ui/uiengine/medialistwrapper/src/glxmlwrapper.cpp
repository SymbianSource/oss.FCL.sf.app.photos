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

/*glxmlwrapper.cpp*/

//internal includes 
#include "glxmlwrapper.h"
#include "glxmlwrapper_p.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// constructor.
// ---------------------------------------------------------------------------
//
GlxMLWrapper::GlxMLWrapper(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType)
{
    mMLWrapperPrivate = GlxMLWrapperPrivate::Instance(this, aCollectionId, aHierarchyId, aFilterType);
}

// ---------------------------------------------------------------------------
// destructor.
// ---------------------------------------------------------------------------
//
GlxMLWrapper::~GlxMLWrapper()
{
	delete mMLWrapperPrivate;
}

// ---------------------------------------------------------------------------
// setContextMode.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::setContextMode(GlxContextMode contextMode)
{
	mMLWrapperPrivate->SetContextMode(contextMode);
}

// ---------------------------------------------------------------------------
// getItemCount.
// ---------------------------------------------------------------------------
//
int GlxMLWrapper::getItemCount()
{
	return (mMLWrapperPrivate->GetItemCount());
}

// ---------------------------------------------------------------------------
// retrieveItemIcon.
// ---------------------------------------------------------------------------
//
HbIcon* GlxMLWrapper::retrieveItemIcon(int index, GlxTBContextType aTBContextType)
{
	return (mMLWrapperPrivate->RetrieveItemIcon(index,aTBContextType));

}

// ---------------------------------------------------------------------------
// retrieveItemIcon.
// ---------------------------------------------------------------------------
//
QString GlxMLWrapper::retrieveListTitle(int index)
{
	return (mMLWrapperPrivate->RetrieveListTitle(index));
}

// ---------------------------------------------------------------------------
// retrieveItemIcon.
// ---------------------------------------------------------------------------
//
QString GlxMLWrapper::retrieveListSubTitle(int index)
{
	return (mMLWrapperPrivate->RetrieveListSubTitle(index));
}
// ---------------------------------------------------------------------------
// retrieveItemUri.
// ---------------------------------------------------------------------------
//
QString GlxMLWrapper::retrieveItemUri(int index)
{
	return (mMLWrapperPrivate->RetrieveItemUri(index));
}
// ---------------------------------------------------------------------------
// retrieveItemDimension.
// ---------------------------------------------------------------------------
//
QSize GlxMLWrapper::retrieveItemDimension(int index)
{
	return (mMLWrapperPrivate->RetrieveItemDimension(index));
}

QDate GlxMLWrapper::retrieveItemDate(int index)
{
	return (mMLWrapperPrivate->RetrieveItemDate(index));
}

int GlxMLWrapper::retrieveItemFrameCount(int index)
{
    return (mMLWrapperPrivate->RetrieveItemFrameCount(index));
}

QVariant GlxMLWrapper::RetrieveBitmap(int index)
{
    QVariant var;
    var.setValue(mMLWrapperPrivate->RetrieveBitmap(index));
    return var;
}

// ---------------------------------------------------------------------------
// getFocusIndex.
// ---------------------------------------------------------------------------
//
int GlxMLWrapper::getFocusIndex() const
{
    return (mMLWrapperPrivate->GetFocusIndex());
}

// ---------------------------------------------------------------------------
// setFocusIndex.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::setFocusIndex(int itemIndex)
{
    mMLWrapperPrivate->SetFocusIndex(itemIndex);
}

// ---------------------------------------------------------------------------
// SetSelectedIndex.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::setSelectedIndex(int itemIndex)
{
    mMLWrapperPrivate->SetSelectedIndex(itemIndex);
}

// ---------------------------------------------------------------------------
// getVisibleWindowIndex.
// ---------------------------------------------------------------------------
//
int GlxMLWrapper::getVisibleWindowIndex()
{
    return (mMLWrapperPrivate->GetVisibleWindowIndex());
}

// ---------------------------------------------------------------------------
// setVisibleWindowIndex.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::setVisibleWindowIndex(int itemIndex)
{
    mMLWrapperPrivate->SetVisibleWindowIndex(itemIndex);
}


// ---------------------------------------------------------------------------
// itemsAdded.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::itemsAdded(int startIndex,int endIndex)
{
	emit insertItems(startIndex,endIndex);
}

// ---------------------------------------------------------------------------
// itemsRemoved.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::itemsRemoved(int startIndex,int endIndex)
{
	emit removeItems(startIndex,endIndex);
}

// ---------------------------------------------------------------------------
// handleReceivedIcon.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleReceivedIcon(int itemIndex, GlxTBContextType tbContextType)
{
	emit updateItem(itemIndex, tbContextType);
}


// ---------------------------------------------------------------------------
// handleIconCorrupt.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleIconCorrupt(int itemIndex)
{
	emit itemCorrupted(itemIndex);
}

// ---------------------------------------------------------------------------
// handleListItemAvailable.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleListItemAvailable(int itemIndex)
{
	emit updateItem(itemIndex, GlxTBContextNone);
}


// ---------------------------------------------------------------------------
// handleGeneralError.
// ---------------------------------------------------------------------------
//
void GlxMLWrapper::handleGeneralError(int aError)
{
	Q_UNUSED(aError);
}


