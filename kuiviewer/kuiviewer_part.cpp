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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "kuiviewer_part.h"
#include "kuiviewer_part.moc"

#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kio/netaccess.h>
#include <klistview.h>
#include <kparts/genericfactory.h>
#include <kstdaction.h>
#include <kstyle.h>
#include <qmetaobject.h>

#include <qclipboard.h>
#include <qcursor.h>
#include <qfile.h>
#include <qobjectlist.h>
#include <qpixmap.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qvariant.h>
#include <qvbox.h>
#include <qvariant.h>
#include <qwidgetfactory.h>

typedef KParts::GenericFactory<KUIViewerPart> KUIViewerPartFactory;
K_EXPORT_COMPONENT_FACTORY( libkuiviewerpart, KUIViewerPartFactory )

KUIViewerPart::KUIViewerPart( QWidget *parentWidget, const char *widgetName,
                                  QObject *parent, const char *name,
                                  const QStringList & /*args*/ )
    : KParts::ReadOnlyPart(parent, name)
{
    // we need an instance
    setInstance( KUIViewerPartFactory::instance() );

    KGlobal::locale()->insertCatalogue("kuiviewer");

    // this should be your custom internal widget
    m_widget = new QVBox( parentWidget, widgetName );

    // notify the part that this is our internal widget
    setWidget(m_widget);

    // set our XML-UI resource file
    setXMLFile("kuiviewer_part.rc");

    m_style = new KListAction( i18n("Style"),
                CTRL + Key_S,
                this,
                SLOT(slotStyle(int)),
                actionCollection(),
                "change_style");
    m_style->setEditable(false);

    kapp->config()->setGroup("General");
    const QString currentStyle = kapp->config()->readEntry("currentWidgetStyle", KStyle::defaultStyle());

    const QStringList styles = QStyleFactory::keys();
    m_style->setItems(styles);
    m_style->setCurrentItem(0);

    QStringList::ConstIterator it = styles.begin();
    QStringList::ConstIterator end = styles.end();
    int idx = 0;
    for (; it != end; ++it, ++idx) {
        if ((*it).lower() == currentStyle.lower()) {
            m_style->setCurrentItem(idx);
            break;
        }
    }
    m_style->setToolTip(i18n("Set the current style to view."));
    m_style->setMenuAccelsEnabled(true);

    m_copy = KStdAction::copy(this, SLOT(slotGrab()), actionCollection());

    updateActions();

// Commented out to fix warning (rich)
// slot should probably be called saveAs() for consistency with
// KParts::ReadWritePart BTW.
//    KStdAction::saveAs(this, SLOT(slotSave()), actionCollection());
}

KUIViewerPart::~KUIViewerPart()
{
}

KAboutData *KUIViewerPart::createAboutData()
{
    // the non-i18n name here must be the same as the directory in
    // which the part's rc file is installed ('partrcdir' in the
    // Makefile)
    KAboutData *aboutData = new KAboutData("kuiviewerpart", I18N_NOOP("KUIViewerPart"), "0.1",
					   I18N_NOOP("Displays Designer's UI files"),
					   KAboutData::License_LGPL );
    aboutData->addAuthor("Richard Moore", 0, "rich@kde.org");
    aboutData->addAuthor("Ian Reinhart Geiser", 0, "geiseri@kde.org");
    return aboutData;
}

bool KUIViewerPart::openFile()
{
    // m_file is always local so we can use QFile on it
    QFile file( m_file );
    if ( !file.open(IO_ReadOnly) )
        return false;

    delete m_view;
    m_view = QWidgetFactory::create( &file, 0, m_widget );

    file.close();
    updateActions();

    if ( !m_view )
	return false;

    m_view->show();
    slotStyle(0);
    return true;
}

bool KUIViewerPart::openURL( const KURL& url)
{
    // just for fun, set the status bar
    emit setStatusBarText( url.prettyURL() );
    emit setWindowCaption( url.prettyURL() );

    m_url = url;
    m_file = QString::null;
    if (KIO::NetAccess::download(url, m_file))
	return openFile();
    else
	return false;
}

void KUIViewerPart::updateActions()
{
    if ( !m_view.isNull() ) {
	m_style->setEnabled( true );
	m_copy->setEnabled( true );
    }
    else {
	m_style->setEnabled( false );
	m_copy->setEnabled( false );
    }
}

void KUIViewerPart::slotStyle(int)
{
    if ( m_view.isNull() ) {
	updateActions();
	return;
    }

    QString  styleName = m_style->currentText();
    QStyle*  style     = QStyleFactory::create(styleName);
    kdDebug() << "Change style..." << endl;
    m_widget->hide();
    QApplication::setOverrideCursor( WaitCursor );
    m_widget->setStyle( style);

    QObjectList *l = m_widget->queryList( "QWidget" );
    for ( QObject *o = l->first(); o; o = l->next() )
        ( static_cast<QWidget *>(o) )->setStyle( style );
    delete l;

    m_widget->show();
    QApplication::restoreOverrideCursor();

    kapp->config()->setGroup("General");
    kapp->config()->writeEntry("currentWidgetStyle", m_style->currentText());
    kapp->config()->sync();
}

void KUIViewerPart::slotGrab()
{
    if ( m_view.isNull() ) {
	updateActions();
	return;
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(QPixmap::grabWidget(m_widget));
}

