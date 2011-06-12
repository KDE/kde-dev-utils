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
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tqpixmap.h>
#include <tqimage.h>
#include <tqwidgetfactory.h>

#include "quicreator.h"

#include <kdemacros.h>

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new QUICreator;
    }
}

bool QUICreator::create(const TQString &path, int width, int height, TQImage &
img)
{
	TQWidget *w = TQWidgetFactory::create(path, 0, 0);
	if ( w )
	{
		TQPixmap p = TQPixmap::grabWidget(w);
		img = p.convertToImage().smoothScale(width,height,TQ_ScaleMin);
		return true;
	}
	else
		return false;
}

ThumbCreator::Flags QUICreator::flags() const
{
    return static_cast<Flags>(DrawFrame);
}

