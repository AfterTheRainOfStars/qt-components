/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ssharedstatusbarsubscriber.h"

#include <fbs.h>

#ifdef Q_DEBUG_SHARED_STATUSBAR
#include <QDebug>
#endif // Q_DEBUG_SHARED_STATUSBAR

CSharedBitmapSubcriber* CSharedBitmapSubcriber::NewL( MSharedBitmapSubcriberObserver& aObserver )
    {
    CSharedBitmapSubcriber* self = new ( ELeave ) CSharedBitmapSubcriber( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CSharedBitmapSubcriber::CSharedBitmapSubcriber( MSharedBitmapSubcriberObserver& aObserver )
: CActive( EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

CSharedBitmapSubcriber::~CSharedBitmapSubcriber()
    {
    Cancel();
    iProperty.Close();
    }

void CSharedBitmapSubcriber::ConstructL()
    {
#ifdef Q_DEBUG_SHARED_STATUSBAR
    qDebug() << "CSharedBitmapSubcriber::ConstructL() start";
#endif
    User::LeaveIfError( iProperty.Attach( KOffScreenSPaneUid, KOffScreenHandle ) );
    Subscribe();
#ifdef Q_DEBUG_SHARED_STATUSBAR
    qDebug() << "CSharedBitmapSubcriber::ConstructL() end";
#endif
    }

CFbsBitmap* CSharedBitmapSubcriber::CreateSharedBitmapL( TBool *aIsMirrored )
    {
#ifdef Q_DEBUG_SHARED_STATUSBAR
    qDebug() << "CSharedBitmapSubcriber::CreateSharedBitmapL() start";
#endif
    TInt handle(0);
    User::LeaveIfError( iProperty.Get( handle ) );

    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Duplicate( handle ) );

    if ( aIsMirrored )
        {
        TInt layoutInfo(0);
        if ( RProperty::Get( KOffScreenSPaneUid, KOffScreenLayoutInfo, layoutInfo) == KErrNone )
            {
            *aIsMirrored = layoutInfo&ELayoutInfoMirroredFlag;
            }
        else
            {
            // if the key is not set, use the non-mirrored version
            }
            *aIsMirrored = false;
            }

    CleanupStack::Pop( bitmap );
#ifdef Q_DEBUG_SHARED_STATUSBAR
    qDebug() << "CSharedBitmapSubcriber::CreateSharedBitmapL() end";
#endif
    return bitmap;
    }

void CSharedBitmapSubcriber::Subscribe()
    {
    iProperty.Subscribe(iStatus);
    SetActive();
    }

void CSharedBitmapSubcriber::RunL()
    {
#ifdef Q_DEBUG_SHARED_STATUSBAR
    qDebug() << "CSharedBitmapSubcriber::RunL() error: " << (int)iStatus.Int();
#endif
    if ( iStatus == KErrNone )
        {
        Subscribe();
        iObserver.SharedBitmapChanged();
        }
    }

void CSharedBitmapSubcriber::DoCancel()
    {
    iProperty.Cancel();
    }