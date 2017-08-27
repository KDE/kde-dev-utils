/**
 *
 *  This file is part of the kuiviewer package
 *  Copyright (c) 2003 Richard Moore <rich@kde.org>
 *  Copyright (c) 2003 Ian Reinhart Geiser <geiseri@kde.org>
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
 **/

#ifndef KUIVIEWERPART_H
#define KUIVIEWERPART_H

// KF
#include <KParts/ReadOnlyPart>
// Qt
#include <QPointer>

class KSelectAction;

/**
 * This is a "Part".  It that does all the real work in a KPart
 * application.
 *
 * @short Main Part
 * @author Richard Moore <rich@kde.org>
 * @version 0.1
 */
class KUIViewerPart : public KParts::ReadOnlyPart
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    KUIViewerPart(QWidget *parentWidget, QObject *parent, const QVariantList &args);

    /**
     * Destructor
     */
    ~KUIViewerPart() override;

public Q_SLOTS:
     void slotStyle(int);
     void slotGrab();
     void updateActions();

protected:
    /**
     * This must be implemented by each part
     */
    bool openFile() override;

private:
    QWidget *m_widget;
    QPointer<QWidget> m_view;
    KSelectAction *m_style;
    QAction *m_copy;
    QString m_styleFromConfig;
};

#endif // KUIVIEWERPART_H

