#include "kuiviewer.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static KCmdLineOptions options[] =
{
    { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("kuiviewerpart", I18N_NOOP("KUIViewerPart"), "0.1",
		     I18N_NOOP("Displays Designer's UI files."),
		     KAboutData::License_LGPL );
    about.addAuthor("Richard Moore", 0, "rich@kde.org");
    about.addAuthor("Ian Reinhart Geiser", 0, "geiseri@kde.org");

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
            for (; i < args->count(); i++ )
            {
                KUIViewer *widget = new KUIViewer;
                widget->show();
                widget->load( args->url( i ) );
            }
        }
        args->clear();
    }

    return app.exec();
}
