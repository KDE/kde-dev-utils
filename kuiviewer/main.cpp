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

// app
#include "kuiviewer_version.h"
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
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kuiviewer");

    KAboutData about(QStringLiteral("kuiviewer"), i18n("KUIViewer"),
                     QStringLiteral(KUIVIEWER_VERSION_STRING),
                     i18n("Displays Designer's UI files"),
                     KAboutLicense::LGPL);
    about.addAuthor(i18n("Richard Moore"), i18n("Original author"), QStringLiteral("rich@kde.org"));
    about.addAuthor(i18n("Ian Reinhart Geiser"), i18n("Original author"), QStringLiteral("geiseri@kde.org"));
    // Screenshot capability
    about.addAuthor(i18n("Benjamin C. Meyer"), i18n("Screenshot capability"), QStringLiteral("ben+kuiviewer@meyerhome.net"));
    about.addAuthor(i18n("Friedrich W. H. Kossebau"), i18n("Subwindow-like display of UI files"), QStringLiteral("kossebau@kde.org"));

    KAboutData::setApplicationData(about);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kuiviewer"), app.windowIcon()));

    QCommandLineParser parser;
    about.setupCommandLine(&parser);

    parser.addPositionalArgument(QLatin1String("[URL]"), i18n("Document to open"));
    const QString takeScreenshotOptionKey(QStringLiteral("takescreenshot"));
    const QString screenshotWidthOptionKey(QStringLiteral("screenshotwidth"));
    const QString screenshotHeightOptionKey(QStringLiteral("screenshotheight"));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("s") << takeScreenshotOptionKey, i18n("Save screenshot to file and exit"), QLatin1String("filename")));
    parser.addOption(QCommandLineOption({QStringLiteral("sw"), screenshotWidthOptionKey},
                                        i18n("Screenshot width"), QLatin1String("int"), QLatin1String("-1")));
    parser.addOption(QCommandLineOption({QStringLiteral("sh"), screenshotHeightOptionKey},
                                        i18n("Screenshot height"), QLatin1String("int"), QLatin1String("-1")));

    parser.process(app);
    about.processCommandLine(&parser);

    // see if we are starting with session management
    if (app.isSessionRestored()) {
        RESTORE(KUIViewer)
    } else {
        // no session.. just start up normally

        KUIViewer* widget = new KUIViewer;
        if (!widget->isReady()) return 1;

        const auto positionalArguments = parser.positionalArguments();
        if (positionalArguments.isEmpty()) {
            widget->show();
        } else {
            const bool takeScreenshot = parser.isSet(takeScreenshotOptionKey);
            // show before loading, so widget geometries will be properly updated when requested
            // TODO: investigate how to do this properly with perhaps showevents & Co.?
            if (takeScreenshot) {
                widget->showMinimized();
            } else {
                widget->show();
            }

            widget->load(QUrl::fromUserInput(positionalArguments.at(0), QDir::currentPath()));

            if (takeScreenshot) {
                widget->takeScreenshot(parser.value(takeScreenshotOptionKey),
                                       parser.value(screenshotWidthOptionKey).toInt(),
                                       parser.value(screenshotHeightOptionKey).toInt());
                return 0;
            }
        }
    }

    return app.exec();
}
