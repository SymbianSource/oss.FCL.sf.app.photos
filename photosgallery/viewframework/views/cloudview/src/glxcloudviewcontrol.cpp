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
* Description:    Cloud View control class 
 *
*/


// INCLUDE FILES
#include "glxcloudviewcontrol.h" //class header
//AlfT headers

#include <alf/alfanchorlayout.h> // For CAlfAnchorLayout 
#include <alf/alfviewportlayout.h> // For CAlfViewPortLayout
#include <touchfeedback.h>  // For MTouchFeedback
#include <alf/alftextstyle.h>  // For CAlfTextStyle
#include <alf/alftextvisual.h> // For CAlfTextVisual
#include <alf/alftransformation.h>
#include <alf/alfwidgetcontrol.h> // For CAlfWidgetControl
#include <alf/alfroster.h> // For CalfRoster
#include <alf/alfdisplay.h> // For CAlfDisplay
#include <alf/ialfscrollbarmodel.h> // For alfScrollbar model
#include <aknlayoutscalable_uiaccel.cdl.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfutil.h>
#include <aknphysics.h> // For Kinetic Scrolling
#include <glxuiutility.h>
#include <glxgeneraluiutilities.h>
#include <glxuistd.h>

//AlfT
#include <glxlog.h>			// For Logs
#include <glxtracer.h>			// For Logs
#include <glxattributecontext.h> //attribute context
#include <mglxmedialist.h> //for medialist

#include "glxcontainerinfobubble.h" //intelligent class for data of infobubble
#include "glxcloudview.hrh"
#include "mglxcloudviewmskobserver.h" // For Msk Observer
#include "mglxenterkeyeventobserver.h" // For enterkey observer
#include "mglxcloudviewlayoutobserver.h"
#include "glxtagscontextmenucontrol.h"

//Constants
const TInt KPrimaryFontSize = 21;
//Font sizes
const TInt KFontSmallest = 28;
const TInt KFontSmaller = 32;
const TInt KFontMedium = 36;
const TInt KFontLarger = 40;
const TInt KFontLargest = 44;
const TInt KRowHeight = 72;
const TInt KLeftMargin = 10;
const TInt KNumMinRowSpace = 2;
const TInt KColSpace = 20;
const TInt KRightmargin = 20;
const TInt KMinTagSize = 77;
const TInt KTagScreenHeight = 460;
const TReal KBoundaryMargin = 0.1; //10% = 10/100 = 0.1 
const TInt KFastCloudMovement = 100; //Transition time to move cloud view
const TInt KSlowCloudMovement = 1000; //Transition time to move cloud view
const TInt KLongPressTimer = 1500000; //1.5 seconds

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
CGlxCloudViewControl *CGlxCloudViewControl::NewL(CAlfDisplay& aDisplay,
        CAlfEnv &aEnv, MGlxMediaList& aMediaList, const TDesC& aEmptyText,
        MGlxCloudViewMskObserver& aObserver,MGlxEnterKeyEventObserver& aObserverEnterKeyEvent
        ,CAlfAnchorLayout *aAnchorLayout,
        MGlxCloudViewLayoutObserver& aLayoutObserver
        ,MGlxItemMenuObserver& aItemMenuObserver)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::NewL");
    CGlxCloudViewControl *self = CGlxCloudViewControl::NewLC (aDisplay, aEnv,
            aMediaList, aEmptyText,aObserver,aObserverEnterKeyEvent,aAnchorLayout,aLayoutObserver,aItemMenuObserver);
    CleanupStack::Pop (self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
CGlxCloudViewControl *CGlxCloudViewControl::NewLC(CAlfDisplay& aDisplay,
        CAlfEnv &aEnv, MGlxMediaList& aMediaList, const TDesC& aEmptyText,
        MGlxCloudViewMskObserver& aObserver,MGlxEnterKeyEventObserver& aObserverEnterKeyEvent
        ,CAlfAnchorLayout *aAnchorLayout,MGlxCloudViewLayoutObserver& aLayoutObserver
        ,MGlxItemMenuObserver& aItemMenuObserver)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::NewLC");
    CGlxCloudViewControl *self = new(ELeave)CGlxCloudViewControl(aEnv,aMediaList,aObserver
            ,aObserverEnterKeyEvent,aLayoutObserver);
    CleanupStack::PushL (self);
    self->ConstructL (aEmptyText,aDisplay,aAnchorLayout,aItemMenuObserver);
    return self;
    }

// ---------------------------------------------------------------------------
// Default C++ Constructor
// ---------------------------------------------------------------------------
//

CGlxCloudViewControl::CGlxCloudViewControl(CAlfEnv &aEnv, MGlxMediaList& aMediaList,
        MGlxCloudViewMskObserver& aObserver,MGlxEnterKeyEventObserver& aObserverEnterKeyEvent
        ,MGlxCloudViewLayoutObserver& aLayoutObserver):
        iEnv(aEnv),  iMediaList(aMediaList), iObserver(aObserver)
        ,iObserverEnterKeyEvent(aObserverEnterKeyEvent),iLayoutObserver(aLayoutObserver)
        {
        TRACER("GLX_CLOUD::CGlxCloudViewControl::CGlxCloudViewControl");
        //added for scrollbar widget implementation
        iScrollEventData.mChangedMask = 0;
        iScrollEventData.mViewStartPos = 0;
        iScrollEventData.mSpan = 0;
        iScrollEventData.mViewLength = 0;    
        }

// ---------------------------------------------------------------------------
//ConstructL()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::ConstructL(const TDesC& aEmptyText,CAlfDisplay& aDisplay
        ,CAlfAnchorLayout *aAnchorLayout,MGlxItemMenuObserver& aItemMenuObserver)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::ConstructL");
    iUiUtility = CGlxUiUtility::UtilityL ();
    CAlfControl::ConstructL(iEnv);
    BindDisplay (aDisplay);
    //Initializing data for cloud view display 
    iFocusRowIndex = 0; //initially focus row will be zero

    iEmptyText = aEmptyText.AllocL();

    iTimer = CGlxBubbleTimer::NewL (this);
    
    CAlfControlGroup* ctrlGroup = iUiUtility->Env()->FindControlGroup(0);
    //Creating Grid control for floating menu bar 
    iTagsContextMenuControl = CGlxTagsContextMenuControl::NewL(aItemMenuObserver);
    if(ctrlGroup)
        {
        ctrlGroup->AppendL(iTagsContextMenuControl);
        }
    
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect (AknLayoutUtils::EMainPane, rect);
    iScreenHeight = rect.Height ();
    iParentLayout = CAlfLayout::AddNewL(*this, aAnchorLayout);                    
    iTagScreenWidth = rect.Width() - iScrollPaneHandle.iW - KRightmargin;

    if(IsLandscape())
        {	
        iTagScreenHeight = rect.Height();	
        }
    else 
        {

        iTagScreenHeight = KTagScreenHeight;
        }                                  

    iViewPortLayout = CAlfViewportLayout::AddNewL(*this, iParentLayout);      
    iLayout=CAlfAnchorLayout::AddNewL (*this,iViewPortLayout);
    iLayout->SetFlags ( EAlfVisualFlagLayoutUpdateNotification); //to get screen change notification

    iViewPortLayout->SetClipping(ETrue); 

    // Set the attribute context
    iAttributeContext = CGlxDefaultAttributeContext::NewL ();
    iAttributeContext->AddAttributeL (KMPXMediaGeneralTitle);
    iAttributeContext->AddAttributeL (KMPXMediaGeneralCount);
    iAttributeContext->SetRangeOffsets( 0, KMaxTInt / 2 );
    iMediaList.AddContextL ( iAttributeContext, KGlxFetchContextPriorityCloudView ); //highest one to use 0 or 1
    iMediaList.AddMediaListObserverL ( this); //make this control as medialist observer

    TInt listCount = iMediaList.Count();
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::ConstructL MedialistCount  %d ",
            listCount);
    
    //Display Empty Cloud View if no tags exists and if the navigation is backwards
    //Since in backward navigation the callback HandlePopulatedL() does not come
    if ( listCount ==0 && (iUiUtility->ViewNavigationDirection ()== EGlxNavigationBackwards) ) 
        {
        DisplayEmptyCloudViewL();
        }
    else	//Setting focus on the 1 st Media List item
        {
        //get the attributes from Cache
        FetchAttributeFromCacheL();

        if ( iLabels.Count ()==listCount && iAttributeContext->RequestCountL (&iMediaList)==0)
            {
            //if we navigate in forward direction, first item should be highlighted.if we are navigating in
            // backwards direction, index is not necessarily zero, it will be restored.
            if ( iUiUtility->ViewNavigationDirection ()== EGlxNavigationForwards && iMediaList.Count ())
                {
                iMediaList.SetFocusL (NGlxListDefs::EAbsolute, 0);
                }
            UpdateRowDataL ();
            }
        }
    //get touch feedback instance
    iTouchFeedback = MTouchFeedback::Instance(); 
    iPhysics = CAknPhysics::NewL(*this, NULL);
    InitPhysicsL();
    }


// ---------------------------------------------------------------------------
//VisualLayoutUpdated()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::VisualLayoutUpdated(CAlfVisual &/* aVisual*/)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::VisualLayoutUpdated");
	
	//If the grid is already shown , disable it
    if(iTagsContextMenuControl->ItemMenuVisibility())
       {
       iTagsContextMenuControl->ShowItemMenu(EFalse);
       }
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect (AknLayoutUtils::EMainPane, rect);
    if ((rect.Width() != (iTagScreenWidth + iScrollPaneHandle.iW + KRightmargin)) || (rect.Height() != iScreenHeight))
        {
        //set the new screen dimensions
        TRAP_IGNORE(UpdateLayoutL());
        }
    }

// --------------------------------------------------------------------------- 
// Destructor
// --------------------------------------------------------------------------- 
//
CGlxCloudViewControl::~CGlxCloudViewControl()

    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::~CGlxCloudViewControl");
    iCloudInfo.Close ();
    iLabels.Close();
    iMediaList.RemoveContext (iAttributeContext);
    iMediaList.RemoveMediaListObserver (this);
    delete iAttributeContext;

    if ( iUiUtility )
        {
        iUiUtility->Close ();
        }	
    delete iEmptyText;

    if ( iTimer)
        {
        iTimer->Cancel ();//cancels any outstanding requests
        delete iTimer;
        }
    delete iPhysics;
    }

// --------------------------------------------------------------------------- 
// LayoutVisibleRows()
// --------------------------------------------------------------------------- 
//
TInt CGlxCloudViewControl::LayoutVisibleRows(TPoint aStartPoint,TInt aRowStartIndex
        , TInt aRowEndIndex)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::DrawRow");
    TSize vsize;
    TPoint tl, br;
    tl = aStartPoint;
    br.iY = tl.iY + KRowHeight;
    //drawing in reverse for arabic hebrew support
    if ( GlxGeneralUiUtilities::LayoutIsMirrored ())
        {
        br.iX =  aStartPoint.iX; 
        const TInt KMaxScreenWidth = iTagScreenWidth - iScrollPaneHandle.iW;
        //Set the positions of tags in a row.
        for (TInt j = aRowStartIndex; j <= aRowEndIndex; j++)
            {
            vsize = iLabels[j]->TextExtents ();
            if ( vsize.iWidth < KMinTagSize )
                {
                vsize.iWidth = KMinTagSize;
                }
            //If the size of the tag is more than the screen size then wrap it
            if (vsize.iWidth > KMaxScreenWidth)
                {
                TAlfRealSize tagSize( KMaxScreenWidth, br.iY );
                iLabels[j]->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
                vsize = iLabels[j]->TextExtents ();
                tl.iX -= aStartPoint.iX;
                tl.iY = aStartPoint.iY;
                br.iX = tagSize.iWidth;              
                }
            else
                {
                tl.iX -= (vsize.iWidth + KColSpace);
                }
            //Set the anchor points for the tags 	            	        
            iLayout->SetAnchor (EAlfAnchorTopLeft, iLayoutIndex,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint (tl.iX, tl.iY));
            iLayout->SetAnchor (EAlfAnchorBottomRight, iLayoutIndex,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint (br.iX, br.iY));
            br.iX -= (vsize.iWidth + KColSpace); 
            iLayoutIndex += 1;
            }

        }
    else
        {
        const TInt KMaxScreenWidth = iTagScreenWidth - iScrollPaneHandle.iW;
        for (TInt j = aRowStartIndex; j <= aRowEndIndex; j++)
            {
            vsize = iLabels[j]->TextExtents ();
            if( vsize.iWidth < KMinTagSize )
                {
                vsize.iWidth = KMinTagSize;
                }
            if (vsize.iWidth > KMaxScreenWidth)
                {
                TAlfRealSize tagSize( KMaxScreenWidth, br.iY );
                TAlfRealPoint startPos( aStartPoint.iX, 0 );
                iLabels[j]->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
                tl.iX = aStartPoint.iX;
                tl.iY = aStartPoint.iY;
                br.iX = tagSize.iWidth;
                }
            else
                {
                br.iX += vsize.iWidth + KColSpace;
                }
            iLayout->SetAnchor (EAlfAnchorTopLeft, iLayoutIndex,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint (tl.iX, tl.iY));
            iLayout->SetAnchor (EAlfAnchorBottomRight, iLayoutIndex,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                    TAlfTimedPoint (br.iX, br.iY));
            tl.iX = br.iX;
            iLayoutIndex += 1;
            }
        }
    return 0;
    }




// --------------------------------------------------------------------------- 
// LayoutVisibleArea()
// 
// ---------------------------------------------------------------------------
void CGlxCloudViewControl::LayoutVisibleArea()
    {
    //screen height for boundary check:how many rows fit in.
    //find out how many rows can fit in.
    //add upper and lower margin spacing 5 pixels

    //draw the row on screens
    TPoint startpoint;
    // arabic hebrew change
    //If the layout is arabic hebrew then start positioning of tags from end
    if ( GlxGeneralUiUtilities::LayoutIsMirrored ())
        {
        startpoint.iX = iTagScreenWidth - KRightmargin;        
        }

    //else start from biginning
    else
        {
        startpoint.iX = KLeftMargin;
        }
        startpoint.iY = KNumMinRowSpace;
    //associate the active visuals with anchor layout
    GLX_LOG_INFO("GLX_CLOUD ::CGlxCloudViewControl::::LayoutVisibleArea Layout reset");
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::::LayoutVisibleArea layout Count after reset  %d ", iLayout->Count ());
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::::LayoutVisibleArea iCloudInfo[0].iStartIndex  %d ", iCloudInfo[0].iStartIndex);
    TInt endindex = iCloudInfo.Count()-1;

    for (TInt j = iCloudInfo[0].iStartIndex; j <=iCloudInfo[endindex].iEndIndex; j++)
        {
        GLX_LOG_INFO("GLX_CLOUD ::CGlxCloudViewControl::::FindEndRowIndex Entering layout append");


        TAlfTimedValue opacity;
        opacity.SetValueNow(1.0); // immediate change
        iLabels[j]->SetOpacity(opacity);
        }

    GLX_LOG_INFO("GLX_CLOUD ::CGlxCloudViewControl::::LayoutVisibleArea Exiting layout append");
    iLayoutIndex = 0;
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::::LayoutVisibleArea layout Count realloc  %d ", iLayout->Count ());
    for (TInt j = 0; j <= iEndRowIndex; j++)
        {
        GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::::LayoutVisibleArea Drawing row started  %d ", j);
        LayoutVisibleRows (startpoint,iCloudInfo[j].iStartIndex, 
                iCloudInfo[j].iEndIndex);
        startpoint.iY += KRowHeight + KNumMinRowSpace;		
        }
    TInt totalHeight = 0;
    for (TInt j = 0; j <= iCloudInfo.Count()-1; j++)
        {
        totalHeight+=KRowHeight;
        totalHeight+=KNumMinRowSpace;
        }

    //Set the virtual size of viewport to the total size of anchor.	
    if( totalHeight > iTagScreenHeight )
        {
        iViewPortLayout->SetVirtualSize(TAlfRealSize(iTagScreenWidth,totalHeight ),0); 
        iViewPortVirtualSize.iHeight = totalHeight;
        }
    else
        {
        iViewPortLayout->SetVirtualSize(TAlfRealSize(iTagScreenWidth,iTagScreenHeight ),0);
        iViewPortVirtualSize.iHeight = iTagScreenHeight;
        }    
    iViewPortVirtualSize.iWidth = iTagScreenWidth;

    iViewPortLayout->SetViewportSize(TAlfRealSize(iTagScreenWidth,iTagScreenHeight), 0);
    iViewPortLayout->SetSize(TAlfRealSize(iTagScreenWidth,iTagScreenHeight), 0);
    iViewPortSize.iWidth = iTagScreenWidth;
    iViewPortSize.iHeight = iTagScreenHeight;	
    iViewPortLayout->SetViewportPos(TAlfRealPoint(0, 0), KSlowCloudMovement);

    iViewPortPosition.iX =0;
    iViewPortPosition.iY =0;

    iScrollEventData.mSpan = iViewPortVirtualSize.iHeight ;
    iScrollEventData.mViewLength = iViewPortSize.iHeight;
    if(iScrollBarWidget)
        {
        ((IAlfScrollBarModel *)(iScrollBarWidget->model()))->changeData(iScrollEventData.mSpan,
                iScrollEventData.mViewLength,
                0);
        DisplayScrollBar();
        }
    //constructing the bubblecontainer
    CreateBubleContainer();   	

    iLayout->UpdateChildrenLayout (); //update layout

    HandleLayoutFocusChange ();
    
    MoveDownIfRequired();
    }


// --------------------------------------------------------------------------- 
// OfferEventL()
// --------------------------------------------------------------------------- 
//
TBool CGlxCloudViewControl::OfferEventL(const TAlfEvent &aEvent)
    {   TRACER("GLX_CLOUD:: CGlxCloudViewControl::OfferEventL");

    //check if key inputs needs handling
    TBool consumed = EFalse;
    if ( ( iLabels.Count () == 0 ) ) 
        {
        GLX_LOG_INFO( "GLX_CLOUD ::CGlxCloudViewControl::offerkeyeventL no key press returning  ");
        return EFalse; //return as no futher processing is required
        }

    if ( aEvent.IsKeyEvent ()&& aEvent.Code() == EEventKey )
        {
        switch (aEvent.KeyEvent().iCode)            
            {	
            //@ EABI-7R7FRU Fute failure: Tag views enter key has no functionality.
            case EKeyEnter :
            case EKeyDevice3:
                {
                iObserverEnterKeyEvent.HandleEnterKeyEventL( (TInt)EAknCmdOpen );
                consumed= ETrue;
                }
                break;
            case EKeyUpArrow:
                {             
                if(iCloudInfo.Count() >1 )
                    {
                    HandleKeyUpL ();
                    consumed = ETrue;
                    iFocusRowIndex = RowNumber (iMediaList.FocusIndex ()); 

                    MoveUpIfRequired();                   
                    }
                }
                break;

            case EKeyDownArrow:
                {              
                if (iCloudInfo.Count() > 1)
                    {
                    HandleKeyDownL ();
                    consumed = ETrue;
                    iFocusRowIndex = RowNumber (iMediaList.FocusIndex ());

                    MoveDownIfRequired();                                 
                    }                  
                }
                break;

            case EKeyLeftArrow:
            case EKeyPrevious:
                {
                // arabic hebrew change
                if ( GlxGeneralUiUtilities::LayoutIsMirrored () )
                    {
                    if ( iMediaList.FocusIndex() == iMediaList.Count() - 1 )
                        {
                        iMediaList.SetFocusL (NGlxListDefs::EAbsolute, 0);
                        }
                    else
                        {
                        iMediaList.SetFocusL (NGlxListDefs::EAbsolute,
                                iMediaList.FocusIndex ()+ 1);
                        }
                    iScrollDirection = 0;
                    }
                else
                    {
                    if ( iMediaList.FocusIndex ()== 0 )
                        {
                        iMediaList.SetFocusL (NGlxListDefs::EAbsolute,
                                iMediaList.Count() - 1 );
                        }
                    else
                        {
                        iMediaList.SetFocusL (NGlxListDefs::EAbsolute,
                                iMediaList.FocusIndex ()- 1);
                        }
                    iScrollDirection = 1;
                    }
                consumed = ETrue;
                TInt focusRowIndex = RowNumber (iMediaList.FocusIndex ());
                if(iFocusRowIndex != focusRowIndex)
                    {
                    iFocusRowIndex = focusRowIndex; 
                    if( iScrollDirection == 0 )
                        {                      
                        MoveDownIfRequired();
                        }
                    else
                        {
                        MoveUpIfRequired();
                        }
                    }                
                }

                break;

            case EKeyRightArrow:
            case EKeyNext:
                {               
                // arabic hebrew change
                if ( GlxGeneralUiUtilities::LayoutIsMirrored () )
                    {
                    if ( iMediaList.FocusIndex ()== 0 )
                        {
                        iMediaList.SetFocusL (NGlxListDefs::EAbsolute,
                                iMediaList.Count() - 1 );
                        }
                    else
                        {
                        iMediaList.SetFocusL (NGlxListDefs::EAbsolute,
                                iMediaList.FocusIndex ()- 1);
                        }
                    iScrollDirection = 1;
                    }
                else
                    {
                    if ( iMediaList.FocusIndex ()== iMediaList.Count() - 1 )
                        {
                        iMediaList.SetFocusL (NGlxListDefs::EAbsolute, 0);
                        }
                    else
                        {
                        iMediaList.SetFocusL (NGlxListDefs::EAbsolute,
                                iMediaList.FocusIndex ()+ 1);
                        }
                    iScrollDirection = 0;
                    }
                TInt focusRowIndex = RowNumber (iMediaList.FocusIndex ());

                if( iFocusRowIndex != focusRowIndex )
                    {
                    iFocusRowIndex = focusRowIndex;  
                    if( iScrollDirection == 1 )
                        {                           
                        MoveUpIfRequired();                               
                        }
                    else
                        {                           
                        MoveDownIfRequired();                               
                        }
                    }
                }
                consumed = ETrue;
                break;

            default:
                break;
            }
        }

    if(aEvent.IsPointerEvent() )
        {
        //if its a pointer event
        consumed = HandlePointerEventL(aEvent);
        }
    return consumed;
    }

// --------------------------------------------------------------------------- 
// HandleKeyUp()
// --------------------------------------------------------------------------- 
//
void CGlxCloudViewControl::HandleKeyUpL()
    {
    TRACER("GLX_CLOUD:: CGlxCloudViewControl::HandleKeyUpL");

    //If the first item is focused and if we are navigating upwards,then set the focus to last element.
    if( iFocusRowIndex ==  0 )
        {               
        iMediaList.SetFocusL (NGlxListDefs::EAbsolute,
                iMediaList.Count() - 1);
        }

    //else set the focus to the item which is in the previous row,that overlaps with midpoint of the 
    //focused item.
    else
        { 
        TInt prevrownum = iFocusRowIndex - 1;
        SetRelativeFocusL(prevrownum);		
        }

    iScrollDirection = 1;
    }


// --------------------------------------------------------------------------- 
// HandleKeyDown()
// --------------------------------------------------------------------------- 
//
void CGlxCloudViewControl::HandleKeyDownL()
    {
    TRACER("GLX_CLOUD:: CGlxCloudViewControl::HandleKeyDownL");

    // If the last item is focused and if we are navigating downwards,then set the
    // focus to first element.
    if (iFocusRowIndex == iCloudInfo.Count() - 1)
        {
        iMediaList.SetFocusL(NGlxListDefs::EAbsolute, 0);
        }

    //else set the focus to the item which is in the next row,that overlaps with midpoint of the 
    //focused item.     
    else
        { 
        TInt nextrownum = iFocusRowIndex + 1;            
        SetRelativeFocusL(nextrownum);	       
        }

    iScrollDirection = 0;
    }

// --------------------------------------------------------------------------- 
// HandleLayoutFocusChange()
// --------------------------------------------------------------------------- 
//
void CGlxCloudViewControl::HandleLayoutFocusChange()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandleLayoutFocusChange");

    if ( iLabels.Count()-1 >= iMediaList.FocusIndex())
        {
        //to highlight focused element 
        SetFocusColor(); 
        }
    }


// --------------------------------------------------------------------------- 
// FocusUpdate()
// --------------------------------------------------------------------------- 
//
void CGlxCloudViewControl::FocusUpdate()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::FocusUpdate");
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::FocusUpdate HighLighted RowNum  %d ",iFocusRowIndex);
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::FocusUpdate End RowNumn %d ",iCloudInfo.Count()-1);
    iFocusRowIndex = RowNumber (iMediaList.FocusIndex ());
    iEndRowIndex = iCloudInfo.Count()-1;
    LayoutVisibleArea();
    }


// --------------------------------------------------------------------------- 
// ResetLayout()
// --------------------------------------------------------------------------- 
//
void CGlxCloudViewControl::UpdateLayout()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::UpdateLayout()");
    iCloudInfo.Close (); //row information is reset
    for(TInt i = 0;i<iLabels.Count();i++)
        {
        iLabels[i]->RemoveAndDestroyAllD(); // removes from control
        iLabels[i] = NULL;
        }        
    iLabels.Reset();
    }


// --------------------------------------------------------------------------- 
// GetRownum()
// --------------------------------------------------------------------------- 
//
TInt CGlxCloudViewControl::RowNumber(TInt aItemIndex) const
{

TRACER("GLX_CLOUD::CGlxCloudViewControl::GetRownum");
//Get the rownumber of the given item index.
TInt i;
for (i = 0; i < iCloudInfo.Count (); i++)
    {
    if ( (aItemIndex >= iCloudInfo[i].iStartIndex) && (aItemIndex <=iCloudInfo[i].iEndIndex))
        break;
    }
return i;

}

// --------------------------------------------------------------------------- 
// FetchAttributeFromCacheL()
// --------------------------------------------------------------------------- 
//
void CGlxCloudViewControl::FetchAttributeFromCacheL()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::FetchAttributeFromCacheL");
    GLX_LOG_INFO("GLX_CLOUD ::CGlxCloudViewControl::FetchAttributeFromCache MediaList already filled" );
    iLayoutObserver.HandleLayoutChanged();
    if ( iLabels.Count ()==0)
        {
        for (TInt i=0; i<iMediaList.Count();i++)
            {
            CAlfTextVisual *tempVisual= NULL;
            tempVisual = CAlfTextVisual::AddNewL (*this,iLayout);
            iLabels.InsertL (tempVisual, i); //appending the visual text   /
            TAlfTimedValue opacity;
            opacity.SetValueNow(0.0);
            iLabels[i]->SetOpacity(opacity);
            }
        }

    for (TInt i=0; i<iMediaList.Count(); i++)
        {
        GLX_LOG_INFO1("GLX_CLOUD::Fetching title for item %d", i );
        // get the media item
        TGlxMedia item = iMediaList.Item( i );
        // get the title and set it to the text visual
        iLabels[i]->SetTextL( item.Title() );
        }
    }


//medialist Observers

// ---------------------------------------------------------------------------
// HandleItemAddedL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
        MGlxMediaList* aList)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandleItemAddedL");
    GLX_LOG_INFO1("GLX_CLOUD::CGlxCloudViewControl::HandleItemAddedL start index  %d ", aStartIndex);
    GLX_LOG_INFO1("GLX_CLOUD::CGlxCloudViewControl::HandleItemAddedL end index  %d ", aEndIndex);
    GLX_LOG_INFO1("GLX_CLOUD::CGlxCloudViewControl::HandleItemAddedL  media list count  %d ", aList->Count());
    GLX_LOG_INFO1("GLX_CLOUD::CGlxCloudViewControl::HandleItemAddedL  attribute request count %d ", iAttributeContext->RequestCountL(aList));

    if ( iEmptyString )
        {
        // remove the empty string so it does not fool around in animations
        iEmptyString->RemoveAndDestroyAllD(); // removes from control
        iEmptyString = NULL;
        }
    //The new item has been added and it is required to cleanup the visuals and info bubble
    //for the existing tags(if any).	
    //when a filter is applied attribute request count is zero but medialist count in non zero
    if (iAttributeContext->RequestCountL(aList) == 0 )
        {
        //reset the layout and get the items from cache
        UpdateLayout(); //remove all the visuals from the layout.layout is empty now.
        iCloudInfo.Close (); //row information is reset
        //get the new array contents from cache.no need to reset the array as the number
        //of items in array will remain same.
        FetchAttributeFromCacheL();
        UpdateRowDataL (); //updates the row data and reassigns font sizes and draw the layout on screen.
        }

    InitPhysicsL();
    }


// ---------------------------------------------------------------------------
// HandleMediaL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandleMediaL");
    }

// ---------------------------------------------------------------------------
// HandleItemRemovedL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex,
        MGlxMediaList* aList)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandleItemRemovedL");
    GLX_LOG_INFO1("GLX_CLOUD::CGlxCloudViewControl::HandleItemRemovedL Start RowNum  %d ", aStartIndex);
    GLX_LOG_INFO1("GLX_CLOUD::CGlxCloudViewControl::HandleItemRemovedL iCloudCount  %d ", aEndIndex);
    GLX_LOG_INFO1("GLX_CLOUD::CGlxCloudViewControl::HandleItemRemovedL  media list count  %d ", aList->Count());

    //when a filter is applied attribute request count is zero but medialist count in non zero
    if (iAttributeContext->RequestCountL(aList) == 0 )
        {
        //reset the layout and get the items from cache
        UpdateLayout(); //remove all the visuals from the layout.layout is empty now.       
        if( aList->Count() > 0 )
            {
            //get the new array contents from cache.no need to reset the array as the number
            //of items in array will remain same.
            FetchAttributeFromCacheL();
            UpdateRowDataL (); //updates the row data and reassigns font sizes and draw the layout on screen.
            }
        else
            {
            DisplayEmptyCloudViewL();
            }
        }
    InitPhysicsL();
    }

// ---------------------------------------------------------------------------
// HandleItemModifiedL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::HandleItemModifiedL(
        const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandleItemModifiedL");
    }

// ---------------------------------------------------------------------------
// HandleAttributesAvailableL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::HandleAttributesAvailableL(TInt aItemIndex,
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandleAttributesAvailableL");
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::HandleAttributesAvailableL  iLabels.Count()  %d ",iLabels.Count());
    TInt attCount = aAttributes.Count ();
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::HandleAttributesAvailableL  count: %d ",attCount);
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::HandleAttributesAvailableL  Item Index: %d ", aItemIndex);

    TBool relevantAttribute = EFalse;
    for ( TInt i = 0 ; i < attCount ; i++ )
        {
        if ( ( aAttributes[i] == KMPXMediaGeneralTitle ) 
                || ( aAttributes[i] == KMPXMediaGeneralCount ) )
            {
            relevantAttribute = ETrue;
            }
        }
    //if the title array has been loaded completely and request count for attribute context becomes 
    //zero update the row data and arrange layout
    if ( ( iAttributeContext->RequestCountL(aList) ==0 ) 
            && ( aList->Count() != 0) && relevantAttribute )
        {
        //get the new array contents from cache.no need to reset the array as the number
        //of items in array will remain same.
        UpdateLayout(); //remove all the visuals from the layout.layout is empty now.        
        FetchAttributeFromCacheL();
        //if we navigate in forward direction, first item should be highlighted.if we are navigating in
        // backwards direction, index is not necessarily zero, it will be restored.
        if ( iUiUtility->ViewNavigationDirection ()== EGlxNavigationForwards && iMediaList.Count ())
            {
            iMediaList.SetFocusL (NGlxListDefs::EAbsolute, 0);
            }
        //generate row structures and draw rows on screen
        UpdateRowDataL ();

        InitPhysicsL();
        }
    }

// ---------------------------------------------------------------------------
// HandleFocusChangedL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, TInt aNewIndex,
        TInt aOldIndex, MGlxMediaList* /*aList*/)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandleFocusChangedL");
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::HandleFocusChangedL aNewIndex %d ", aNewIndex);
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::HandleFocusChangedL aOldIndex %d ",aOldIndex);
    if ( (aOldIndex >= 0) && (iLabels.Count()-1 >= aOldIndex) )
        {      		  
        //Reset the focused color
        iLabels[aOldIndex]->SetColor (KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG6);
        HandleLayoutFocusChange ();
        }
    }

// ---------------------------------------------------------------------------
// HandleItemSelectedL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::HandleItemSelectedL(TInt /*aIndex*/,
        TBool /*aSelected*/, MGlxMediaList* /*aList*/)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandleItemSelectedL");
    }

// ---------------------------------------------------------------------------
// HandleMessageL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::HandleMessageL(const CMPXMessage& /*aMessage*/,
        MGlxMediaList* /*aList*/)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandleMessageL");
    }

// ---------------------------------------------------------------------------
// HandlePopulatedL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::HandlePopulatedL(MGlxMediaList* /*aList*/)
    {
    //Display the Empty clould view only when we get this call back.
    // At this stage the medialist will be populated.    
    if(iMediaList.Count()<=0)
        {
        DisplayEmptyCloudViewL();
        }
    }

// ---------------------------------------------------------------------------
//UpdateRowDataL()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::UpdateRowDataL()

    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::UpdateRowData()");

    TInt rowStartingTagIndex = 0;
    TInt lastRowStartTagIndex = 0;
    TInt currentTagIndex = 0;
    TSize currentTagSize(0, 0);
    TGlxCloudInfo cloudInfo;
    TInt rowHeight = 0;
    TInt rowWidth = 0;
    TInt itemUsageCount = 0;
    const TInt KLabelsCount = iLabels.Count();
    iScrollPaneHandle = AknLayoutScalable_UiAccel::aa_scroll_pane(0).LayoutLine();
    const TInt KMaxScreenWidth = iTagScreenWidth - KRightmargin - iScrollPaneHandle.iW;

    iCloudInfo.Close (); //delete existing structures for orintation change


    //Setting the Font Styles based on association counts
    //Finding the maximum value of image association
    //Determining the Font(Style) for each visual
    TInt maxCount= MaxUsageCount (); //Maximum Association count 
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData  mediaCount %d ", maxCount);
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData  iLabels.Count()  %d ",iLabels.Count());
    if ( 0 == maxCount )
        {
        return; //do not proceed if max count is zero
        }

    for (TInt i = 0; i < KLabelsCount; i++)
        {
        TInt percent( 0 );
        GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData  Item Counter value  %d ",i);
        itemUsageCount = UsageCount(i);
        GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData  Usage Count  %d ", itemUsageCount);
        percent = ((itemUsageCount * 99)/ maxCount);
        GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData  iLabels.Count()  %d ",iLabels.Count());

        // percent is between 0 and 99 so divide it by 20 to make it 0, 1, 2, 3 or 4
        // as there is 5 different font sizes
        SetPropertiesL( i, TInt( percent / 20 ) );
        }

    while ( currentTagIndex < KLabelsCount )
        {
        //Determining text size of visual.
        currentTagSize = iLabels[currentTagIndex]->TextExtents();
        if( currentTagSize .iWidth < KMinTagSize )
            {
            currentTagSize.iWidth = KMinTagSize;
            }
        GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData  currentTagSize.iHeight %d ", currentTagSize.iHeight);

        // The current row's height will always be the largest height of all the tags in that row
        if ( currentTagSize.iHeight > rowHeight )
            {
            rowHeight = currentTagSize.iHeight;
            }

        // Current row width will be progressively incremented to fit as many tags as possible
        rowWidth += currentTagSize.iWidth + 10;
        GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData   currentTagSize.iWidth %d ", currentTagSize.iWidth);
        GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData  rowWidth %d ", rowWidth);

        // If the tag size crosses the Screen Size, Then wrap it.
        if ( currentTagSize.iWidth > KMaxScreenWidth )
            {
            lastRowStartTagIndex = ((rowStartingTagIndex == currentTagIndex)?rowStartingTagIndex : currentTagIndex-1);
            AppendToCloudArrayL( cloudInfo,/* rowHeight,*/  rowStartingTagIndex, lastRowStartTagIndex );
            // Reset row height and row width for the next line
            rowHeight = 0;
            rowWidth = 0;
            if (lastRowStartTagIndex != currentTagIndex-1 )
                {
                currentTagIndex++; 
                }
            rowStartingTagIndex = currentTagIndex;
            lastRowStartTagIndex++;
            }

        // If the current row's width is overflowing the screen width
        // then we must fit the current visual into the next row.
        // Do that check now and adjust accordingly.
        // Fix for EAHN-7BZD78 is to exclude the gap value between the row's tags from the logic
        else if ( rowWidth - 10 > KMaxScreenWidth )
            {
            GLX_LOG_INFO("GLX_CLOUD :: CGlxCloudViewControl::UpdateRowData Row added");
            lastRowStartTagIndex = currentTagIndex - 1;
            AppendToCloudArrayL( cloudInfo,rowStartingTagIndex, lastRowStartTagIndex );
            lastRowStartTagIndex++;
            // Reset row height and row width for the next line
            rowHeight = 0;
            rowWidth = 0;
            rowStartingTagIndex = currentTagIndex;
            GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData  iCloudInfo.count  %d ", iCloudInfo.Count());
            } 

        // need to work with next tag only if current tag has been taken care of
        if ( rowWidth )
            {
            currentTagIndex++; 
            }

        } // while ( currentTagIndex < KLabelsCount )

    if( lastRowStartTagIndex < KLabelsCount)
        {
        AppendToCloudArrayL( cloudInfo,lastRowStartTagIndex, KLabelsCount - 1 );
        }

    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::UpdateRowData  iCloudInfo.count loop exit  %d ", iCloudInfo.Count());
    FocusUpdate (); //Start screen drawing 

    iEndRowIndex = iCloudInfo.Count()-1;
    }

// ---------------------------------------------------------------------------
// SetProperties
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::SetPropertiesL(TInt aIndex, TInt aScale)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::SetProperties()");
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::SetProperties layout Count after reset  %d ", iLayout->Count());
    //Retrieving theme color

    TAlfTimedValue scale(0);
    TRAP_IGNORE(iLabels[aIndex]->EnableTransformationL());   
    TAlfTimedValue opacity;
    opacity.SetStyle (EAlfTimedValueStyleLinear);
    opacity.SetValueNow(0.0);
    iLabels[aIndex]->SetOpacity(opacity);
    iLabels[aIndex]->SetAlign (EAlfAlignHLocale, EAlfAlignVBottom); //setting allignment
    iLabels[aIndex]->SetColor (KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
    //custom style cod
    TInt typefaceSecondaryStyleId = CGlxCloudViewControl::Env().TextStyleManager().CreatePlatformTextStyleL(
            EAknLogicalFontSecondaryFont,EAlfTextStyleNormal);
    TInt typefacePrimaryStyleId = CGlxCloudViewControl::Env().TextStyleManager().CreatePlatformTextStyleL(
            EAknLogicalFontPrimaryFont,EAlfTextStyleNormal);
    CAlfTextStyle* stylePrimary = CGlxCloudViewControl::Env().TextStyleManager().TextStyle(
            typefacePrimaryStyleId);
    CAlfTextStyle* styleSecondary = CGlxCloudViewControl::Env().TextStyleManager().TextStyle(
            typefaceSecondaryStyleId);
    switch (aScale)
        {
        case 0:
            {
            styleSecondary->SetTextSizeInPixels (KFontSmallest, 0);
            iLabels[aIndex]->SetTextStyle (typefaceSecondaryStyleId);
            TAlfTimedValue opacity;
            opacity.SetValueNow(0.0);
            iLabels[aIndex]->SetOpacity(opacity);
            scale.SetTarget(1, 1000);
            break;
            }
        case 1:
            {
            styleSecondary->SetTextSizeInPixels (KFontSmaller, 0);
            iLabels[aIndex]->SetTextStyle (typefaceSecondaryStyleId);
            scale.SetTarget(1, 1200);
            TAlfTimedValue opacity;
            opacity.SetValueNow(0.0);
            iLabels[aIndex]->SetOpacity(opacity);
            break;
            }
        case 2:
            {
            stylePrimary->SetTextSizeInPixels (KFontMedium, 0);
            iLabels[aIndex]->SetTextStyle (typefacePrimaryStyleId);
            scale.SetTarget(1, 1500);
            TAlfTimedValue opacity;
            opacity.SetValueNow(0.0);
            iLabels[aIndex]->SetOpacity(opacity);
            break;
            }
        case 3:
            {
            stylePrimary->SetTextSizeInPixels (KFontLarger, 0);
            iLabels[aIndex]->SetTextStyle (typefacePrimaryStyleId);
            scale.SetTarget(1, 1800);
            TAlfTimedValue opacity;
            opacity.SetValueNow(0.0);
            iLabels[aIndex]->SetOpacity(opacity);
            break;
            }
        case 4:
            {
            stylePrimary->SetTextSizeInPixels (KFontLargest, 0);
            iLabels[aIndex]->SetTextStyle (typefacePrimaryStyleId);
            scale.SetTarget(1, 2200);
            TAlfTimedValue opacity;
            opacity.SetValueNow(0.0);
            iLabels[aIndex]->SetOpacity(opacity);
            break;
            }
        }

    iLabels[aIndex]->Transformation().Scale (scale, scale);
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::SetProperties layout Count after reset  %d ", iLayout->Count());
    }

// ---------------------------------------------------------------------------
// DisplayEmptyCloudView
// ---------------------------------------------------------------------------
// 
void CGlxCloudViewControl::DisplayEmptyCloudViewL()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::DisplayEmptyCloudView");
    if( !iEmptyString )
        {
        iEmptyString = CAlfTextVisual::AddNewL ( *this, iViewPortLayout );
        iViewPortLayout->SetSize(TAlfRealSize(iTagScreenWidth,iTagScreenHeight), 0);
        iLayout->UpdateChildrenLayout (); //update layout
        }
    //Setting text color to default theme text color.
    iEmptyString->SetColor(KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
    iEmptyString->SetTextStyle(iUiUtility->TextStyleIdL(EAknLogicalFontPrimaryFont
            ,KPrimaryFontSize));
    iEmptyString->SetWrapping( CAlfTextVisual::ELineWrapBreak );
    iEmptyString->SetTextL (iEmptyText->Des());
    // no shadow wanted for the text
    iEmptyString->EnableShadow( EFalse );

    //disable msk
    iLayoutObserver.HandleLayoutChanged();
    iScrollEventData.mSpan = 0;
    iScrollEventData.mViewLength = 0;
    DisplayScrollBar();
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::DisplayEmptyCloudView layout Count after reset  %d ", iLayout->Count());
    }

// ---------------------------------------------------------------------------
// MaxUsageCount()
// ---------------------------------------------------------------------------
//
TInt CGlxCloudViewControl::MaxUsageCount()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::GetMaxusageCount");
    TInt maxValue=0;
    const TInt mediaListSize = iMediaList.Count();
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::GetMaxusageCount MediaList Count %d ",iMediaList.Count());
    for (TInt k=0; k < mediaListSize; k++)
        {
        TInt count = UsageCount (k);
        if ( count > maxValue)
            {
            GLX_LOG_INFO("GLX_CLOUD :: CGlxCloudViewControl::GetMaxusageCount  in the loop");
            maxValue=count;
            }
        }
    return maxValue;
    }

// ---------------------------------------------------------------------------
// UsageCount()
// ---------------------------------------------------------------------------
//
TInt CGlxCloudViewControl::UsageCount(TInt aIndex)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::UsageCount");
    GLX_LOG_INFO1("GLX_CLOUD :: CGlxCloudViewControl::UsageCount Item index: %d ", aIndex);
    TMPXAttribute attrCount(KMPXMediaGeneralCount); // get the attrCount
    TInt usageCount =0;
    const CGlxMedia* media = iMediaList.Item(aIndex).Properties ();
    if ( media)
        {
        if ( media->IsSupported (attrCount))
            {
            usageCount = media->ValueTObject<TInt> (attrCount);
            GLX_LOG_INFO1("GLX_CLOUD :: CGlxCloudViewControl::UsageCount Count: %d ", usageCount);
            }
        }
    return usageCount;
    }

// ---------------------------------------------------------------------------
// SetFocusColor()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::AppendToCloudArrayL( 
        TGlxCloudInfo& aCloudInfo,const TInt& aStartIndex, const TInt& aEndIndex )
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::AppendToCloudArrayL");
    aCloudInfo.iStartIndex = aStartIndex;
    aCloudInfo.iEndIndex = aEndIndex; 
    iCloudInfo.AppendL( aCloudInfo );
    }

// ---------------------------------------------------------------------------
// SetFocusColor()
// ---------------------------------------------------------------------------
//
void  CGlxCloudViewControl::SetFocusColor()
    {
    iLabels[iMediaList.FocusIndex()]->SetColor (KAknsIIDQsnHighlightColors ,EAknsCIQsnHighlightColorsCG3); 
    }


// ---------------------------------------------------------------------------
// SetBubleMidPoint()
// ---------------------------------------------------------------------------
//
void  CGlxCloudViewControl::SetBubleMidPoint(TPoint& aMidPoint)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::SetBubleMidPoint");
    //Substract the viewport position so as to set the buble at the right position on the screen     
    aMidPoint.iX-=iViewPortPosition.iX;
    aMidPoint.iY-=iViewPortPosition.iY;
    }

// ---------------------------------------------------------------------------
// CreateBubleContainer()
// ---------------------------------------------------------------------------
//   
void CGlxCloudViewControl::CreateBubleContainer()
    {   
    }

// ---------------------------------------------------------------------------
// MoveUpIfRequired()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::MoveUpIfRequired()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::MoveUpIfRequired");
    //get the focused item's position
    TPoint point;
    iLayout->ChildPos(iMediaList.FocusIndex(),point);
    TSize size;
    iLayout->ChildSize(iMediaList.FocusIndex(),size);
    TInt focus = iMediaList.FocusIndex();
    TInt rownumber = RowNumber(focus);
    //if the focused item is not visible then move the viewport
    if(point.iY < iViewPortPosition.iY)         
        { 
        //set the new position of viewport
        TInt offeset = iViewPortPosition.iY - point.iY;
        iViewPortPosition.iY-=offeset;
        if( (iViewPortPosition.iY < 0) )
            {
            iViewPortPosition.iY = 0;
            }   
        }    
    //if its key event then it should be cyclic
    else if( rownumber == (iCloudInfo.Count()-1))
        {
        iViewPortPosition.iY = iViewPortVirtualSize.iHeight - iViewPortSize.iHeight;
        }
    iViewPortLayout->SetViewportPos(iViewPortPosition, KSlowCloudMovement);
    iScrollEventData.mViewStartPos = iViewPortPosition.iY;
    Scroll();  
    }


// ---------------------------------------------------------------------------
// MoveDownIfRequired()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::MoveDownIfRequired()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::MoveDownIfRequired");  
    //get the focused item's position
    TPoint point;
    iLayout->ChildPos(iMediaList.FocusIndex(),point);
    TSize size;
    iLayout->ChildSize(iMediaList.FocusIndex(),size);
    //if the focused visual is not visible then move the viewport 
    if(point.iY+size.iHeight > iViewPortPosition.iY+iViewPortSize.iHeight )
        { 
        //set the new position of viewport
        TInt offeset = (point.iY+size.iHeight) - (iViewPortPosition.iY+iViewPortSize.iHeight);
        iViewPortPosition.iY+=offeset;
        if( (iViewPortPosition.iY > iViewPortVirtualSize.iHeight) )
            {
            iViewPortPosition.iY = iViewPortVirtualSize.iHeight - iViewPortSize.iHeight;
            } 
        }
    //if its key event then it should be cyclic
    else if (iMediaList.FocusIndex() == 0)
        {
        iViewPortPosition.iY = 0;
        }     
    iViewPortLayout->SetViewportPos(iViewPortPosition, KSlowCloudMovement);
    iScrollEventData.mViewStartPos = iViewPortPosition.iY;
    if(iScrollBarWidget)
        {
        Scroll();           
        }
    }     

// ---------------------------------------------------------------------------
// MoveDownIfRequired()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::CalculateBubleMidPoint()
    { 
    TRACER("GLX_CLOUD::CGlxCloudViewControl::CalculateBubleMidPoint");
    TPoint midpoint;
    TAlfRealRect focussedItemRect;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect (AknLayoutUtils::EMainPane, mainPaneRect);

    CAlfVisual& visual = iLayout->Visual( iMediaList.FocusIndex() );
    focussedItemRect = visual.DisplayRect();

    if( GlxGeneralUiUtilities::LayoutIsMirrored () )
        {  
        midpoint.iX = focussedItemRect.iBr.iX +
        ((focussedItemRect.iTl.iX - focussedItemRect.iBr.iX )/2);
        }

    else 
        {
        midpoint.iX=focussedItemRect.iTl.iX +
        ((focussedItemRect.iBr.iX - focussedItemRect.iTl.iX )/2);
        }

    midpoint.iY=focussedItemRect.iTl.iY+
                ((focussedItemRect.iBr.iY - focussedItemRect.iTl.iY )/2); 
    TPoint anchorRect;

    //Get the position of anchor layout
    iViewPortLayout->ChildPos (0, anchorRect);

    //Add the differance to the midpoint
    midpoint.iX+=anchorRect.iX;
    midpoint.iY+=anchorRect.iY; 

    //Substract from the viewport position the viewport position    
    SetBubleMidPoint(midpoint);   
    } 

// ---------------------------------------------------------------------------
// HandlePointerEventL()
// ---------------------------------------------------------------------------
//  
TBool CGlxCloudViewControl::HandlePointerEventL( const TAlfEvent &aEvent )
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::HandlePointerEventL");
    CAlfVisual* tappedvisual = aEvent.Visual();	
    TBool consumed = EFalse;
    

    if(aEvent.PointerEvent().iType == TPointerEvent::EButton1Down)
        {	
        iDownEventReceived = EFalse;
        //reset variables & Physics simulator 
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        iDragging = EFalse;
        iPhysicsStarted = EFalse;
        iStartTime.HomeTime();
        iViewDragged = EFalse;
        Display()->Roster().SetPointerEventObservers(
                EAlfPointerEventReportDrag + EAlfPointerEventReportLongTap
                        + EAlfPointerEventReportUnhandled, *this);
        Display()->Roster().DisableLongTapEventsWhenDragging(*this);
        
        //If the grid is already shown , disable it
        if(iTagsContextMenuControl->ItemMenuVisibility())
           {
           iDownEventReceived = ETrue;
           iTagsContextMenuControl->ShowItemMenu(EFalse);
           return ETrue;
           }
        if(tappedvisual)
            {
            for(TInt index=0;index<iLayout->Count();index++)
                {    	
                CAlfVisual* layoutvisual = &(iLayout->Visual(index));
                //if the tapped visual is same as the visual in the layout then focus that visual
                if(layoutvisual == tappedvisual)
                    {
                    iTouchFeedback->InstantFeedback(ETouchFeedbackBasic);
                    iMediaList.SetFocusL (NGlxListDefs::EAbsolute, index);

                    //Start the timer to interpret longpress events
                    iTimerComplete = EFalse;
                    iTimer->Cancel ();//cancels any outstanding requests
                    iTimer->SetDelay (KLongPressTimer);
                    
                    consumed = ETrue;
                    break;
                    }
                }
            }
        }
    else if(iDownEventReceived)
        {
        consumed = ETrue;
        }
    else if (aEvent.PointerEvent().iType == TPointerEvent::EDrag)
        {
        GLX_LOG_INFO("GLX_CLOUD :: CGlxCloudViewControl::HandlePointerEventL(EDrag) event");
        
        iTouchFeedback->InstantFeedback(ETouchFeedbackBasic);

        consumed = HandleDragL(aEvent.PointerEvent());
        }
    else if (aEvent.PointerUp())
        {
        Display()->Roster().SetPointerEventObservers(0, *this);
        consumed = ETrue;
        
        //If the long press timer is completed , and if upevent is received.. ignore it
        if(iTimerComplete)
            {
            consumed =  ETrue;
            }

        //Check if dragging actually happened using iViewDragged 
        if (iDragging && iViewDragged)
            {
			iTimer->Cancel ();//cancels any outstanding requests
						
            iDragging = EFalse;
            TPoint drag = iStart - aEvent.PointerEvent().iPosition;
            iPhysics->StartPhysics(drag, iStartTime);
            iPhysicsStarted = ETrue;
            }
        //If dragging not happened consider it as Tapped event
        //When it recognises the long press event ,  and if up event is received..Ignore it 
        else if (tappedvisual && !iViewDragged && !iTimerComplete)
            {
           for (TInt index = 0; index < iLayout->Count(); index++)
                {
                CAlfVisual* layoutvisual = &(iLayout->Visual(index));
                //if the tapped visual is same as the visual in the layout then focus that visual
                if (layoutvisual == tappedvisual)
                    {
                    TInt focus = iMediaList.FocusIndex();
                    if (index != focus)
                        {
                        iTouchFeedback->InstantFeedback( ETouchFeedbackBasic );
                        TInt focusrowindex = iFocusRowIndex;
                        iMediaList.SetFocusL(NGlxListDefs::EAbsolute, index);
                        SetFocusColor();
                        iFocusRowIndex = RowNumber (iMediaList.FocusIndex ());
                        
                        if( iFocusRowIndex > focusrowindex)
                            {
                            GLX_LOG_INFO("GLX_CLOUD :: CGlxCloudViewControl::HandleDragL,b4 movedown");
                            iScrollDirection = 0;               
                            MoveDownIfRequired();               
                            }
                        else if( iFocusRowIndex < focusrowindex )
                            {
                            GLX_LOG_INFO("GLX_CLOUD :: CGlxCloudViewControl::HandleDragL,b4 moveup");
                            iScrollDirection = 1;                
                            MoveUpIfRequired();                 
                            }
                        }

                    iTouchFeedback->InstantFeedback(ETouchFeedbackBasic);
                    iObserverEnterKeyEvent.HandleEnterKeyEventL((TInt) EAknCmdOpen);
                    consumed = ETrue;
                    break;
                    }
                }
            }
        iViewDragged = EFalse;
        }
    return consumed;
    }

// ---------------------------------------------------------------------------
// HandleDragL()
// ---------------------------------------------------------------------------
//
TBool CGlxCloudViewControl::HandleDragL(const TPointerEvent& aPointerEvent)
    {
    TBool consumed(EFalse);
    
    // If Physics Emulation is going on, no need to entertain drag event
    if (iPhysicsStarted)
        return consumed;

    //Simply ignore the first drag event as there is huge difference between position 
    //coordinates in drag event and corordinates in normal pointer down / up events  
    if (!iDragging)
        {
        //Note the current position for future reference
        iStart = aPointerEvent.iPosition;
        iPrev = iStart; 
        iDragging = ETrue;
        return consumed;
        }

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    TInt cntrlTopYLimit = rect.iTl.iY;
    TInt cntrlBottomYLimit = rect.iBr.iY;
    TInt delta = iPrev.iY - aPointerEvent.iPosition.iY;

    //Check for physics threshold, before regidterting drag corordinates
    TInt deltaAbs = delta < 0 ? -delta : delta;
    TBool panning = deltaAbs >= iPhysics->DragThreshold();
    if (panning)
        {
        TPoint deltaPt = iPrev - aPointerEvent.iPosition;
        iPhysics->RegisterPanningPosition(deltaPt);
        iPrev = aPointerEvent.iPosition;
        iViewDragged = ETrue;
        }
    consumed = ETrue;

    //Calculate margin after which we might get pointer out of screen, and we might not 
    //get further pointer events. Also This is currently only 10% of total delta we can find, 
    //to get margin corresponding to movement of user flicking speed
    //This implies higher speed - higher margin
    //Lower speed - 0 margin
    TInt deltaMargin = deltaAbs * KBoundaryMargin;

    //Code to predict future movement if same delta movement we might get in future
    //For flick event down - If the movement goes below screen, start physics
    //or For flick event up - If the movement goes above screen, start physics 
    if (((delta < 0) && (aPointerEvent.iPosition.iY - delta >= cntrlBottomYLimit - deltaMargin))
            || ((delta > 0) && (aPointerEvent.iPosition.iY - delta <= cntrlTopYLimit + deltaMargin)))
        {
        iPhysicsStarted = ETrue;
        }

    //If found that possibly in next move it might go out of bounds, start physics
    //emulation
    if (iPhysicsStarted && iDragging)
        {
        iTimer->Cancel();
        TPoint drag = iStart - aPointerEvent.iPosition;
        iPhysics->StartPhysics(drag, iStartTime);
        iDragging = EFalse;
        }

    return consumed;
    }

// ---------------------------------------------------------------------------
// GetAbsoluteIndex()
// ---------------------------------------------------------------------------
//
TInt CGlxCloudViewControl::GetAbsoluteIndex(const TPoint& aPosition)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::GetAbsoluteIndex");
    //find out the visual on which the drag event has occured
    for(TInt j = 0; j < iLayout->Count(); j++)
        {
        CAlfVisual* visual = &(iLayout->Visual(j));
        TRect visualrect = visual->DisplayRect();
        TInt focusindex = iMediaList.FocusIndex();
        if(visualrect.Contains(aPosition) && (focusindex != j))      
            {
            return j;
            }
        }
    return KErrNotFound;	         
    }

// ---------------------------------------------------------------------------
// SetRelativeFocusL()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::SetRelativeFocusL(TInt aRowNumber)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::SetRelativeFocusL");
    TInt focus = iMediaList.FocusIndex ();
    //get the co-ordinates of the focused tag
    TInt x = iLayout->Visual(focus).DisplayRect().Center().iX;
    TInt y = iLayout->Visual(focus).DisplayRect().Center().iY;
    TPoint midpoint(x, y);

    //find the tag below\above that overlaps with the focused tag
    for(TInt index = iCloudInfo[aRowNumber].iStartIndex; 
        index <= iCloudInfo[aRowNumber].iEndIndex; index++)
        {	    
        CAlfVisual* visual = &(iLayout->Visual(index));				
        TRect visualrect = visual->DisplayRect();
        if ( (visualrect.iTl.iX <= midpoint.iX) && (midpoint.iX <= visualrect.iBr.iX))
            {            
            iMediaList.SetFocusL (NGlxListDefs::EAbsolute,index);
            break;
            } 
        }

    // if no element is found that overlaps with midpoint of the 
    //focused item,then focus the last element of the next row.
    if(focus == iMediaList.FocusIndex ())	
        {        
        iMediaList.SetFocusL (NGlxListDefs::EAbsolute,
                iCloudInfo[aRowNumber].iEndIndex);
        } 	 	
    }

// ---------------------------------------------------------------------------
// setActiveStates()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::setActiveStates( unsigned int /*aStates*/ )
    {
    }

// ---------------------------------------------------------------------------
// makeInterface()
// ---------------------------------------------------------------------------
//
IAlfInterfaceBase* CGlxCloudViewControl::makeInterface ( const IfId& /*aType*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// eventHandlerType()
// ---------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerType CGlxCloudViewControl::eventHandlerType()
    {
    return IAlfWidgetEventHandler::ELogicalEventHandler;
    }  

// ---------------------------------------------------------------------------
// eventExecutionPhase()
// ---------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerExecutionPhase CGlxCloudViewControl::eventExecutionPhase()
    {
    return IAlfWidgetEventHandler::ETunnellingPhaseEventHandler;
    }

// ---------------------------------------------------------------------------
// offerEvent()
// ---------------------------------------------------------------------------
//
AlfEventStatus CGlxCloudViewControl::offerEvent( CAlfWidgetControl& aControl, const TAlfEvent& aEvent )
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::offerEvent");  
    AlfEventStatus status = EEventNotHandled;  
    if(aEvent.IsCustomEvent() && accept(aControl, aEvent))
        {
        if ( iScrollBarWidget )
            {    		
            if (aEvent.IsCustomEvent())
                {
                switch(aEvent.CustomParameter())
                    {
                    case KAlfActionIdDeviceLayoutChanged:
                        {
                        iLayoutObserver.HandleLayoutChanged();
                        TRAP_IGNORE(UpdateLayoutL());
                        }
                         return EEventNotHandled;

                    case EEventDragVertical:
                        {    
                        GLX_LOG_INFO("GLX_CLOUD :: CGlxCloudViewControl::offerEvent(EEventScrollDown) event");
                        TInt steps = TInt(aEvent.CustomEventData());
                        UpdateScrollBar(steps,EFalse);                        
                        Scroll();                       
                        iPhysics->StopPhysics();

                        status = EEventHandled; 
                        }
                        break;                   
                    case EEventScrollPageUp:
                        { 
                        GLX_LOG_INFO("GLX_CLOUD :: CGlxCloudViewControl::offerEvent(EEventScrollPageUp) event");                       
                        UpdateScrollBar(-iScrollEventData.mViewLength); 
                        Scroll();                        
                        iPhysics->StopPhysics();
                        status = EEventHandled;
                        }
                        break;
                    case EEventScrollPageDown:
                        {  
                        GLX_LOG_INFO("GLX_CLOUD :: CGlxCloudViewControl::offerEvent(EEventScrollPageDown) event");                       
                        UpdateScrollBar(iScrollEventData.mViewLength); 
                        Scroll();
                        iPhysics->StopPhysics();
                        status = EEventHandled; 		    	    
                        }
                        break;                    
                    }//end switch
                }
            }
        }
    return status;
    }

// ---------------------------------------------------------------------------
// accept()
// ---------------------------------------------------------------------------
//   
bool CGlxCloudViewControl::accept ( CAlfWidgetControl& /*aControl*/, const TAlfEvent& aEvent ) const
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::accept");
    if(	aEvent.CustomParameter() == EEventScrollPageUp 	||
        aEvent.CustomParameter() == EEventScrollPageDown ||
        aEvent.CustomParameter() == EEventDragVertical ||
        aEvent.CustomParameter() == KAlfActionIdDeviceLayoutChanged )
        {
        return true;
        }
    return false;
    }

// ---------------------------------------------------------------------------
// attachScrollBar()
// ---------------------------------------------------------------------------
//	
void CGlxCloudViewControl::InitializeScrollBar(IAlfScrollBarWidget* aScrollBarWidget)
    { 
    TRACER("GLX_CLOUD::CGlxCloudViewControl::InitializeScrollBar");
    iScrollBarWidget=aScrollBarWidget;
    ((IAlfScrollBarModel *)(iScrollBarWidget->model()))->initializeData(iScrollEventData.mSpan,
            iScrollEventData.mViewLength,0);													           
    DisplayScrollBar();	
    }

// ---------------------------------------------------------------------------
// Scroll()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::Scroll()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::Scroll()");
    TAlfEvent customevent(EEventScrollBarModelChanged,(TInt)&iScrollEventData);
    IAlfElement* baseelement = iScrollBarWidget->control()->findElement ("BaseElement");
    iScrollbarElement=static_cast<IAlfWidgetEventHandler*> (
            baseelement->makeInterface (IAlfWidgetEventHandler::type()));
    //pass the custom event to scrollbar element so as to scroll to the required position
    iScrollbarElement->offerEvent(*(iScrollBarWidget->control()),customevent);
    GLX_LOG_INFO1("GLX_CLOUD ::CGlxCloudViewControl::Scroll(),position %d ",iScrollEventData.mViewStartPos);
    iViewPortPosition.iY = iScrollEventData.mViewStartPos;
    iViewPortLayout->SetViewportPos(iViewPortPosition, KSlowCloudMovement);
    }

// ---------------------------------------------------------------------------
// UpdateScrollBar()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::UpdateScrollBar(TInt aNumberOfSteps, TBool aDiff)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewControl::UpdateScrollBar()");
    TInt viewLength = ((IAlfScrollBarModel *)(iScrollBarWidget->model()))->getViewLength();
    TInt viewStartPos=((IAlfScrollBarModel *)(iScrollBarWidget->model()))->getViewStartPosition();
    TInt totalLength=((IAlfScrollBarModel *)(iScrollBarWidget->model()))->getTotalLength();

    TInt newViewStartPos=viewStartPos;
    if(aDiff)
        {
        aNumberOfSteps+=viewStartPos;
        }
    if( (aNumberOfSteps <= (totalLength-viewLength))&&(aNumberOfSteps>=0) )
        {
        newViewStartPos=aNumberOfSteps;
        }
    else if( aNumberOfSteps > (totalLength-viewLength) )
        {
        newViewStartPos=totalLength-viewLength;
        }
    else if ( aNumberOfSteps < 0 )
        {
        newViewStartPos=0;
        }
    iScrollEventData.mViewStartPos = newViewStartPos;
    }

// ---------------------------------------------------------------------------
// DisplayScrollBar()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::DisplayScrollBar() 
    {
    if( iScrollBarWidget )
        {
        IAlfElement* vertBaseElement =(iScrollBarWidget->control()->findElement ("BaseElement"));
        IAlfScrollBarDefaultBaseElement* scrollbarbaselement=static_cast<IAlfScrollBarDefaultBaseElement*> (
            vertBaseElement->makeInterface (IAlfScrollBarDefaultBaseElement::type() ) );

        if( iScrollEventData.mSpan )
            {       
            scrollbarbaselement->setThumbOpacity(1.0);  
            }
        else
            {
            scrollbarbaselement->setThumbOpacity(0.0);  
            }
        }
    }

// ---------------------------------------------------------------------------
// UpdateLayoutL() 
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::UpdateLayoutL() 
    {
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect (AknLayoutUtils::EMainPane, rect);
    if ((rect.Width() != (iTagScreenWidth + iScrollPaneHandle.iW + KRightmargin)) || (rect.Height() != iScreenHeight))
        {
        //set the new screen dimensions
        iScreenHeight=rect.Height();
       iTagScreenWidth = rect.Width()- iScrollPaneHandle.iW - KRightmargin;
        if(IsLandscape())
            {   
            iTagScreenHeight = rect.Height();   
            }
        else 
            {
            iTagScreenHeight = KTagScreenHeight;
            }

        iViewPortLayout->SetSize(TAlfRealSize(iTagScreenWidth,iTagScreenHeight), 0);
        //delete all layout associations
        if ( iCloudInfo.Count ()!= 0)//check for the empty cloud view
            {
            UpdateLayout();
            FetchAttributeFromCacheL();
            //generate row structures and draw rows on screen
            UpdateRowDataL ();                                    

            InitPhysicsL();
            }
        }
    }

// ---------------------------------------------------------------------------
// InitPhysicsL() 
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::InitPhysicsL()
    {
    //Update physics with new total layout, viewable size and our landscape mode 
    iPhysics->InitPhysicsL(TSize(iViewPortVirtualSize.iWidth,
            iViewPortVirtualSize.iHeight), TSize(iTagScreenWidth,
            iTagScreenHeight), EFalse);

    //Note: Physics viewposition must always be mid of viewable screen size to get best result
    //I learned the hard way
    iViewPosition.SetXY(iTagScreenWidth / 2, iTagScreenHeight / 2);
    }

// ---------------------------------------------------------------------------
// ViewPositionChanged() 
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::ViewPositionChanged(const TPoint& aNewPosition,
        TBool aDrawNow, TUint /*aFlags*/)
    {
    iViewPosition = aNewPosition;
    iViewPortPosition.iY = iViewPosition.iY - iTagScreenHeight / 2;
    if (aDrawNow)
        {
        iViewPortLayout->SetViewportPos(iViewPortPosition, KFastCloudMovement);
        
        iScrollEventData.mViewStartPos = iViewPortPosition.iY;
        Scroll();
        }
    }

// ---------------------------------------------------------------------------
// PhysicEmulationEnded() 
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::PhysicEmulationEnded()
    {
    iPhysicsStarted = EFalse;
    }

// ---------------------------------------------------------------------------
// ViewPosition() 
// ---------------------------------------------------------------------------
//
TPoint CGlxCloudViewControl::ViewPosition() const
    {
    return iViewPosition;
    }

// ---------------------------------------------------------------------------
// IsLandscape() 
// ---------------------------------------------------------------------------
//
TBool CGlxCloudViewControl::IsLandscape()
    {
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, rect);
    if(rect.Width() > rect.Height())
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
// ---------------------------------------------------------------------------
// TimerComplete()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::TimerComplete()
    {
    iTimerComplete = ETrue;
    iTagsContextMenuControl->ShowItemMenu(EFalse);

    CAlfVisual& visual = iLayout->Visual( iMediaList.FocusIndex() );
    TRect focussedItemRect = visual.DisplayRect();
    TPoint midpoint;
    midpoint.iX=focussedItemRect.iTl.iX +
            ((focussedItemRect.iBr.iX - focussedItemRect.iTl.iX )/2);
    midpoint.iY=focussedItemRect.iTl.iY+
                    ((focussedItemRect.iBr.iY - focussedItemRect.iTl.iY )/2); 
    
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    
    if(!iPhysicsStarted)//only if physics hasnt started
        {
        iTagsContextMenuControl->SetViewableRect(rect);
        iTagsContextMenuControl->ShowItemMenu(ETrue);
        iTagsContextMenuControl->SetDisplay(midpoint);
        }
   }
// ---------------------------------------------------------------------------
// ShowContextItemMenu()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewControl::ShowContextItemMenu(TBool aShow)
    {
    iTagsContextMenuControl->ShowItemMenu(aShow);
    }

//End of file
