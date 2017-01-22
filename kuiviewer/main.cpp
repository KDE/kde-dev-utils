/**
 *
 *  This file is part of the kuiviewer package
 *  Copyright (c) 2003 Richard Moore <rich@kde.org>
 *  Copyright (c) 2003 Ian Reinhart Geiser <geiseri@kde.org>
 *  Copyright (c) 2004 Benjamin C. Meyer <ben+kuiviewer@meyerhome.net>
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

#include "kuiviewer.h"

#include <QApplication>
#include <QUrl>
#include <KAboutData>
#include <KLocalizedString>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <kprofilemethod.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kuiviewer");

    KAboutData about(QStringLiteral("kuiviewer"), i18n("KUIViewer"), QStringLiteral("0.2"),
		     i18n("Displays Designer's UI files"),
		     KAboutLicense::LGPL );
    about.addAuthor(i18n("Richard Moore"), QString(), "rich@kde.org");
    about.addAuthor(i18n("Ian Reinhart Geiser"), QString(), "geiseri@kde.org");
    // Screenshot capability
    about.addAuthor(i18n("Benjamin C. Meyer"), QString(), "ben+kuiviewer@meyerhome.net");

    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    about.setupCommandLine(&parser);

    parser.addPositionalArgument(QLatin1String("[URL]"), i18n( "Document to open" ));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("s") << QLatin1String("takescreenshot"), i18n( "Save screenshot to file and exit" ), QLatin1String("filename")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("w") << QLatin1String("screenshotwidth"), i18n( "Screenshot width" ), QLatin1String("int"), QLatin1String("-1")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("h") << QLatin1String("screenshotheight"), i18n( "Screenshot height" ), QLatin1String("int"), QLatin1String("-1")));

    parser.process(app);
    about.processCommandLine(&parser);

    // see if we are starting with session management
    if (app.isSessionRestored())
        RESTORE(KUIViewer)
    else
    {
        // no session.. just start up normally

        if ( parser.positionalArguments().count() == 0 )
        {
            KUIViewer *widget = new KUIViewer;
            widget->show();
        }
        else
        {
            int i = 0;
            for (; i < parser.positionalArguments().count(); i++ ) {
                KUIViewer *widget = new KUIViewer;
                widget->load( QUrl::fromUserInput(parser.positionalArguments().at(i)) );
            
                if (parser.isSet("takescreenshot")){
                    widget->takeScreenshot(parser.value("takescreenshot").toLocal8Bit(),
                                    parser.value("screenshotwidth").toInt(),
                                    parser.value("screenshotheight").toInt());
                    return 0;
                }
                widget->show();
            }
        }
        
    }

    return app.exec();
}
