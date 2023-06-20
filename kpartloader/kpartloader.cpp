/*
    SPDX-FileCopyrightText: 2008 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kpartloader.h"

// app
#include "kpartloader_version.h"
// KF
#include <kcoreaddons_version.h>
#include <KAboutData>
#include <KActionCollection>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KPluginMetaData>
#include <KAboutPluginDialog>
#include <KMessageBox>
// Qt
#include <QAction>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QIcon>


KPartLoaderWindow::KPartLoaderWindow(const QString& partLib)
    : m_part(nullptr)
{
    setXMLFile(QStringLiteral("kpartloaderui.rc"));

    QAction * a = new QAction( QIcon::fromTheme(QStringLiteral("application-exit")), i18n("&Quit"), this );
    actionCollection()->addAction( "file_quit", a );
    connect(a, SIGNAL(triggered()), this, SLOT(close()));

    a = actionCollection()->addAction(QStringLiteral("help_about_kpart"));
    a->setText(i18n("&About KPart..."));
    connect(a, SIGNAL(triggered()), this, SLOT(aboutKPart()));

    KPluginFactory *factory = KPluginFactory::loadFactory(KPluginMetaData(partLib)).plugin;
    if (factory) {
        // Create the part
        m_part = factory->create<KParts::ReadOnlyPart>(this, this);
    } else {
        KMessageBox::error(this, i18n("No part named %1 found.", partLib));
    }

    if (m_part) {
        setCentralWidget( m_part->widget() );
        // Integrate its GUI
        createGUI( m_part );
    }

    // Set a reasonable size
    resize( 600, 350 );
}

KPartLoaderWindow::~KPartLoaderWindow()
{
}

void KPartLoaderWindow::aboutKPart()
{
    KAboutPluginDialog dlg(m_part->metaData(), this);
    dlg.exec();
}

int main( int argc, char **argv )
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif

    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kpartloader");

    KAboutData aboutData(
        QStringLiteral("kpartloader"),
        i18n("KPartLoader"),
        QStringLiteral(KPARTLOADER_VERSION_STRING),
        i18n("This is a test application for KParts."),
        KAboutLicense::GPL
    );
    KAboutData::setApplicationData(aboutData);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kde-frameworks"), app.windowIcon()));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.addPositionalArgument(QLatin1String("part"), i18n("Name of the part to load, e.g. dolphinpart"));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    const QStringList args = parser.positionalArguments();
    if ( args.count() == 1 )
    {
        KPartLoaderWindow *shell = new KPartLoaderWindow(args.at(0));
        shell->show();
        return app.exec();
    }
    return -1;
}

#include "moc_kpartloader.cpp"
