/*
 *
 *  This file is part of the kuiviewer package
 *  Copyright (c) 2003 Richard Moore <rich@kde.org>
 *  Copyright (c) 2003 Ian Reinhart Geiser <geiseri@kde.org>
 *  Copyright (c) 2004 Benjamin C. Meyer <ben+kuiviewer@meyerhome.net>
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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#ifndef KUIVIEWER_H
#define KUIVIEWER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <kparts/mainwindow.h>

class KToggleAction;
class KListView;

namespace KParts {
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
    virtual ~KUIViewer();

    /**
     * Use this method to load whatever file/URL you have
     */
    void load(const KURL& url);

    /**
     * Take screenshot of current ui file
     * @param filename to save image in
     * @param h height of image
     * @param w width of image
     */
    void takeScreenshot(const QCString &filename, int h=-1, int w=-1);

protected:
    /**
     * This method is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties(KConfig *);

    /**
     * This method is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties(KConfig *);

private slots:
    void fileOpen();

private:
    void setupActions();

private:
    KParts::ReadOnlyPart *m_part;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
};

#endif // KUIVIEWER_H

