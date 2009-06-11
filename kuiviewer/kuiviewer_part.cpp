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
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <kcomponentdata.h>
#include <kio/netaccess.h>
#include <kparts/genericfactory.h>
#include <kstandardaction.h>
#include <kstyle.h>
#include <qmetaobject.h>

#include <qclipboard.h>
#include <qcursor.h>
#include <qfile.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qvariant.h>
#include <kvbox.h>
#include <kglobal.h>
#include <QFormBuilder>
#include <kselectaction.h>

typedef KParts::GenericFactory<KUIViewerPart> KUIViewerPartFactory;
K_EXPORT_COMPONENT_FACTORY( libkuiviewerpart, KUIViewerPartFactory )

KUIViewerPart::KUIViewerPart( QWidget *parentWidget,
                                  QObject *parent,
                                  const QStringList & /*args*/ )
    : KParts::ReadOnlyPart(parent)
{
    // we need an instance
    setComponentData( KUIViewerPartFactory::componentData() );

    KGlobal::locale()->insertCatalog("kuiviewer");

    // this should be your custom internal widget
    m_widget = new KVBox( parentWidget );

    // notify the part that this is our internal widget
    setWidget(m_widget);

    // set our XML-UI resource file
    setXMLFile("kuiviewer_part.rc");

    m_style = actionCollection()->add<KSelectAction>("change_style");
    m_style->setText(i18n("Style"));
    connect(m_style, SIGNAL(triggered(int)), SLOT(slotStyle(int)));
    m_style->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    m_style->setEditable(false);

    const QString currentStyle = KConfigGroup(KGlobal::config(), "General").readEntry("currentWidgetStyle", KStyle::defaultStyle());

    const QStringList styles = QStyleFactory::keys();
    m_style->setItems(styles);
    m_style->setCurrentItem(0);

    QStringList::ConstIterator it = styles.begin();
    QStringList::ConstIterator end = styles.end();
    int idx = 0;
    for (; it != end; ++it, ++idx) {
        if ((*it).toLower() == currentStyle.toLower()) {
            m_style->setCurrentItem(idx);
            break;
        }
    }
    m_style->setToolTip(i18n("Set the current style to view."));
    m_style->setMenuAccelsEnabled(true);

    m_copy = KStandardAction::copy(this, SLOT(slotGrab()), actionCollection());

    updateActions();

// Commented out to fix warning (rich)
// slot should probably be called saveAs() for consistency with
// KParts::ReadWritePart BTW.
//    KStandardAction::saveAs(this, SLOT(slotSave()), actionCollection());
}

KUIViewerPart::~KUIViewerPart()
{
}

KAboutData *KUIViewerPart::createAboutData()
{
    // the non-i18n name here must be the same as the directory in
    // which the part's rc file is installed ('partrcdir' in the
    // Makefile)
    KAboutData *aboutData = new KAboutData("kuiviewerpart", 0, ki18n("KUIViewerPart"), "0.1",
					   ki18n("Displays Designer's UI files"),
					   KAboutData::License_LGPL );
    aboutData->addAuthor(ki18n("Richard Moore"), KLocalizedString(), "rich@kde.org");
    aboutData->addAuthor(ki18n("Ian Reinhart Geiser"), KLocalizedString(), "geiseri@kde.org");
    return aboutData;
}

bool KUIViewerPart::openFile()
{
    // m_file is always local so we can use QFile on it
    QFile file( localFilePath() );
    if ( !file.open(QIODevice::ReadOnly) )
        return false;

    delete m_view;
    QFormBuilder builder;
    m_view = builder.load(&file, m_widget);

    file.close();
    updateActions();

    if ( !m_view )
	return false;

    m_view->show();
    slotStyle(0);
    return true;
}

bool KUIViewerPart::openURL( const KUrl& url)
{
    // just for fun, set the status bar
    emit setStatusBarText( url.prettyUrl() );
    emit setWindowCaption( url.prettyUrl() );

    setUrl(url);
    setLocalFilePath( QString() );
    QString filePath;
    if (KIO::NetAccess::download(this->url(), filePath, 0L)) {
        setLocalFilePath( filePath );
	return openFile();
    }

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
    kDebug() << "Change style..." << endl;
    m_widget->hide();
    QApplication::setOverrideCursor( Qt::WaitCursor );
    m_widget->setStyle( style);

    QList<QWidget *>l = m_widget->findChildren<QWidget*>();
    for (int i = 0; i < l.size(); ++i) {
        l.at(i)->setStyle( style );
    }

    m_widget->show();
    QApplication::restoreOverrideCursor();

    KConfigGroup cg(KGlobal::config(), "General");
    cg.writeEntry("currentWidgetStyle", m_style->currentText());
    KGlobal::config()->sync();
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

