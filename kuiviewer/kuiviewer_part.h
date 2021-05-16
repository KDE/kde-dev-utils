/*
    SPDX-FileCopyrightText: 2003 Richard Moore <rich@kde.org>
    SPDX-FileCopyrightText: 2003 Ian Reinhart Geiser <geiseri@kde.org>
    SPDX-FileCopyrightText: 2017 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KUIVIEWERPART_H
#define KUIVIEWERPART_H

#include "kuiviewer_part_interface.h"

// KF
#include <kparts_version.h>
#include <KParts/ReadOnlyPart>
// Qt
#include <QPointer>
#include <QByteArray>
#include <QPoint>
#include <QSize>

class KSelectAction;
class KPluginMetaData;
class QIODevice;
class QMdiArea;
class QMdiSubWindow;

/**
 * This is a "Part".  It that does all the real work in a KPart
 * application.
 *
 * @short Main Part
 * @author Richard Moore <rich@kde.org>
 * @version 0.1
 */
class KUIViewerPart : public KParts::ReadOnlyPart, public KUIViewerPartInterface
{
    Q_OBJECT
    Q_INTERFACES(KUIViewerPartInterface)

public:
    /**
     * Default constructor
     */
    KUIViewerPart(QWidget* parentWidget, QObject* parent, const KPluginMetaData& metaData, const QVariantList& args);

    /**
     * Destructor
     */
    ~KUIViewerPart() override;

public Q_SLOTS:
    void slotStyle(int);
    void slotGrab();
    void updateActions();

public:
    void setWidgetSize(const QSize& size) override;
    QPixmap renderWidgetAsPixmap() const override;

protected:
    bool openFile() override;

    bool doOpenStream(const QString& mimeType) override;
    bool doWriteStream(const QByteArray& data) override;
    bool doCloseStream() override;

    bool closeUrl() override;

private:
    bool loadUiFile(QIODevice* device);
    void restyleView(const QString& styleName);

private:
    QMdiArea* m_widget;
    QMdiSubWindow* m_subWindow;
    QPointer<QWidget> m_view;
    KSelectAction* m_style;
    QAction* m_copy;
    QString m_styleFromConfig;

    QByteArray m_streamedData;

    QUrl m_previousUrl;
    QPoint m_previousScrollPosition;
    QSize m_previousSize;
};

#endif // KUIVIEWERPART_H

