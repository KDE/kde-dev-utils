/*
    SPDX-FileCopyrightText: 2003 Richard Moore <rich@kde.org>
    SPDX-FileCopyrightText: 2003 Ian Reinhart Geiser <geiseri@kde.org>
    SPDX-FileCopyrightText: 2004 Benjamin C. Meyer <ben+kuiviewer@meyerhome.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "quicreator.h"

// KF
#include <KPluginFactory>
// Qt
#include <QPixmap>
#include <QImage>
#include <QFormBuilder>
#include <QWidget>
#include <QCoreApplication>
#include <QDebug>

K_PLUGIN_CLASS_WITH_JSON(QUICreator, "designerthumbnail.json")

KIO::ThumbnailResult QUICreator::create( const KIO::ThumbnailRequest &request )
{
    QStringList designerPluginPaths;
    const QStringList& libraryPaths = QCoreApplication::libraryPaths();
    for (const auto& path : libraryPaths) {
        designerPluginPaths.append(path + QLatin1String("/designer"));
    }
    QFormBuilder builder;
    builder.setPluginPath(designerPluginPaths);

    QFile file(request.url().toLocalFile());
    if (!file.open(QFile::ReadOnly)) {
        return KIO::ThumbnailResult::fail();
    }

    QWidget* w = builder.load(&file);
    file.close();

    if (!w) {
        return KIO::ThumbnailResult::fail();
    }

    const QPixmap p = w->grab();
    const QImage img = p.toImage().scaled(request.targetSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return KIO::ThumbnailResult::pass(img);
}

#include "quicreator.moc"
