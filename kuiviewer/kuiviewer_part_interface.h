/*
 *  This file is part of the kuiviewer package
 *  Copyright (c) 2017 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
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
