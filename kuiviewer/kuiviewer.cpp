/*
    SPDX-FileCopyrightText: 2003 Richard Moore <rich@kde.org>
    SPDX-FileCopyrightText: 2003 Ian Reinhart Geiser <geiseri@kde.org>
    SPDX-FileCopyrightText: 2004 Benjamin C. Meyer <ben+kuiviewer@meyerhome.net>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kuiviewer.h"
#include "kuiviewer_part.h"
#include "kuiviewer_part_interface.h"

// KF
#include <KActionCollection>
#include <KStandardAction>
#include <KPluginFactory>
#include <KLocalizedString>
#include <KMessageBox>

// Qt
#include <QObject>
#include <QPixmap>
#include <QUrl>
#include <QApplication>
#include <QAction>
#include <QFileDialog>


KUIViewer::KUIViewer()
    : KParts::MainWindow(),
      m_part(nullptr)
{
    setObjectName(QStringLiteral("KUIViewer"));

    // setup our actions
    setupActions();

    setMinimumSize(300, 200);

    // Bring up the gui
    setupGUI();

    // this routine will find and load our Part.  it finds the Part by
    // name which is a bad idea usually.. but it's alright in this
    // case since our Part is made for this Shell

    const auto partLoadResult = KPluginFactory::instantiatePlugin<KParts::ReadOnlyPart>(KPluginMetaData(QStringLiteral("kf5/parts/kuiviewerpart")), this);

    if (partLoadResult)
    {
        m_part = partLoadResult.plugin;

        m_part->setObjectName(QStringLiteral("kuiviewer_part"));
        // tell the KParts::MainWindow that this is indeed the main widget
        setCentralWidget(m_part->widget());

        // and integrate the part's GUI with the shell's
        createGUI(m_part);
    } else {
        // if we couldn't find our Part, we exit since the Shell by
        // itself can't do anything useful
        //FIXME improve message, which Part is this referring to?
        KMessageBox::error(this, i18n("Unable to locate or load KUiViewer KPart."));
        QApplication::quit();
        // we return here, cause kapp->quit() only means "exit the
        // next time we enter the event loop...
        return;
    }
}

KUIViewer::~KUIViewer()
{
}

void KUIViewer::load(const QUrl& url)
{
    m_part->openUrl(url);
    adjustSize();
}

void KUIViewer::setupActions()
{
    KStandardAction::open(this, &KUIViewer::fileOpen, actionCollection());
    KStandardAction::quit(this, &KUIViewer::close, actionCollection());
}

void KUIViewer::fileOpen()
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked
    QUrl file_name =
        QFileDialog::getOpenFileUrl(this, QString(), QUrl(), i18n("*.ui *.UI|User Interface Files"));

    if (!file_name.isEmpty()) {
        // About this function, the style guide
        // says that it should open a new window if the document is _not_
        // in its initial state.  This is what we do here..
        if (m_part->url().isEmpty()) {
            // we open the file in this window...
            load(file_name);
        } else {
            // we open the file in a new window...
            KUIViewer* newWin = new KUIViewer;
            newWin->load(file_name);
            newWin->show();
        }
    }
}

void KUIViewer::takeScreenshot(const QString& filename, int w, int h)
{
    auto uiviewerInterface = qobject_cast<KUIViewerPartInterface*>(m_part);
    if (!uiviewerInterface) {
        return;
    }

    if (w != -1 && h != -1) {
        // resize widget to the desired size
        uiviewerInterface->setWidgetSize(QSize(w, h));
    }

    const QPixmap pixmap = uiviewerInterface->renderWidgetAsPixmap();
    pixmap.save(filename, "PNG");
}
