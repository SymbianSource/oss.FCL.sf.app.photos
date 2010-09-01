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
* Description:    Implementation of Single Line metapane for Fulscreen view
*
*/


// Avkon Headers
#include <stringloader.h>                           // String loader
#include <data_caging_path_literals.hrh>            // KDC_APP_RESOURCE_DIR
#include <pathinfo.h>                               // For Path info
#include <driveinfo.h>                              // Added for DriveInfo class
#include <avkon.rsg>                                // Added for R_QTN_DATE_USUAL_WITH_ZERO
#include <akntoolbar.h>

// Alf Headers
#include <alf/alfenv.h>
#include <alf/alfgridlayout.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfroster.h>
#include <alf/alftexture.h>
#include <alf/alfimage.h>
#include <alf/alfimagevisual.h>
#include <alf/alftextvisual.h>
#include <alf/alflinevisual.h>
#include <alf/alfutil.h>
#include <alf/alfevent.h>
#include <alf/alfbrusharray.h>
#include <alf/alfborderbrush.h>

// Photos Headers
#include <glxtracer.h>                              // Tracer logs
#include <glxlog.h>                                 // Glx logs
#include <mglxmedialist.h>                          // CGlxMedialist
#include <glxustringconverter.h>                    // Converts the symbian types to UString type
#include <glxattributecontext.h>                    // Attribute context
#include <glxuistd.h>                               // Attribute fetch priority
#include <glxuiutility.h>                           // UiUtility Singleton
#include <glxfullscreenviewdata.rsg>                // FS resource
#include <glxicons.mbg>                             // icons
#include <glxtexturemanager.h>                      // Texturemanager
#include <glxcommandhandlers.hrh>                   // Commandhandlers
#include <glxgeneraluiutilities.h>                  // General utilties class definition
#include <glxslmpfavmlobserver.h>

// User Includes
#include <glxsinglelinemetapanecontrol.h>           // User include

// Contants and Literals
const TInt KGlxFullScreenControlGrpId =0x113;       // This is the same control grp id used in FS view
const TInt KGridColoumns = 4;
const TInt KGridRows = 1;
const TReal KOpacityOpaque = 1.0;
const TReal KOpacityTransparent = 0.0;

_LIT(KEmptyText, "");
_LIT(KFormat, " ");
_LIT8(KTagLoc, "Loc");
_LIT8(KTagFav, "Fav");

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//  
CGlxSingleLineMetaPane* CGlxSingleLineMetaPane::NewL(CGlxFullScreenViewImp& aFullscreenView,
        MGlxMediaList& aMediaList, CGlxUiUtility& aUiUtility)
    {
    TRACER("CGlxSingleLineMetaPane* CGlxSingleLineMetaPane::NewL()");
    CGlxSingleLineMetaPane* self = new (ELeave) CGlxSingleLineMetaPane(aFullscreenView,
            aMediaList, aUiUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CGlxSingleLineMetaPane() constructor
// ---------------------------------------------------------------------------
//  
CGlxSingleLineMetaPane::CGlxSingleLineMetaPane(CGlxFullScreenViewImp& aFullscreenView, 
        MGlxMediaList& aMediaList, CGlxUiUtility& aUiUtility) :iFullscreenView(aFullscreenView),
        iMediaList(aMediaList), iUiUtility(aUiUtility)
    {
    TRACER("CGlxSingleLineMetaPane* CGlxSingleLineMetaPane::NewL()");
    // No Implementation
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::ConstructL()
    {
    TRACER("CGlxSingleLineMetaPane::ConstructL()");
    // Construct the base class.
    // Get the icon file
    TFileName resFile(KDC_APP_BITMAP_DIR);
    resFile.Append(KGlxIconsFilename);

    iEnv = iUiUtility.Env();
    CAlfControl::ConstructL(*iEnv);
    
    CAlfTexture& backgroundTexture = iUiUtility.GlxTextureManager().CreateIconTextureL(
            EMbmGlxiconsQgn_lgal_bg, resFile ) ;
    //----------------------- LAYOUT HIERARCHY -------------------------------
    //
    //                             ->iDateTextVisual 
    //                            |
    //                            |                   
    //                            |->iFavIconImageVisual
    //                            |
    // iMainVisual-->iGridVisual->
    //                            |->iMapIconImageVisual                   
    //                            |                    
    //                            |
    //                             ->iLocationTextVisual 
    //-------------------------------------------------------------------------

    iMainVisual = CAlfAnchorLayout::AddNewL(*this);
 
    // BackGround Border Image Visual
    iBackgroundBorderImageVisual = CAlfImageVisual::AddNewL(*this,iMainVisual);
    iBackgroundBorderImageVisual->EnableBrushesL();
    iBackgroundBorderImageVisual->SetImage(TAlfImage(backgroundTexture));
    
    iBackgroundBorderImageVisual->SetOpacity( 0.6 );
    
    iGridVisual = CAlfGridLayout::AddNewL(*this, KGridColoumns, KGridRows, iMainVisual);
    CAlfControlGroup* group =&iEnv->ControlGroup(KGlxFullScreenControlGrpId);
    group->AppendL(this);

    // Create a 1 pixel Border brush and append it to the background visual
    CAlfBorderBrush* borderBrush= CAlfBorderBrush::NewLC(*iEnv, 1, 1, 0, 0);
    borderBrush->SetColor(KRgbWhite);
    iBackgroundBorderImageVisual->Brushes()->AppendL(borderBrush, EAlfHasOwnership);
    CleanupStack::Pop(borderBrush);
    
    iMediaList.AddMediaListObserverL( this );
    // Set up and add the context
    iAttribContext = CGlxDefaultAttributeContext::NewL();
    // add all the attributes needed in handleattributes available
    // this is because even
    iAttribContext->AddAttributeL( KMPXMediaGeneralDate );
    iAttribContext->AddAttributeL( KGlxMediaGeneralLocation );
    
    // Using priority as same as UMP view's album pane
    iMediaList.AddContextL( iAttribContext, KGlxFetchContextPriorityLow );

    // Hide the Metapane at the initial start up, to avoid flicker
    ShowMetaPane(EFalse);
    
    CreateGridL();

    iMainVisual->SetFlag(EAlfVisualFlagManualLayout);

    // Update the pos and size of the visual 
    UpdatePosition();

    // Create the Metapane
    iSLMPFavMLObserver = CGlxSLMPFavMLObserver::NewL(*this, iMediaList.FocusIndex(), &iMediaList);
    
    // Update the Grid Visuals with initial Data
    UpdateMetaPaneL(iMediaList.FocusIndex(), &iMediaList, ETrue);
    // Setting the Flag to get layout change notifications
    iMainVisual->SetFlag ( EAlfVisualFlagLayoutUpdateNotification); 
    
    // Store the Current Screensize
    iScreenSize = GetScreenSize();
    }

// ---------------------------------------------------------------------------
// ~CGlxSingleLineMetaPane Destructor
// ---------------------------------------------------------------------------
//  
CGlxSingleLineMetaPane::~CGlxSingleLineMetaPane()
    {
    TRACER("CGlxSingleLineMetaPane::~CGlxSingleLineMetaPane()");
    iMediaList.RemoveMediaListObserver(this);
    if( iAttribContext )
        {
        iMediaList.RemoveContext( iAttribContext );
        delete iAttribContext;
        }
    if (iMainVisual)
        {
        iMainVisual->RemoveAndDestroyAllD();
        iMainVisual = NULL;
        }
    if (iSLMPFavMLObserver)
        {
        delete iSLMPFavMLObserver;
        iSLMPFavMLObserver= NULL;
        }
    }

// ---------------------------------------------------------------------------
// CreateIconTextureAndUpdateVisualsL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::CreateIconTextureAndUpdateVisualsL()
    {
    TRACER("CGlxSingleLineMetaPane::CreateIconTextureAndUpdateVisualsL()");
    // Get the icon file
    TFileName resFile(KDC_APP_BITMAP_DIR);
    resFile.Append(KGlxIconsFilename);
    CGlxTextureManager& tm = (iUiUtility.GlxTextureManager());
    //Create the texture for broken thumbnail
    iFavNotAddedIconTexture = &(tm.CreateIconTextureL(
            EMbmGlxiconsQgn_lgal_fav2, resFile ) ) ;
    iFavAddedIconTexture = &(tm.CreateIconTextureL(
            EMbmGlxiconsQgn_lgal_fav1, resFile ) );
    CAlfTexture& locIconTexture = tm.CreateIconTextureL(
            EMbmGlxiconsQgn_menu_maps, resFile ) ;
    iLocIconImageVisual->SetImage(TAlfImage(locIconTexture));
    }

// ---------------------------------------------------------------------------
// CreateGridL
// @bug :tapadar :major :This function is using magic numbers, would be corrected with updated LAF docs
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::CreateGridL()
    {
    TRACER("CGlxSingleLineMetaPane::CreateGridL()");

    iGridVisual->SetPadding(TPoint(1,1));
    iGridVisual->SetInnerPadding( TPoint(1,1) );
    RArray<TInt> weights;
    CleanupClosePushL(weights);

    User::LeaveIfError(weights.Append(2));
    User::LeaveIfError(weights.Append(1));
    User::LeaveIfError(weights.Append(1));
    User::LeaveIfError(weights.Append(5));

    iGridVisual->SetColumnsL(weights);
    CleanupStack::PopAndDestroy(&weights);

    iDateTextVisual = CAlfTextVisual::AddNewL(*this,iGridVisual);
    iDateTextVisual->SetStyle( EAlfTextStyleSmall, EAlfBackgroundTypeDark );
    iDateTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
    iDateTextVisual->SetTextL( KEmptyText );
    iDateTextVisual->SetAlign(EAlfAlignHLocale, EAlfAlignVCenter);

    iFavIconImageVisual= CAlfImageVisual::AddNewL(*this,iGridVisual);
    iFavIconImageVisual->SetScaleMode(CAlfImageVisual::EScaleFitInside);
    iFavIconImageVisual->SetTagL(KTagFav);
    CAlfBorderBrush* borderBrush= CAlfBorderBrush::NewLC(*iEnv, 1, 1, 0, 0);
    borderBrush->SetColor(KRgbWhite);
    borderBrush->SetOpacity(0.35f);
    iFavIconImageVisual->EnableBrushesL();
    iFavIconImageVisual->Brushes()->AppendL(borderBrush, EAlfHasOwnership);
    CleanupStack::Pop(borderBrush);

    iLocIconImageVisual = CAlfImageVisual::AddNewL(*this,iGridVisual);
    iLocIconImageVisual->SetScaleMode(CAlfImageVisual::EScaleFitInside);

    // Creates the Icon Textures and Updates the Favourite and Loc Icon Visuals
    CreateIconTextureAndUpdateVisualsL();

    iLocationTextVisual = CAlfTextVisual::AddNewL(*this,iGridVisual);
    iLocationTextVisual->SetStyle( EAlfTextStyleSmall, EAlfBackgroundTypeDark );
    iLocationTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
    HBufC* noLocationInfo = 
        StringLoader::LoadLC(R_FULLSCREEN_NO_LOCATION_INFO);
    iLocationTextVisual->SetTextL(*noLocationInfo);
    CleanupStack::PopAndDestroy(noLocationInfo );
    iLocationTextVisual->SetTagL(KTagLoc);
    iLocationTextVisual->SetAlign(EAlfAlignHLocale, EAlfAlignVCenter);
    }

// ---------------------------------------------------------------------------
// ShowMetaPane
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::ShowMetaPane(TBool aShow)
    {
    TRACER("CGlxSingleLineMetaPane::ShowMetaPane()");
    GLX_LOG_INFO1("CGlxSingleLineMetaPane::ShowMetaPane() value %d",aShow);
    if(aShow)
        {
        iMainVisual->SetOpacity(KOpacityOpaque);    
        }
    else
        {
        iMainVisual->SetOpacity(KOpacityTransparent);
        }
    }

// ---------------------------------------------------------------------------
// OfferEventL
// ---------------------------------------------------------------------------
//  
TBool CGlxSingleLineMetaPane::OfferEventL(const TAlfEvent &aEvent)
    {
    TRACER("CGlxSingleLineMetaPane::OfferEventL()");
    TBool consumed = EFalse;
    
    if (aEvent.IsKeyEvent())
        {
        GLX_LOG_INFO( "CGlxSingleLineMetaPane::OfferEventL::KeyEvent");
        }

    // Dont handle if not visible. [HACK ALERT]: This might probably be a hack. 
	// Further investigation needed. 
    if (KOpacityTransparent == iMainVisual->Opacity().ValueNow())
        {
        return consumed;
        }
    
    if(aEvent.IsPointerEvent() && aEvent.PointerDown() )
        {
        GLX_LOG_INFO( "CGlxSingleLineMetaPane::OfferEventL::PointerEvent");
        CAlfVisual* onVisual = NULL;
        onVisual = aEvent.Visual();
        if(AlfUtil::TagMatches(onVisual->Tag(), KTagFav))
            {
            GLX_LOG_INFO( "CGlxSingleLineMetaPane::OfferEventL::PointerEvent - KTagFav");
            if (iFavIconStatus)
                {
                GLX_LOG_INFO( "CGlxSingleLineMetaPane::OfferEventL::PointerEvent - Already Fav");
                iFullscreenView.HandleCommandL(EGlxCmdRemoveFromFavourites,this);
                }
            else
                {
                GLX_LOG_INFO( "CGlxSingleLineMetaPane::OfferEventL::PointerEvent - Add to Fav");
                iFullscreenView.HandleCommandL(EGlxCmdAddToFavourites,this);    
                }
            consumed = ETrue;
            }
        else if (AlfUtil::TagMatches(onVisual->Tag(), KTagLoc))
            {
            GLX_LOG_INFO( "CGlxSingleLineMetaPane::OfferEventL::PointerEvent - KTagLoc , Location info present");
            iFullscreenView.HandleCommandL(KGlxCmdMnShowMap,this); 
            consumed = ETrue;
            }
        }
    return consumed;
    }

// ---------------------------------------------------------------------------
// VisualLayoutUpdated
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::VisualLayoutUpdated(CAlfVisual&/* aVisual*/)
    {
    TRACER("CGlxSingleLineMetaPane::VisualLayoutUpdated()");
    // Update the Screen positions only if the layout has changed.
    // For performace improvement
    TRect presentScreenSize = GetScreenSize();
    if (iScreenSize.Width() != presentScreenSize.Width())
        {
        UpdatePosition();
        iScreenSize = presentScreenSize;
        }
    }

// ---------------------------------------------------------------------------
// DetermineOrientation
// ---------------------------------------------------------------------------
//  
TInt CGlxSingleLineMetaPane::DetermineOrientation(TRect aRect)
    {
    TRACER("CGlxSingleLineMetaPane::DetermineOrientation()");
    TInt orientationValue = (aRect.Width()>aRect.Height()?EMetaOrinentaionLandscape:
                                                            EMetaOrinentaionPortrait);
    return orientationValue;
    }

// ---------------------------------------------------------------------------
// GetScreenSize
// ---------------------------------------------------------------------------
//  
TRect CGlxSingleLineMetaPane::GetScreenSize()
    {
    TRACER("CGlxSingleLineMetaPane::GetScreenSize()");
    return AlfUtil::ScreenSize();
    }
// ---------------------------------------------------------------------------
// UpdatePosition
// @bug :tapadar :major :This function is using magic numbers, would be corrected with updated LAF docs
// @bug :tapadar :major :This function needs to be worked upon across corrolla and ivalo once LAF present
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::UpdatePosition()
    {
    TRACER("CGlxSingleLineMetaPane::UpdatePosition()");
    TRect rect = GetScreenSize();
    TInt screenWidth = rect.Width();
    TInt screenHeight = rect.Height();
    TInt orientation = DetermineOrientation(rect);
    // Switch off the Flag for layout update notification, else a change in size/pos 
    // will call VisualLayoutUpdated multiple times.
    iMainVisual->ClearFlag( EAlfVisualFlagLayoutUpdateNotification);
    if (EMetaOrinentaionPortrait == orientation)
        {
        GLX_LOG_INFO("CGlxSingleLineMetaPane::UpdatePosition - EMetaOrinentaionPortrait Orientation");
        CAknToolbar* toolbar = iAvkonAppUi->CurrentFixedToolbar();
        
        if (toolbar->IsToolbarDisabled())
            {
            //set the control size
            iMainVisual->SetSize(TSize(screenWidth-6,45));
            iMainVisual->SetPos(TAlfRealPoint(3,screenHeight-100));
            }
        else
            {
            //set the control size
            iMainVisual->SetSize(TSize(screenWidth-6,45));
            iMainVisual->SetPos(TAlfRealPoint(3,screenHeight-160));
            }
        }
    else if(EMetaOrinentaionLandscape == orientation)
        {
        GLX_LOG_INFO("CGlxSingleLineMetaPane::UpdatePosition - EMetaOrinentaionLandscape Orientation");
        //set the control size
        iMainVisual->SetSize(TSize(screenWidth-100,45));
        iMainVisual->SetPos(TAlfRealPoint(3,screenHeight-45));
        }
    else
        {
        GLX_LOG_INFO("CGlxSingleLineMetaPane::UpdatePosition - Wrong Orientation");
        }
    iMainVisual->SetFlag ( EAlfVisualFlagLayoutUpdateNotification);
    }

// ---------------------------------------------------------------------------
// UpdateMetaPaneL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::UpdateMetaPaneL(TInt aFocusIndex, MGlxMediaList* aList, 
            TBool aUpdateFavIcon)
    {
    TRACER("CGlxSingleLineMetaPane::UpdateMetaPaneL()");
    
    const TGlxMedia& item = aList->Item(aFocusIndex);
    const CGlxMedia* media = item.Properties();
    CGlxUStringConverter* stringConverter = CGlxUStringConverter::NewL();
    CleanupStack::PushL(stringConverter );
    HBufC* dataString = NULL;

    if (media->IsSupported(KMPXMediaGeneralDate))
        {
        // R_QTN_DATE_USUAL is for the Date of format 08/02/1982 type
        stringConverter->AsStringL( item, KMPXMediaGeneralDate,R_QTN_DATE_USUAL, 
                dataString );
        if (dataString)
            {
            iDateTextVisual->SetTextL(*dataString);            
            }
        }
    if (media->IsSupported(KGlxMediaGeneralLocation))
        {
        stringConverter->AsStringL( item, KGlxMediaGeneralLocation,0, dataString );
        if (dataString->Compare(KFormat) != 0)
            {
            iLocationTextVisual->SetTextL(*dataString);
            }
        }
    else
        {
        HBufC* noLocationInfo = 
                  StringLoader::LoadLC(R_FULLSCREEN_NO_LOCATION_INFO);
        iLocationTextVisual->SetTextL(*noLocationInfo);
        CleanupStack::PopAndDestroy(noLocationInfo );
        }
    delete dataString;
    dataString = NULL;
    CleanupStack::PopAndDestroy(stringConverter );
    
    // Setting the not created icon to the FavIcon initially, would update it once available 
    // through HandleObjectChangeL
    if (aUpdateFavIcon)
        {
        iFavIconStatus = EFalse;
        iFavIconImageVisual->SetImage(TAlfImage(*iFavNotAddedIconTexture));
        }
    }

// ---------------------------------------------------------------------------
// HandleUpdateIconL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleUpdateIconL(TBool aModify)
    {
    TRACER("CGlxSingleLineMetaPane::HandleUpdateIconL()");
    // Check if the item is added to favourites already
    if (aModify)
        {
        iFavIconStatus = ETrue;
        iFavIconImageVisual->SetImage(TAlfImage(*iFavAddedIconTexture));
        }
    else
        {
        iFavIconStatus = EFalse;
        iFavIconImageVisual->SetImage(TAlfImage(*iFavNotAddedIconTexture));
        }
    }

// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleItemAddedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/,
        MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxSingleLineMetaPane::HandleItemAddedL()");
    // No Implementation
    }

// ---------------------------------------------------------------------------
// HandleItemRemovedL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleItemRemovedL( TInt/* aStartIndex*/, 
        TInt/* aEndIndex*/, MGlxMediaList*/* aList */)
    {
    TRACER("CGlxSingleLineMetaPane::HandleItemRemovedL()");
    // No Implementation
    }

// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleAttributesAvailableL( TInt aItemIndex, 
    const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* aList )
    {
    TRACER("CGlxSingleLineMetaPane::HandleAttributesAvailableL()");

    TInt focusIndex = iMediaList.FocusIndex();
    GLX_LOG_INFO2("CGlxSingleLineMetaPane::HandleAttributesAvailableL FocusIndex is : %d and ItemIdex is : %d", 
            focusIndex, aItemIndex);
    
    // Update the metapane only for the focus Index, do not make unnecessary calls.
    // for focus change, we would get calls to HandleFocusChange and thus would update
    // metapane from there.
    if (aItemIndex == focusIndex)
        {
        UpdateMetaPaneL(focusIndex, aList);    
        }
    }

// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleFocusChangedL( NGlxListDefs::TFocusChangeType /*aType*/, 
TInt aNewIndex, TInt /*aOldIndex*/, MGlxMediaList* aList )
    {
    TRACER("CGlxSingleLineMetaPane::HandleFocusChangedL()");
    // Check the count here to ensure the medialist is present
    // this case may be occurred when the last item of the Fs view is deleted and it 
    // wants to go back to the grid view
    if (aList->Count())
        {
        if (iSLMPFavMLObserver)
            {    
            delete iSLMPFavMLObserver;
            iSLMPFavMLObserver= NULL;
            iSLMPFavMLObserver = CGlxSLMPFavMLObserver::NewL(*this, aList->FocusIndex(), aList);
            }
        // Update the Metapane on Focus change
        UpdateMetaPaneL(aNewIndex, aList, ETrue);
        }
    }

// ---------------------------------------------------------------------------
// HandleItemSelectedL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleItemSelectedL(TInt/* aIndex*/, TBool/* aSelected*/,
        MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxSingleLineMetaPane::HandleItemSelectedL()");
    // No Implementation
    }

// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleMessageL( const CMPXMessage& /*aMessage*/, 
        MGlxMediaList*/* aList */)
    {
    TRACER("CGlxSingleLineMetaPane::HandleMessageL()");
    // No Implementation
    }

// ---------------------------------------------------------------------------
// HandleError
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleError( TInt /*aError*/ )
    {
    TRACER("CGlxSingleLineMetaPane::HandleError()");
    // No Implementation
    }

// ---------------------------------------------------------------------------
// HandleCommandCompleteL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, 
        TInt /*aError*/, MGlxMediaList*/* aList */)
    {
    TRACER("CGlxSingleLineMetaPane::HandleCommandCompleteL()");
    // No Implementation
    }

// ---------------------------------------------------------------------------
// HandleMediaL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleMediaL( TInt /*aListIndex*/, MGlxMediaList*/* aList */)
    {
    TRACER("CGlxSingleLineMetaPane::HandleMediaL()");
    // No Implementation
    }

// ---------------------------------------------------------------------------
// HandleItemModifiedL
// ---------------------------------------------------------------------------
//  
void CGlxSingleLineMetaPane::HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/,
        MGlxMediaList*/* aList */)
    {
    TRACER("CGlxSingleLineMetaPane::HandleItemModifiedL()");
    // No Implementation
    }

