#include "kuiviewer_part.h"
#include "kuiviewer_part.moc"

#include <kaction.h>
#include <kstdaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kio/netaccess.h>
#include <kparts/genericfactory.h>

#include <qcursor.h>
#include <qclipboard.h>
#include <qfile.h>
#include <qobjectlist.h>
#include <qpixmap.h> 
#include <qstylefactory.h>
#include <qvbox.h>
#include <qwidgetfactory.h>

typedef KParts::GenericFactory<KUIViewerPart> KUIViewerPartFactory;
K_EXPORT_COMPONENT_FACTORY( libkuiviewerpart, KUIViewerPartFactory );

KUIViewerPart::KUIViewerPart( QWidget *parentWidget, const char *widgetName,
                                  QObject *parent, const char *name,
                                  const QStringList & /*args*/ )
    : KParts::ReadOnlyPart(parent, name)
{
    // we need an instance
    setInstance( KUIViewerPartFactory::instance() );

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
    m_style->setItems(QStyleFactory::keys());
    m_style->setToolTip(i18n("Set the current style to view."));
    m_style->setCurrentItem(0);
    m_style->setMenuAccelsEnabled(true);

    KStdAction::copy(this, SLOT(slotGrab()), actionCollection()); 
    KStdAction::saveAs(this, SLOT(slotSave()), actionCollection());
}

KUIViewerPart::~KUIViewerPart()
{
}

KAboutData *KUIViewerPart::createAboutData()
{
    // the non-i18n name here must be the same as the directory in
    // which the part's rc file is installed ('partrcdir' in the
    // Makefile)
    KAboutData *aboutData = new KAboutData("kuiviewerpart", I18N_NOOP("KUIViewerPart"), "0.1");
    aboutData->addAuthor("Richard Moore", 0, "rich@kde.org");
    return aboutData;
}

bool KUIViewerPart::openFile()
{
    // m_file is always local so we can use QFile on it
    QFile file( m_file );
    if ( !file.open(IO_ReadOnly) )
        return false;

    m_view = QWidgetFactory::create( &file, 0, m_widget );
    m_view->show();
    
    file.close();

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

void KUIViewerPart::slotStyle(int)
{
    QString style = m_style->currentText();
    kdDebug() << "Change style..." << endl;
    m_widget->hide();
    QApplication::setOverrideCursor( WaitCursor );
    m_widget->setStyle( style);

    QObjectList *l = m_widget->queryList( "QWidget" );
    for ( QObject *o = l->first(); o; o = l->next() )
        ( (QWidget*)o )->setStyle( style );
    delete l;

    m_widget->show();
    QApplication::restoreOverrideCursor();
}

void KUIViewerPart::slotGrab()
{
	QClipboard *clipboard = QApplication::clipboard();
        clipboard->setPixmap(QPixmap::grabWidget(m_widget));
}
