#ifndef KUIVIEWERPART_H
#define KUIVIEWERPART_H

#include <qguardedptr.h>
#include <kparts/part.h>

class QWidget;
class KURL;
class QVBox;
class KAboutData;
class KListAction;
class KListView;

/**
 * This is a "Part".  It that does all the real work in a KPart
 * application.
 *
 * @short Main Part
 * @author Richard Moore <rich@kde.org>
 * @version 0.1
 */
class KUIViewerPart : public KParts::ReadOnlyPart
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    KUIViewerPart(QWidget *parentWidget, const char *widgetName,
                    QObject *parent, const char *name, const QStringList &args);

    /**
     * Destructor
     */
    virtual ~KUIViewerPart();

    static KAboutData *createAboutData();

public slots:
     bool openURL( const KURL& );
     void slotStyle(int);
     void slotGrab();
     void updateActions();

protected:
    /**
     * This must be implemented by each part
     */
    virtual bool openFile();

private:
    QVBox *m_widget;
    QGuardedPtr<QWidget> m_view;
    KListAction *m_style;
    KAction *m_copy;
};

#endif // KUIVIEWERPART_H
