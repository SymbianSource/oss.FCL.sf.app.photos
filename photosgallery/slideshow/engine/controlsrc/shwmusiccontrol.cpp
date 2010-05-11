/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The music control for the slideshow
 *
*/




//  CLASS HEADER
#include "shwmusiccontrol.h"

//  EXTERNAL INCLUDES
#include <f32file.h>    // RFs
#include <mpxplaybackutility.h>     // MMPXPlaybackUtility
#include <mpxplaybackframeworkdefs.h> // TMPXPlaybackProperty, EPbRepeatOne
#include <mpxplaybackmessage.h> // TMPXPlaybackMessage
#include <mpxmessagegeneraldefs.h>

//  INTERNAL INCLUDES
#include "shwevent.h"
#include "shwslideshowenginepanic.h"

#include <glxlog.h>
#include <glxtracer.h>

// -----------------------------------------------------------------------------
// C++ Constructor. save a few bits of ROM by inlining it.
// -----------------------------------------------------------------------------
inline CShwMusicControl::CShwMusicControl(
            MShwMusicObserver& aMusicObsvr, 
            const TDesC& aFilePath )
        : iFilePath( aFilePath ), 
        iMusicObsvr( aMusicObsvr ),
        iMaxVolume( KErrNotFound ),
        iCurrentVolume( KErrNotFound ),
        iState( EMusicOff )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwMusicControl* CShwMusicControl::NewL(
    MShwMusicObserver& aMusicObsvr, const TDesC& aFilePath )
    {
    TRACER(" CShwMusicControl::NewL");
    GLX_LOG_INFO( "CShwMusicControl::NewL" );
    CShwMusicControl* self = 
        new( ELeave ) CShwMusicControl( aMusicObsvr, aFilePath );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwMusicControl::~CShwMusicControl()
    {
    TRACER(" CShwMusicControl::~CShwMusicControl");
    GLX_LOG_INFO( "CShwMusicControl::~CShwMusicControl" );
    if( iPlaybackUtility )
        {
        //The Code Scanner Error is not corrected here in the case of
        //calling a leaving function in the Non Leaving Function
        //We need to write a seperate Function for this.
        //but will reduce not reduce any error.
        iPlaybackUtility->RemoveObserverL(*this);
        // Tell player to close the music playback
        TRAP_IGNORE( iPlaybackUtility->CommandL( EPbCmdClose ) );



        // Don't destroy, close releases resources.
        iPlaybackUtility->Close();
        }
    }

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
void CShwMusicControl::ConstructL()
    {
    TRACER("CShwMusicControl::ConstructL");
    GLX_LOG_INFO("CShwMusicControl::ConstructL");
    // need to specify the mode and observer, without these we get a crash
    iPlaybackUtility = MMPXPlaybackUtility::NewL( KPbModeNewPlayer, this );
    // music playback is sacrificed if MPX fails
    TRAPD( err, InitPlayerL() );
    if( err == KErrNone )
        {
        iState = EMusicOn;
        }
    }

// ---------------------------------------------------------------------------
// Initialize the player with File Path
// ---------------------------------------------------------------------------
void CShwMusicControl::InitPlayerL()
    {
    TRACER("CShwMusicControl::InitPlayerL");
    GLX_LOG_INFO( "CShwMusicControl::InitPlayerL" );
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.ShareProtected() );
    RFile file;
    // try to open the file
    TInt error = file.Open( 
                    fs, 
                    iFilePath, 
                    EFileRead | EFileShareReadersOrWriters );
    // was there an error
    if( KErrNone != error )
        {
        GLX_LOG_INFO1( 
            "CShwMusicControl error in track %d", error );
        // let the observer know there was an error
        iMusicObsvr.ErrorWithTrackL( error );
        User::Leave( error );
        }

    CleanupClosePushL( file );
    iPlaybackUtility->InitL( file );
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fs );

    // The track should loop
    iPlaybackUtility->SetL( EPbPropertyRepeatMode, EPbRepeatOne );
    }

// -----------------------------------------------------------------------------
// VolumeL
// Retrieve the volume values
// -----------------------------------------------------------------------------
void CShwMusicControl::VolumeL()
    {
    TRACER("CShwMusicControl::VolumeL");
    GLX_LOG_INFO( "CShwMusicControl::VolumeL" );
    // Retrieve the volume - Volume indicator is shown upon slideshow start and
    // when the volume is changed
    if (iMaxVolume == KErrNotFound)
        {
        iPlaybackUtility->ValueL( *this, EPbPropertyMaxVolume );	
        }
    iPlaybackUtility->ValueL( *this, EPbPropertyVolume );
    }

// -----------------------------------------------------------------------------
// NotifyL.
// -----------------------------------------------------------------------------
void CShwMusicControl::NotifyL( MShwEvent* aEvent )
    {
    TRACER("CShwMusicControl::NotifyL");
    GLX_LOG_INFO( "CShwMusicControl::NotifyL" );
    // only handle events if music initialisation succeeded
    if( iState == EMusicOn )
        {
        // reset state flag
        iSwitchingMusicOn = EFalse;
        // music playback is sacrificed if MPX fails
        TRAPD( errr, HandleEventL( aEvent ) );
        // if we had an error while switching music back on, tell observer 
        // that music is off
        if( ( errr != KErrNone )&&
            ( iSwitchingMusicOn ) )
            {
            iMusicObsvr.MusicOff();
            }
        }
    }

// ---------------------------------------------------------------------------
// From class MMPXPlaybackCallback.
// Handle the change of status of playback engine
// ---------------------------------------------------------------------------
void CShwMusicControl::HandlePropertyL( TMPXPlaybackProperty aProperty,
                                        TInt aValue,
                                        TInt aError )
    {
    TRACER("CShwMusicControl::HandlePropertyL");
    GLX_LOG_INFO( "CShwMusicControl::HandlePropertyL" );

    // leave if there was an error
    User::LeaveIfError( aError );

    // handle max volume and volume, ignore other properties
    if( EPbPropertyMaxVolume == aProperty )
        {
        iMaxVolume = aValue;
        }
    else if( EPbPropertyVolume == aProperty )
        {
        // set the current volume
        iCurrentVolume = aValue;
        // call observer only when max volume is also known
        // iMaxVolume and iCurrentVolume are initialized to KErrNotFound
        if( iMaxVolume != KErrNotFound )
        	{
        	iMusicObsvr.MusicVolumeL( iCurrentVolume, iMaxVolume );
        	}
        }
    }

// -----------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that 
// it is done and there will be no more callbacks
// Only new items are passed each time
// -----------------------------------------------------------------------------
void CShwMusicControl::HandleSubPlayerNamesL(
    TUid /*aPlayer*/, const MDesCArray* /*aSubPlayers*/,
    TBool /*aComplete*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// From class MMPXPlaybackCallback.
// Call back of media request
// ---------------------------------------------------------------------------
void CShwMusicControl::HandleMediaL(
    const CMPXMedia& /*aProperties*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// GetMusicVolumeL
// ---------------------------------------------------------------------------
void CShwMusicControl::GetMusicVolumeL()
    {
    TRACER("CShwMusicControl::GetMusicVolumeL");
    if ( iCanPlay )
        {
        VolumeL();
        }
    }
	
// ---------------------------------------------------------------------------
// From class MMPXPlaybackObserver 
// Handle playback message - required for debugging state of MPX 
// Player component
// ---------------------------------------------------------------------------
void CShwMusicControl::HandlePlaybackMessageL(
    const TMPXPlaybackMessage& /*aMessage*/ )
    {
    }
	
// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// ---------------------------------------------------------------------------
void CShwMusicControl::HandlePlaybackMessage(const CMPXMessage& aMsg)
    {
    TRACER("CShwMusicControl::HandlePlaybackMessage");
    GLX_LOG_ENTRY_EXIT( "CShwMusicControl::HandlePlaybackMessageL()" );
    
    switch(*aMsg.Value<TMPXPlaybackMessage::TEvent>( KMPXMessageGeneralEvent ))
        {
        case TMPXPlaybackMessage::EInitializeComplete:
                {
                GLX_LOG_INFO( "TMPXPlaybackMessage::EInitializeComplete" );
                // at here, can play the track
                iCanPlay = ETrue;

                //execute the cached "play" command 
                if ( iPlayCached )
                    {
                    GLX_LOG_INFO( "iPlayCached" );
                    // Play the track
                    //The Code Scanner Error is not corrected here in the case of
                    //calling a leaving function in the Non Leaving Function
                    //We need to write a seperate Function for this.
                    //but will reduce only one error.
                    TRAP_IGNORE(iPlaybackUtility->CommandL( EPbCmdPlay ) );                    
		            iMusicObsvr.MusicOnL();
		            // need to call volume ourself to show the 
		            // volume indicator in start
		            VolumeL();                    
                    iPlayCached = EFalse;
                    }
                break;
                }
        default:            
           {
           GLX_LOG_INFO( "CShwMusicControl::HandlePlaybackMessage: Default" );
            break;
            }
        }  
    }

// -----------------------------------------------------------------------------
// HandleEventL.
// -----------------------------------------------------------------------------
void CShwMusicControl::HandleEventL( MShwEvent* aEvent )
    {
    TRACER("CShwMusicControl::HandleEventL( MShwEvent* aEvent )");
    GLX_LOG_INFO( "CShwTimerControl::HandleEventL" );
    // we got an event, was it start
    if( dynamic_cast< TShwEventStart* >( aEvent ) )
        {
        GLX_LOG_INFO( "CShwMusicControl::NotifyL - TShwEventStart" );
        // set state flag
        iSwitchingMusicOn = ETrue;
        // iCanPlay is True when Player is Initialized.
        if ( iCanPlay )
            {
            GLX_LOG_INFO( "inside ICanPlay" );
            // Play the track
            TRAP_IGNORE(iPlaybackUtility->CommandL( EPbCmdPlay ) );
            iMusicObsvr.MusicOnL();
            // need to call volume ourself to show the 
            // volume indicator in start
            VolumeL();
            }
        else
            {
            GLX_LOG_INFO( "inside Else: ICanPlay" );
			// initialization is not finished ,and cache the "play" command 
            iPlayCached = ETrue;
            }     
        }
    else if( dynamic_cast< TShwEventPause* >( aEvent ) )
        {
        GLX_LOG_INFO( "CShwMusicControl::NotifyL - TShwEventPause" );
        // pause playback
        iPlaybackUtility->CommandL( EPbCmdPause );
        // let the observer know
        iMusicObsvr.MusicOff();
        }
    else if( dynamic_cast< TShwEventResume* >( aEvent ) )
        {
        GLX_LOG_INFO( "CShwMusicControl::NotifyL - TShwEventResume" );
        // set state flag
        iSwitchingMusicOn = ETrue;
        // resume
        iPlaybackUtility->CommandL( EPbCmdPlay );
        // let the observer know
        iMusicObsvr.MusicOnL();
        }
    else if( dynamic_cast< TShwEventVolumeDown* >( aEvent ) )
        {
        GLX_LOG_INFO( "CShwMusicControl::NotifyL - TShwEventVolumeDown" );

        iPlaybackUtility->CommandL( EPbCmdDecreaseVolume );
        VolumeL();
        }
    else if( dynamic_cast< TShwEventVolumeUp* >( aEvent ) )
        {
        GLX_LOG_INFO( "CShwMusicControl::NotifyL - TShwEventVolumeUp" );

        iPlaybackUtility->CommandL( EPbCmdIncreaseVolume );
        VolumeL();
        }
    else if( dynamic_cast< TShwEventToggleControlUi* >( aEvent ) )
        {
        GLX_LOG_INFO( "CShwMusicControl::NotifyL - TShwEventToggleControlUi" );
        // Have to impliment if need comes
        }
    }

