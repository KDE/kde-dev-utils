/* vi: ts=8 sts=4 sw=4
 *
 * $Id$
 *
 * This file is part of the KDE project, module kstartperf.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 *
 * You can freely redistribute this program under the "Artistic License".
 * See the file "LICENSE.readme" for the exact terms.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kstandarddirs.h>


static KCmdLineOptions options[] =
{
    { "+command", I18N_NOOP("Specifies the command to run"), 0 },
    KCmdLineLastOption
};


QString libkstartperf()
{
    QString lib = QString::null;
    QString la_file = KStandardDirs::locate("lib", "libkstartperf.la");

    if (la_file.isEmpty())
	return lib;

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
    lib = KStandardDirs::locate("lib", lib);
    return lib;
}


int main(int argc, char **argv)
{
    KAboutData aboutData("kstartperf", I18N_NOOP("KStartPerf"),
	    "1.0", I18N_NOOP("Measures start up time of a KDE application"),
	    KAboutData::License_Artistic,
	    "Copyright (c) 2000 Geert Jansen and libkmapnotify authors");
    aboutData.addAuthor("Geert Jansen", I18N_NOOP("Maintainer"),
	    "jansen@kde.org", "http://www.stack.nl/~geertj/");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    KApplication *app = new KApplication(false);

    // Check arguments

    if (args->count() == 0)
    {
	fprintf(stderr, "No command specified!\n");
	fprintf(stderr, "usage: kstartperf command [arguments]\n");
	exit(1);
    }

    // Build command

    char cmd[1024];
    sprintf(cmd, "LD_PRELOAD=%s %s", libkstartperf().latin1(), args->arg(0));
    for (int i=1; i<args->count(); i++)
    {
	strcat(cmd, " ");
	strcat(cmd, args->arg(i));
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
