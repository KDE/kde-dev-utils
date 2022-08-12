/*
    SPDX-FileCopyrightText: 2003 Richard Moore <rich@kde.org>
    SPDX-FileCopyrightText: 2003 Ian Reinhart Geiser <geiseri@kde.org>
    SPDX-FileCopyrightText: 2004 Benjamin C. Meyer <ben+kuiviewer@meyerhome.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "quicreator.h"

// Qt
#include <QPixmap>
#include <QImage>
#include <QFormBuilder>
#include <QWidget>
#include <QCoreApplication>

extern "C"
{

Q_DECL_EXPORT ThumbCreator* new_creator()
{
    return new QUICreator;
}

}

class KIOPluginForMetaData : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "KIOPluginForMetaData" FILE "designerthumbnail.json")
};


bool QUICreator::create(const QString& path, int width, int height, QImage& img)
{
    QStringList designerPluginPaths;
    const QStringList& libraryPaths = QCoreApplication::libraryPaths();
    for (const auto& path : libraryPaths) {
        designerPluginPaths.append(path + QLatin1String("/designer"));
    }
    QFormBuilder builder;
    builder.setPluginPath(designerPluginPaths);

    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }

    QWidget* w = builder.load(&file);
    file.close();

    if (!w) {
        return false;
    }

    const QPixmap p = w->grab();
    img = p.toImage().scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return true;
}

#include "quicreator.moc"
