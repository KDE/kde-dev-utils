#ifndef KUIVIEWERPART_H
#define KUIVIEWERPART_H

#include <kparts/part.h>

class QWidget;
class QPainter;
class KURL;
class QVBox;
class KAboutData;
class KListAction;

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

protected:
    /**
     * This must be implemented by each part
     */
    virtual bool openFile();

private:
    QVBox *m_widget;
    QWidget *m_view;
    KListAction *m_style;
};

#endif // KUIVIEWERPART_H
