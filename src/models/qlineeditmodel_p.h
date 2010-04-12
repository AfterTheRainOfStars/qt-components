/****************************************************************************
**
** Copyright (C) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Components project on Trolltech Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QLINEEDITMODEL_P_H
#define QLINEEDITMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt Components API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qlineeditmodel.h"

QT_BEGIN_NAMESPACE

class QLineEditModel;

class QLineEditModelPrivate
{
    Q_DECLARE_PUBLIC(QLineEditModel)

public:
    QLineEditModelPrivate(QLineEditModel *qq);
    virtual ~QLineEditModelPrivate();

    QLineEditModel *q_ptr;

    QString text;
    int cursor;
    Qt::LayoutDirection layoutDirection;
    uint separator : 1;
    uint dragEnabled : 1;
    uint echoMode : 2;
    uint textDirty : 1;
    uint selDirty : 1;
    uint validInput : 1;
    int deleteAllTimer;
    int ascent;
    int maxLength;
    int lastCursorPos;
    QList<int> transactions;

#ifndef QT_NO_VALIDATOR
    QPointer<QValidator> validator;
#endif

    struct MaskInputData {
        enum Casemode { NoCaseMode, Upper, Lower };
        QChar maskChar; // either the separator char or the inputmask
        bool separator;
        Casemode caseMode;
    };
    QString inputMask;
    QChar blank;
    MaskInputData *maskData;

    // undo/redo handling
    enum CommandType { Separator, Insert, Remove, Delete, RemoveSelection, DeleteSelection, SetSelection };
    struct Command {
        inline Command() {}
        inline Command(CommandType t, int p, QChar c, int ss, int se) : type(t),uc(c),pos(p),selStart(ss),selEnd(se) {}
        uint type : 4;
        QChar uc;
        int pos, selStart, selEnd;
    };
    int modifiedState;
    int undoState;
    QVector<Command> history;

    // selection
    int selstart;
    int selend;

    // complex text layout
    QTextLayout textLayout;

    QChar passwordCharacter;


    void init(const QString &txt);
    void removeSelectedText();
    void internalSetText(const QString &txt, int pos = -1, bool edited = true);
    void updateDisplayText();

    void internalInsert(const QString &s);
    void internalDelete(bool wasBackspace = false);
    void internalRemove(int pos);

    void internalDeselect();

    void internalUndo(int until = -1);
    void internalRedo();

    void emitCursorPositionChanged();

    bool finishChange(int validateFromState = -1, bool update = false, bool edited = true);


    void addCommand(const Command& cmd);

    inline void separate() { separator = true; }

    // masking
    void parseInputMask(const QString &maskFields);
    bool isValidInput(QChar key, QChar mask) const;
    bool hasAcceptableInput(const QString &text) const;
    QString maskString(uint pos, const QString &str, bool clear = false) const;
    QString clearString(uint pos, uint len) const;
    QString stripString(const QString &str) const;
    int findInMask(int pos, bool forward, bool findSeparator, QChar searchChar = QChar()) const;

    // Private slots
    void _q_clipboardChanged();
    void _q_deleteSelected();

};

inline void QLineEditModelPrivate::internalDeselect()
{
    selDirty |= (selend > selstart);
    selstart = selend = 0;
}

#endif
