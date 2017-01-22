/* vi: ts=8 sts=4 sw=4
 *
 * $Id$
 *
 * This file is part of the KDE project, module kstartperf.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFile>
#include <QString>
#include <QTextStream>

#include <KAboutData>
#include <KLocalizedString>

#include <kstandarddirs.h>

QString libkstartperf()
{
    QString lib;
    QString la_file = KStandardDirs::locate("module", "kstartperf.la");

    if (la_file.isEmpty())
    {
        // if no '.la' file could be found, fallback to a search for the .so file
        // in the standard KDE directories
        lib = KStandardDirs::locate("module","kstartperf.so");
	return lib;
    }

    // Find the name of the .so file by reading the .la file
    QFile la(la_file);
    if (la.open(QIODevice::ReadOnly))
    {
	QTextStream is(&la);
	QString line;

	while (!is.atEnd())
        {
	    line = is.readLine();
            if (line.left(15) == "library_names='")
            {
		lib = line.mid(15);
                int pos = lib.indexOf(' ');
                if (pos > 0)
		    lib = lib.left(pos);
	    }
	}

        la.close();
    }

    // Look up the actual .so file.
    lib = KStandardDirs::locate("module", lib);
    return lib;
}


int main(int argc, char **argv)
{
    QCoreApplication app( argc, argv );

    KAboutData aboutData(QStringLiteral("kstartperf"), i18n("KStartPerf"),
	    QStringLiteral("1.0"), i18n("Measures start up time of a KDE application"),
	    KAboutLicense::Artistic,
	    i18n("Copyright (c) 2000 Geert Jansen and libkmapnotify authors"));
    aboutData.addAuthor(i18n("Geert Jansen"), i18n("Maintainer"),
	    "jansen@kde.org", "http://www.stack.nl/~geertj/");

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);
    parser.addPositionalArgument(QLatin1String("command"), i18n("Specifies the command to run"));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("!+[args]"), i18n("Arguments to 'command'")));

    parser.process(app); // PORTING SCRIPT: move this to after any parser.addOption
    aboutData.processCommandLine(&parser);

    // Check arguments

    if (parser.positionalArguments().count() == 0)
    {
	fprintf(stderr, "No command specified!\n");
	fprintf(stderr, "usage: kstartperf command [arguments]\n");
	exit(1);
    }

    // Build command

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "LD_PRELOAD=%s %s", 
             qPrintable( libkstartperf() ), qPrintable(parser.positionalArguments().at(0)));
    for (int i=1; i<parser.positionalArguments().count(); i++)
    {
	strcat(cmd, " ");
	strcat(cmd, parser.positionalArguments().at(i).toLocal8Bit());
    }

    // Put the current time in the environment variable `KSTARTPERF'

    struct timeval tv;
    if (gettimeofday(&tv, 0L) != 0)
    {
	perror("gettimeofday()");
	exit(1);
    }
    char env[100];
    sprintf(env, "KSTARTPERF=%ld:%ld", tv.tv_sec, tv.tv_usec);
    putenv(env);

    // And exec() the command

    execl("/bin/sh", "sh", "-c", cmd, (void *)0);

    perror("execl()");
    exit(1);
}
