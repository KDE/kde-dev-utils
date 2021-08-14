/*
    SPDX-FileCopyrightText: 2003 Richard Moore <rich@kde.org>
    SPDX-FileCopyrightText: 2003 Ian Reinhart Geiser <geiseri@kde.org>
    SPDX-FileCopyrightText: 2004 Benjamin C. Meyer <ben+kuiviewer@meyerhome.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef UICREATOR_H
#define UICREATOR_H

#include <KIO/ThumbCreator>

class QUICreator : public ThumbCreator
{
public:
    QUICreator() {}

    bool create(const QString& path, int, int, QImage& img) override;
};

#endif // UICREATOR_H

