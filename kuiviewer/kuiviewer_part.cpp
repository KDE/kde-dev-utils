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

// Frameworks
#include <KActionCollection>
#include <KSelectAction>
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KLocalizedString>
#include <KAboutData>
#include <KPluginFactory>
#include <KIO/NetAccess>
// Qt
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFile>
#include <QFormBuilder>
#include <QStyle>
#include <QStyleFactory>
#include <QVBoxLayout>


K_PLUGIN_FACTORY( KUIViewerPartFactory, registerPlugin<KUIViewerPart>(); )

KUIViewerPart::KUIViewerPart( QWidget *parentWidget,
                              QObject *parent,
                              const QVariantList &/*args*/ )
    : KParts::ReadOnlyPart(parent)
{
    // we need an instance
    KAboutData about(QStringLiteral("kuiviewerpart"),
        i18n("KUIViewerPart"),
        QStringLiteral("0.1"),
        i18n("Displays Designer's UI files"),
        KAboutLicense::LGPL);
    about.addAuthor(i18n("Richard Moore"), QString(), "rich@kde.org");
    about.addAuthor(i18n("Ian Reinhart Geiser"), QString(), "geiseri@kde.org");
    setComponentData(about);

    // this should be your custom internal widget
    m_widget = new QWidget( parentWidget );
    QVBoxLayout *widgetVBoxLayout = new QVBoxLayout(m_widget);
    widgetVBoxLayout->setMargin(0);

    // notify the part that this is our internal widget
    setWidget(m_widget);

    setComponentName(QStringLiteral("kuiviewerpart"), QString());

    // set our XML-UI resource file
    setXMLFile("kuiviewer_part.rc");

    m_style = actionCollection()->add<KSelectAction>("change_style");
    m_style->setText(i18n("Style"));
    connect(m_style, SIGNAL(triggered(int)), SLOT(slotStyle(int)));
    //m_style->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    m_style->setEditable(false);

    m_styleFromConfig = KConfigGroup(KSharedConfig::openConfig(), "General").readEntry("currentWidgetStyle", QString());

    const QStringList styles = QStyleFactory::keys();
    m_style->setItems(QStringList(i18nc("Default style", "Default")) + styles);
    m_style->setCurrentItem(0);

    // empty or incorrect value  means the default value of currentWidgetStyle,
    // which leads to the Default option.
    if (!m_styleFromConfig.isEmpty()) {
        QStringList::ConstIterator it = styles.begin();
        QStringList::ConstIterator end = styles.end();

        // Skip the default item
        int idx = 1;
        for (; it != end; ++it, ++idx) {
            if ((*it).toLower() == m_styleFromConfig.toLower()) {
                m_style->setCurrentItem(idx);
                break;
            }
        }
    }
    m_style->setToolTip(i18n("Set the style used for the view."));
    m_style->setMenuAccelsEnabled(true);

    m_copy = KStandardAction::copy(this, SLOT(slotGrab()), actionCollection());
    m_copy->setText(i18n("Copy as Image"));

    updateActions();

// Commented out to fix warning (rich)
// slot should probably be called saveAs() for consistency with
// KParts::ReadWritePart BTW.
//    KStandardAction::saveAs(this, SLOT(slotSave()), actionCollection());
}

KUIViewerPart::~KUIViewerPart()
{
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

bool KUIViewerPart::openURL( const QUrl &url)
{
    // just for fun, set the status bar
    emit setStatusBarText( url.toDisplayString() );
    emit setWindowCaption( url.toDisplayString() );

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
    qDebug() << "Change style: " << styleName;
    m_widget->hide();
    QApplication::setOverrideCursor( Qt::WaitCursor );
    m_widget->setStyle( style);

    QList<QWidget *>l = m_widget->findChildren<QWidget*>();
    for (int i = 0; i < l.size(); ++i) {
        l.at(i)->setStyle( style );
    }

    m_widget->show();
    QApplication::restoreOverrideCursor();

    /* the style changed, update the configuration */
    if (m_styleFromConfig != styleName) {
        KSharedConfig::Ptr cfg = KSharedConfig::openConfig();
        KConfigGroup cg(cfg, "General");
        if (m_style->currentItem() > 0) {
            /* A style different from the default */
            cg.writeEntry("currentWidgetStyle", styleName);
        } else {
            /* default style: remove the entry */
            cg.deleteEntry("currentWidgetStyle");
        }
        cfg->sync();
    }
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

#include "kuiviewer_part.moc"
