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
#include <tqmetaobject.h>

#include <tqclipboard.h>
#include <tqcursor.h>
#include <tqfile.h>
#include <tqobjectlist.h>
#include <tqpixmap.h>
#include <tqstyle.h>
#include <tqstylefactory.h>
#include <tqvariant.h>
#include <tqvbox.h>
#include <tqvariant.h>
#include <tqwidgetfactory.h>

typedef KParts::GenericFactory<KUIViewerPart> KUIViewerPartFactory;
K_EXPORT_COMPONENT_FACTORY( libkuiviewerpart, KUIViewerPartFactory )

KUIViewerPart::KUIViewerPart( TQWidget *tqparentWidget, const char *widgetName,
                                  TQObject *tqparent, const char *name,
                                  const TQStringList & /*args*/ )
    : KParts::ReadOnlyPart(tqparent, name)
{
    // we need an instance
    setInstance( KUIViewerPartFactory::instance() );

    KGlobal::locale()->insertCatalogue("kuiviewer");

    // this should be your custom internal widget
    m_widget = new TQVBox( tqparentWidget, widgetName );

    // notify the part that this is our internal widget
    setWidget(m_widget);

    // set our XML-UI resource file
    setXMLFile("kuiviewer_part.rc");

    m_style = new KListAction( i18n("Style"),
                CTRL + Key_S,
                TQT_TQOBJECT(this),
                TQT_SLOT(slotStyle(int)),
                actionCollection(),
                "change_style");
    m_style->setEditable(false);

    kapp->config()->setGroup("General");
    const TQString currentStyle = kapp->config()->readEntry("currentWidgetStyle", KStyle::defaultStyle());

    const TQStringList styles = TQStyleFactory::keys();
    m_style->setItems(styles);
    m_style->setCurrentItem(0);

    TQStringList::ConstIterator it = styles.begin();
    TQStringList::ConstIterator end = styles.end();
    int idx = 0;
    for (; it != end; ++it, ++idx) {
        if ((*it).lower() == currentStyle.lower()) {
            m_style->setCurrentItem(idx);
            break;
        }
    }
    m_style->setToolTip(i18n("Set the current style to view."));
    m_style->setMenuAccelsEnabled(true);

    m_copy = KStdAction::copy(this, TQT_SLOT(slotGrab()), actionCollection());

    updateActions();

// Commented out to fix warning (rich)
// slot should probably be called saveAs() for consistency with
// KParts::ReadWritePart BTW.
//    KStdAction::saveAs(this, TQT_SLOT(slotSave()), actionCollection());
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
    // m_file is always local so we can use TQFile on it
    TQFile file( m_file );
    if ( !file.open(IO_ReadOnly) )
        return false;

    delete m_view;
    m_view = TQWidgetFactory::create( &file, 0, m_widget );

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
    m_file = TQString();
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

    TQString  styleName = m_style->currentText();
    TQStyle*  style     = TQStyleFactory::create(styleName);
    kdDebug() << "Change style..." << endl;
    m_widget->hide();
    TQApplication::setOverrideCursor( WaitCursor );
    m_widget->setStyle( style);

    TQObjectList *l = m_widget->queryList( TQWIDGET_OBJECT_NAME_STRING );
    for ( TQObject *o = l->first(); o; o = l->next() )
        ( TQT_TQWIDGET(o) )->setStyle( style );
    delete l;

    m_widget->show();
    TQApplication::restoreOverrideCursor();

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

    TQClipboard *clipboard = TQApplication::tqclipboard();
    clipboard->setPixmap(TQPixmap::grabWidget(m_widget));
}

