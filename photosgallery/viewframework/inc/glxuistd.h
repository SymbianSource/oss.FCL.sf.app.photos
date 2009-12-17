/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Viewer app general definitions
*
*/




#ifndef GLXUISTD_H
#define GLXUISTD_H

enum TGlxHuiCustomCommands 
	{
	EGlxHuiCustomEventBackToZeroSpeed = 200,
	EGlxHuiCustomEventBackToSharpPath
	};

_LIT(KGlxUiUtilitiesResource, "glxuiutilities.rsc");

_LIT(KGlxIconsFilename, "glxicons.mif");

/** View activation parameters */
_LIT8(KGlxActivationFullScreen, "fullscreen");


/** attribute/thumbnail fetch context priority definitions */

const TInt KGlxFetchContextPriorityLow = 0;
const TInt KGlxFetchContextPriorityNormal = 1;

// cloud view fetch context priorities
const TInt KGlxFetchContextPriorityCloudView = 4;
const TInt KGlxFetchContextPriorityCloudViewInfoBubble = 3;

// UMP view fetch context priorities
const TInt KGlxFetchContextPriorityUMPViewAlbumPane = 2;
const TInt KGlxFetchContextPriorityUMPViewTagPane = 3;

// tile view fetch context priorities
const TInt KGlxFetchContextPriorityGridViewVisibleThumbnail = 6;
const TInt KGlxFetchContextPriorityGridViewDetails = 0;
const TInt KGlxFetchContextPriorityGridViewDetailsFirstLast = 0;
const TInt KGlxFetchContextPriorityGridViewNonVisibleThumbnail = 0;
const TInt KGlxFetchContextPriorityGridViewFullscreenPrefetchNonVisibleThumbnail = 0;
const TInt KGlxFetchContextPriorityGridViewFullscreenVisibleThumbnail = 5;

const TInt KGlxFetchContextPriorityCommandHandlerOpening = 7;
const TInt KGlxFetchContextPriorityCommandHandlerPrefetch = 0;

const TInt KGlxFetchContextPriorityListViewTitles = 9;
const TInt KGlxFetchContextPriorityListViewFocusedSubtitle = 8;
const TInt KGlxFetchContextPriorityListViewSubtitles = 0;


// tile fetcher
const TInt KGlxFetchContextPriorityTitleFetcher = 10;

// video playback view UID
const TInt KGlxVideoPlaybackviewPlugin = 0x2000A7B3;


#endif  // GLXUISTD_H
