/*  This file is part of the KDE project
 *  Copyright 2008  David Faure  <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License or ( at
 *  your option ) version 3 or, at the discretion of KDE e.V. ( which shall
 *  act as a proxy as in section 14 of the GPLv3 ), any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "kpartloader.h"

#include <QAction>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QIcon>
#include <KAboutData>
#include <KActionCollection>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KPluginLoader>
#include <kmessagebox.h>


KPartLoaderWindow::KPartLoaderWindow(const QString& partLib)
    : m_part(0)
{
    setXMLFile(QStringLiteral("kpartloaderui.rc"));

    QAction * paQuit = new QAction( QIcon::fromTheme(QStringLiteral("application-exit")), i18n("&Quit"), this );
    actionCollection()->addAction( "file_quit", paQuit );
    connect(paQuit, SIGNAL(triggered()), this, SLOT(close()));

    KPluginLoader loader(partLib);
    KPluginFactory* factory = loader.factory();
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

int main( int argc, char **argv )
{
    QApplication app(argc, argv);
    const char version[] = "v 1.1";

    KLocalizedString::setApplicationDomain("kpartloader");

    KAboutData aboutData(QLatin1String("kpartloader"), i18n("kpartloader"), QLatin1String(version));
    aboutData.setShortDescription(i18n("This is a test application for KParts."));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.addVersionOption();
    parser.addHelpOption();
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
