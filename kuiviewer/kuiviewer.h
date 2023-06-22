/*
    SPDX-FileCopyrightText: 2003 Richard Moore <rich@kde.org>
    SPDX-FileCopyrightText: 2003 Ian Reinhart Geiser <geiseri@kde.org>
    SPDX-FileCopyrightText: 2004 Benjamin C. Meyer <ben+kuiviewer@meyerhome.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KUIVIEWER_H
#define KUIVIEWER_H

// KF
#include <KParts/MainWindow>

namespace KParts
{
class ReadOnlyPart;
}

/**
 * This is the application "Shell".  It has a menubar, toolbar, and
 * statusbar but relies on the "Part" to do all the real work.
 *
 * @short KUI Viewer Shell
 * @author Richard Moore <rich@kde.org>
 * @author Ian Reinhart Geiser <geiser@kde.org>
 * @version 1.0
 */
class KUIViewer : public KParts::MainWindow
{
    Q_OBJECT

public:
    /**
     * Default Constructor
     */
    KUIViewer();

    /**
     * Default Destructor
     */
    ~KUIViewer() override;

    /**
     * Use this method to load whatever file/URL you have
     */
    void load(const QUrl& url);

    /**
     * Check whether the viewer KPart is correctly found and loaded
     */
    bool isReady() const 				{ return (m_part!=nullptr); }

    /**
     * Take screenshot of current ui file
     * @param filename to save image in
     * @param h height of image
     * @param w width of image
     */
    void takeScreenshot(const QString& filename, int h = -1, int w = -1);

private Q_SLOTS:
    void fileOpen();

private:
    void setupActions();

private:
    KParts::ReadOnlyPart* m_part;
};

#endif // KUIVIEWER_H

