/*
    SPDX-FileCopyrightText: 2003 Richard Moore <rich@kde.org>
    SPDX-FileCopyrightText: 2003 Ian Reinhart Geiser <geiseri@kde.org>
    SPDX-FileCopyrightText: 2004 Benjamin C. Meyer <ben+kuiviewer@meyerhome.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef UICREATOR_H
#define UICREATOR_H

#include <KIO/ThumbnailCreator>

class QUICreator : public KIO::ThumbnailCreator
{
public:
    QUICreator(QObject *parent, const QVariantList &args)
    : KIO::ThumbnailCreator(parent, args)
    {}

    KIO::ThumbnailResult create( const KIO::ThumbnailRequest &request ) override;
};

#endif // UICREATOR_H

