/*
    SPDX-FileCopyrightText: 2016 Pino Toscano <pino@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef HTMLTEXTDUMP_H
#define HTMLTEXTDUMP_H

#include <QByteArray>

bool htmlTextDump( const QByteArray& data, QByteArray *title, QByteArray *text );

#endif
