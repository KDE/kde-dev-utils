#ifndef _UICREATOR_H_
#define _UICREATOR_H_ 
/*$Id$*/

#include <kio/thumbcreator.h>

class QUICreator : public ThumbCreator
{
public:
    QUICreator() {};
    virtual bool create(const QString &path, int, int, QImage &img);
	virtual Flags flags() const;
};

#endif
