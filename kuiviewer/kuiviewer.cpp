/*
 * kuiviewer.cpp
 *
 * Copyright (C) 2003  <rich@kde.org>
 * Copyright (C) 2003  <geiseri@kde.org>
 */
#include "kuiviewer.h"
#include "kuiviewer.moc"
#include "kuiviewer_part.h"

#include <kdebug.h>

#include <qobjectlist.h>
#include <qdockwindow.h>
#include <qpixmap.h>

#include <kkeydialog.h>
#include <kconfig.h>
#include <kurl.h>

#include <kedittoolbar.h>

#include <kaction.h>
#include <kstdaction.h>

#include <kiconloader.h>
#include <klibloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kstatusbar.h>

KUIViewer::KUIViewer()
    : KParts::MainWindow( 0L, "KUIViewer" )
{
    // set the shell's ui resource file
    setXMLFile("kuiviewer.rc");

    // then, setup our actions
    setupActions();

    // and a status bar
    statusBar()->show();


    // this routine will find and load our Part.  it finds the Part by
    // name which is a bad idea usually.. but it's alright in this
    // case since our Part is made for this Shell
    KLibFactory *factory = KLibLoader::self()->factory("libkuiviewerpart");
    if (factory)
    {
        // now that the Part is loaded, we cast it to a Part to get
        // our hands on it
        m_part = static_cast<KParts::ReadOnlyPart *>(factory->create(this,
                                "kuiviewer_part", "KParts::ReadOnlyPart" ));

        if (m_part)
        {
            // tell the KParts::MainWindow that this is indeed the main widget
            setCentralWidget(m_part->widget());

            // and integrate the part's GUI with the shell's
            createGUI(m_part);
        }
    }
    else
    {
        // if we couldn't find our Part, we exit since the Shell by
        // itself can't do anything useful
	//FIXME improve message, which Part is this referring to?
        KMessageBox::error(this, i18n("Unable to locate Part"));
        kapp->quit();
        // we return here, cause kapp->quit() only means "exit the
        // next time we enter the event loop...
        return;
    }

    // apply the saved mainwindow settings, if any, and ask the mainwindow
    // to automatically save settings if changed: window size, toolbar
    // position, icon size, etc.
    setAutoSaveSettings();
}

KUIViewer::~KUIViewer()
{
}

void KUIViewer::load(const KURL& url)
{
	m_part->openURL( url );
}

void KUIViewer::setupActions()
{
    KStdAction::open(this, SLOT(fileOpen()), actionCollection());

    KStdAction::quit(kapp, SLOT(quit()), actionCollection());

    m_toolbarAction = KStdAction::showToolbar(this, SLOT(optionsShowToolbar()), actionCollection());
    m_statusbarAction = KStdAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
}

void KUIViewer::saveProperties(KConfig* /*config*/)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored
}

void KUIViewer::readProperties(KConfig* /*config*/)
{
    // the 'config' object points to the session managed
    // config file.  this function is automatically called whenever
    // the app is being restored.  read in here whatever you wrote
    // in 'saveProperties'
}

void KUIViewer::optionsShowToolbar()
{
    // this is all very cut and paste code for showing/hiding the
    // toolbar
    if (m_toolbarAction->isChecked())
        toolBar()->show();
    else
        toolBar()->hide();
}

void KUIViewer::optionsShowStatusbar()
{
    // this is all very cut and paste code for showing/hiding the
    // statusbar
    if (m_statusbarAction->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}

void KUIViewer::optionsConfigureKeys()
{
    KKeyDialog::configure(actionCollection(), 0, true);
}

void KUIViewer::optionsConfigureToolbars()
{
    saveMainWindowSettings(KGlobal::config(), autoSaveGroup());

    // use the standard toolbar editor
    KEditToolbar dlg(factory());
    connect(&dlg, SIGNAL(newToolbarConfig()),
            this, SLOT(applyNewToolbarConfig()));
    dlg.exec();
}

void KUIViewer::applyNewToolbarConfig()
{
    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
}

void KUIViewer::fileOpen()
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked
    KURL file_name =
        KFileDialog::getOpenURL( QString::null, i18n("*.ui *.UI|User Interface Files"), this );

    if (file_name.isEmpty() == false)
    {
        // About this function, the style guide (
        // http://developer.kde.org/documentation/standards/kde/style/basics/index.html )
        // says that it should open a new window if the document is _not_
        // in its initial state.  This is what we do here..
        if ( m_part->url().isEmpty() )
        {
            // we open the file in this window...
            load( file_name );
        }
        else
        {
            // we open the file in a new window...
            KUIViewer* newWin = new KUIViewer;
            newWin->load( file_name  );
            newWin->show();
        }
    }
}



