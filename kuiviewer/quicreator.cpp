// $Id$

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <qpixmap.h>
#include <qimage.h>
#include <qwidgetfactory.h>


#include "quicreator.h"



extern "C"
{
    ThumbCreator *new_creator()
    {
        return new QUICreator;
    }
};


bool QUICreator::create(const QString &path, int width, int height, QImage &
img)
{
	QWidget *w = QWidgetFactory::create(path, 0, 0);
	if ( w )
	{
		QPixmap p = QPixmap::grabWidget(w);
		img = p.convertToImage().smoothScale(width,height,QImage::ScaleMin);
		return true;
	}
	else
		return false;		
}

ThumbCreator::Flags QUICreator::flags() const
{
    return static_cast<Flags>(DrawFrame);
}
