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



#ifndef GLXMODELPARM_H
#define GLXMODELPARM_H

#include<glxfiltergeneraldefs.h>

enum
{
GlxFsImageRole = Qt::UserRole + 1,
GlxContextRole,
GlxFocusIndexRole,
GlxVisualWindowIndex, 
GlxUriRole,
GlxDimensionsRole,
GlxSelectedIndexRole,
GlxDateRole,
GlxFrameCount
};


class GlxModelParm
{
public:
	GlxModelParm (qint32 collectionId = 0, qint32 depth = 0 , TGlxFilterItemType filterType = EGlxFilterItemAll ) : mCollectionId(collectionId) ,mDepth(depth) , mFilterType(filterType)
	{
	
	}
	
	void setCollection (qint32 collectionId = 0) { mCollectionId = collectionId ; }
	qint32 collection() { return mCollectionId; }
	void setDepth (qint32 depth = 0) { mDepth = depth ; }
	qint32 depth() { return mDepth; }
	TGlxFilterItemType filterType() { return mFilterType; }
private:
	qint32 mCollectionId;
	qint32 mDepth;
	TGlxFilterItemType mFilterType;
};


#endif /* GLXMODELPARM_H_ */
