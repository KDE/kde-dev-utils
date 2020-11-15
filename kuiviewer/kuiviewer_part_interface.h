/*
    SPDX-FileCopyrightText: 2017 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KUIVIEWERPARTINTERFACE_H
#define KUIVIEWERPARTINTERFACE_H

#include <QObject>

class QPixmap;
class QSize;


class KUIViewerPartInterface
{
public:
    virtual ~KUIViewerPartInterface() {}

    virtual void setWidgetSize(const QSize& size) = 0;
    virtual QPixmap renderWidgetAsPixmap() const = 0;
};

Q_DECLARE_INTERFACE(KUIViewerPartInterface, "org.kde.KUIViewerPartInterface")

#endif // KUIVIEWERPARTINTERFACE_H
