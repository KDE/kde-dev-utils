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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "kuiviewer.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static KCmdLineOptions options[] =
{
    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    { "s",0,0 },
    { "takescreenshot <filename>", I18N_NOOP( "Save screenshot to file and exit" ), 0 },
    { "w",0,0 },
    { "screenshotwidth <int>", I18N_NOOP( "Screenshot width" ), "-1" },
    { "h",0,0 },
    { "screenshotheight <int>", I18N_NOOP( "Screenshot height" ), "-1" },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("kuiviewer", I18N_NOOP("KUIViewer"), "0.1",
		     I18N_NOOP("Displays Designer's UI files"),
		     KAboutData::License_LGPL );
    about.addAuthor("Richard Moore", 0, "rich@kde.org");
    about.addAuthor("Ian Reinhart Geiser", 0, "geiseri@kde.org");
    // Screenshot capability
    about.addAuthor("Benjamin C. Meyer", 0, "ben+kuiviewer@meyerhome.net");

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;

    // see if we are starting with session management
    if (app.isRestored())
        RESTORE(KUIViewer)
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        if ( args->count() == 0 )
        {
            KUIViewer *widget = new KUIViewer;
            widget->show();
        }
        else
        {
            int i = 0;
            for (; i < args->count(); i++ ) {
                KUIViewer *widget = new KUIViewer;
                widget->load( args->url(i) );
            
                if (args->isSet("takescreenshot")){
                    widget->takeScreenshot(args->getOption("takescreenshot"),
                                    QString(args->getOption("screenshotwidth")).toInt(),
                                    QString(args->getOption("screenshotheight")).toInt());
                    return 0;
                }
                widget->show();
            }
        }
        args->clear();
    }

    return app.exec();
}
