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
* Description:  AIW Service Handler stub implementation.
*
*/



#ifndef _T_CAIWSERVICEHANDLERSTUB_H_
#define _T_CAIWSERVICEHANDLERSTUB_H_



NONSHARABLE_CLASS(CAiwServiceHandlerImpl) : public CBase
    {
public:
    static CAiwServiceHandlerImpl* NewL();
    ~CAiwServiceHandlerImpl();

    CAiwGenericParamList& InParamListL();
    CAiwGenericParamList& OutParamListL();
    void ResetInParams();
    
private:
    CAiwServiceHandlerImpl();
    void ConstructL();
    
private:
    CAiwGenericParamList*   iInParams;
    CAiwGenericParamList*   iOutParams;
    };
    
#endif
