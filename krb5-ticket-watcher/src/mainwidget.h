#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <qvariant.h>
#include <qwidget.h>

#include "ui_mainwidget.h"

#include <QtGui/QLabel>
class QLineEdit;
class QComboBox;
class QCheckBox;
class QSpinBox;

class MainWidget : public QWidget, private Ui::MainWidget
{
    Q_OBJECT

public:
    MainWidget( QWidget* parent = 0, Qt::WindowFlags fl = 0 );
    ~MainWidget();

	void setCommonLabel(const QString &text);

	void ticketViewClear()
	{
		ticketView->clear();
	}

	void ticketViewFirstChildSetOpen(bool st)
	{
		ticketView->firstChild()->setOpen(st);
	}

	Q3ListView * getTicketView()
	{
		return ticketView;
	}
	
signals:
    void refreshClicked();

private slots:
	void emitRefresh();
	
};

#endif // MAINWIDGET_H
