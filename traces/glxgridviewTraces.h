// Created by TraceCompiler 2.1.2
// DO NOT EDIT, CHANGES WILL BE LOST

#ifndef __GLXGRIDVIEWTRACES_H__
#define __GLXGRIDVIEWTRACES_H__

#define KOstTraceComponentID 0x200009ef

#define GLXGRIDVIEW_GLXGRIDVIEW_ENTRY 0x8a0001
#define GLXGRIDVIEW_GLXGRIDVIEW_EXIT 0x8a0002
#define GLXGRIDVIEW_ACTIVATE_ENTRY 0x8a0003
#define GLXGRIDVIEW_ACTIVATE_EXIT 0x8a0004
#define GLXGRIDVIEW_DEACTIVATE_ENTRY 0x8a0005
#define GLXGRIDVIEW_DEACTIVATE_EXIT 0x8a0006
#define GLXGRIDVIEW_SETMODEL_ENTRY 0x8a0007
#define GLXGRIDVIEW_SETMODEL_EXIT 0x8a0008
#define GLXGRIDVIEW_ADDTOOLBAR_ENTRY 0x8a0009
#define GLXGRIDVIEW_ADDTOOLBAR_EXIT 0x8a000a
#define GLXGRIDVIEW_GETANIMATIONITEM_ENTRY 0x8a000b
#define GLXGRIDVIEW_GETANIMATIONITEM_EXIT 0x8a000c
#define GLXGRIDVIEW_LOADGRIDVIEW_ENTRY 0x8a000d
#define GLXGRIDVIEW_LOADGRIDVIEW_EXIT 0x8a000e
#define DUP1_GLXGRIDVIEW_GLXGRIDVIEW_ENTRY 0x8a000f
#define DUP1_GLXGRIDVIEW_GLXGRIDVIEW_EXIT 0x8a0010
#define GLXGRIDVIEW_ENABLEMARKING 0x860001
#define GLXGRIDVIEW_DISABLEMARKING 0x860002
#define GLXGRIDVIEW_HANDLEUSERACTION 0x860003
#define DUP1_GLXGRIDVIEW_HANDLEUSERACTION 0x860004
#define DUP2_GLXGRIDVIEW_HANDLEUSERACTION 0x860005
#define GLXGRIDVIEW_GETSELECTIONMODEL 0x860006
#define GLXGRIDVIEW_ITEMDESTROYED 0x860007
#define GLXGRIDVIEW_ITEMCHANGE 0x860008
#define GLXGRIDVIEW_ADDVIEWCONNECTION 0x860009
#define GLXGRIDVIEW_REMOVEVIEWCONNECTION 0x86000a
#define GLXGRIDVIEW_ITEMSELECTED 0x86000b
#define GLXGRIDVIEW_INDICATELONGPRESS 0x86000c
#define EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_START 0x8b0001
#define EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_STOP 0x8b0002
#define GLXGRIDVIEW_SETVISVALWINDOWINDEX 0x850001
#define DUP1_GLXGRIDVIEW_SETVISVALWINDOWINDEX 0x850002
#define DUP2_GLXGRIDVIEW_SETVISVALWINDOWINDEX 0x850003
#define DUP3_GLXGRIDVIEW_SETVISVALWINDOWINDEX 0x850004
#define DUP1_GLXGRIDVIEW_SCROLLPOSITIONCHANGE 0x850005
#define GLXGRIDVIEW_SCROLLPOSITIONCHANGE 0x850006
#define DUP2_GLXGRIDVIEW_SCROLLPOSITIONCHANGE 0x850007
#define DUP3_GLXGRIDVIEW_SCROLLPOSITIONCHANGE 0x850008


inline TBool OstTraceGen2( TUint32 aTraceID, TInt aParam1, TUint aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TUint8 data[ 8 ];
        TUint8* ptr = data;
        *( ( TInt* )ptr ) = aParam1;
        ptr += sizeof ( TInt );
        *( ( TUint* )ptr ) = aParam2;
        ptr += sizeof ( TUint );
        ptr -= 8;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
        }
    return retval;
    }

inline TBool OstTraceGen2( TUint32 aTraceID, TInt32 aParam1, TUint32 aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TUint8 data[ 8 ];
        TUint8* ptr = data;
        *( ( TInt* )ptr ) = aParam1;
        ptr += sizeof ( TInt );
        *( ( TUint* )ptr ) = aParam2;
        ptr += sizeof ( TUint );
        ptr -= 8;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
        }
    return retval;
    }


inline TBool OstTraceGen2( TUint32 aTraceID, TReal aParam1, TReal aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TUint8 data[ 16 ];
        TUint8* ptr = data;
        *( ( TReal* )ptr ) = aParam1;
        ptr += sizeof ( TReal );
        *( ( TReal* )ptr ) = aParam2;
        ptr += sizeof ( TReal );
        ptr -= 16;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 16 );
        }
    return retval;
    }


inline TBool OstTraceGen2( TUint32 aTraceID, TInt aParam1, TInt aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TUint8 data[ 8 ];
        TUint8* ptr = data;
        *( ( TInt* )ptr ) = aParam1;
        ptr += sizeof ( TInt );
        *( ( TInt* )ptr ) = aParam2;
        ptr += sizeof ( TInt );
        ptr -= 8;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
        }
    return retval;
    }

inline TBool OstTraceGen2( TUint32 aTraceID, TInt32 aParam1, TInt32 aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TUint8 data[ 8 ];
        TUint8* ptr = data;
        *( ( TInt* )ptr ) = aParam1;
        ptr += sizeof ( TInt );
        *( ( TInt* )ptr ) = aParam2;
        ptr += sizeof ( TInt );
        ptr -= 8;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
        }
    return retval;
    }


#endif

// End of file

