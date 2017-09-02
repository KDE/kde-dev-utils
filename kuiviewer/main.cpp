/*
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
 */

#include "kuiviewer.h"

// KF
#include <KAboutData>
#include <KLocalizedString>
// Qt
#include <QApplication>
#include <QDir>
#include <QUrl>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kuiviewer");

    KAboutData about(QStringLiteral("kuiviewer"), i18n("KUIViewer"), QStringLiteral("0.2"),
                     i18n("Displays Designer's UI files"),
                     KAboutLicense::LGPL);
    about.addAuthor(i18n("Richard Moore"), i18n("Original author"), QStringLiteral("rich@kde.org"));
    about.addAuthor(i18n("Ian Reinhart Geiser"), i18n("Original author"), QStringLiteral("geiseri@kde.org"));
    // Screenshot capability
    about.addAuthor(i18n("Benjamin C. Meyer"), i18n("Screenshot capability"), QStringLiteral("ben+kuiviewer@meyerhome.net"));

    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    about.setupCommandLine(&parser);

    parser.addPositionalArgument(QLatin1String("[URL]"), i18n("Document to open"));
    const QString takeScreenshotOptionKey(QStringLiteral("takescreenshot"));
    const QString screenshotWidthOptionKey(QStringLiteral("screenshotwidth"));
    const QString screenshotHeightOptionKey(QStringLiteral("screenshotheight"));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("s") << takeScreenshotOptionKey, i18n("Save screenshot to file and exit"), QLatin1String("filename")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("w") << screenshotWidthOptionKey, i18n("Screenshot width"), QLatin1String("int"), QLatin1String("-1")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("h") << screenshotHeightOptionKey, i18n("Screenshot height"), QLatin1String("int"), QLatin1String("-1")));

    parser.process(app);
    about.processCommandLine(&parser);

    // see if we are starting with session management
    if (app.isSessionRestored()) {
        RESTORE(KUIViewer)
    } else {
        // no session.. just start up normally

        if (parser.positionalArguments().isEmpty()) {
            KUIViewer* widget = new KUIViewer;
            widget->show();
        } else {
            int i = 0;
            for (; i < parser.positionalArguments().count(); i++) {
                KUIViewer* widget = new KUIViewer;
                widget->load(QUrl::fromUserInput(parser.positionalArguments().at(i), QDir::currentPath()));

                if (parser.isSet(takeScreenshotOptionKey)) {
                    widget->takeScreenshot(parser.value(takeScreenshotOptionKey),
                                           parser.value(screenshotWidthOptionKey).toInt(),
                                           parser.value(screenshotHeightOptionKey).toInt());
                    return 0;
                }
                widget->show();
            }
        }

    }

    return app.exec();
}
