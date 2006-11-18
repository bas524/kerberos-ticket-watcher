/*
 * krb5-ticket-watcher
 *
 * Copyright (C) 2006  Michael Calmer (Michael.Calmer at gmx.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "TicketListItem.h"

void
TicketListItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
	QColorGroup _cg( cg );
	QColor c = _cg.base();
	
	if( exp == 0 )
		_cg.setColor( QColorGroup::Base, Qt::red );
	else if(exp > 0 && exp < 60)
		_cg.setColor( QColorGroup::Base, Qt::yellow );
	else
		_cg.setColor( QColorGroup::Base, Qt::green );
	
	QListViewItem::paintCell( p, _cg, column, width, align);
	
	_cg.setColor( QColorGroup::Base, c );
};
