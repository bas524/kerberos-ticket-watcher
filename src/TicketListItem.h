/*
 * krb5-ticket-watcher
 *
 * Copyright (C) 2006  Michael Calmer (Michael.Calmer at gmx.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef  TICKET_LIST_ITEM_H
#define  TICKET_LIST_ITEM_H

#include <qstring.h>
#include <qlistview.h>
#include <krb5.h>

class TicketListItem : public QListViewItem
{
	public:
	TicketListItem( QListView * parent )
		: QListViewItem(parent),
		  exp(-1)
	{};
	
	TicketListItem( TicketListItem * parent )
		: QListViewItem(parent),
		  exp(-1)
	{};

	TicketListItem( TicketListItem * parent, QListViewItem * after )
		: QListViewItem(parent, after),
		  exp(-1)
	{};

	TicketListItem( QListView * parent, QListViewItem * after )
		: QListViewItem(parent, after),
		  exp(-1)
	{};

	TicketListItem(QListView * parent,
	               QString label1, QString label2 = QString::null, QString label3 = QString::null,
	               QString label4 = QString::null, QString label5 = QString::null,
	               QString label6 = QString::null, QString label7 = QString::null,
	               QString label8 = QString::null )
		: QListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8),
		  exp(-1)
	{};

	TicketListItem(TicketListItem * parent,
	               QString label1, QString label2 = QString::null, QString label3 = QString::null,
	               QString label4 = QString::null, QString label5 = QString::null,
	               QString label6 = QString::null, QString label7 = QString::null,
	               QString label8 = QString::null )
		: QListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8),
		  exp(-1)
	{};
		
	TicketListItem(QListView * parent, QListViewItem * after, krb5_timestamp expires,
	               QString label1, QString label2 = QString::null, QString label3 = QString::null,
	               QString label4 = QString::null, QString label5 = QString::null,
	               QString label6 = QString::null, QString label7 = QString::null,
	               QString label8 = QString::null )
		: QListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8),
		  exp(expires)
	{};

	TicketListItem(TicketListItem * parent, QListViewItem * after,
	               QString label1, QString label2 = QString::null, QString label3 = QString::null,
	               QString label4 = QString::null, QString label5 = QString::null,
	               QString label6 = QString::null, QString label7 = QString::null,
	               QString label8 = QString::null )
		: QListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8),
		  exp(-1)
	{};

	virtual ~TicketListItem() {};
	
	virtual void
	paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align );

	private:
	krb5_timestamp exp;
};

#endif /* TICKET_LIST_ITEM_H */
