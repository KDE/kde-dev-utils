#include "kuiviewer_part.h"
#include "kuiviewer_part.moc"

#include <kinstance.h>
#include <kparts/genericfactory.h>
#include <kio/netaccess.h>

#include <qfile.h>
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

    QWidget *view = QWidgetFactory::create( &file, 0, m_widget );
    view->show();
    
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
