#ifndef PWCHANGEDIALOG_H
#define PWCHANGEDIALOG_H

#include <QDialog>
#include <QString>

#include "krb5_tw_gettext.h"

#include "ui_pwchangedialog.h"

class PWChangeDialog : public QDialog, private Ui::PWChangeDialog
{
    Q_OBJECT

public:
    PWChangeDialog( QWidget* parent = 0, const char* name = 0,
                   bool modal = FALSE, Qt::WindowFlags fl = 0 )
    	: QDialog(parent, fl)
	{
		setModal(modal);
		setAccessibleName(name);
		setupUi(this);
	}
	
	~PWChangeDialog() {}

	void titleTextLabelSetText(const QString& text)
	{
		titleTextLabel->setText(text);
	}

	void errorLabelSetText(const QString& text)
	{
		errorLabel->setText(text);
	}

	QString pwEdit1Text() const
	{
		return pwEdit1->text();
	}

	QString pwEdit2Text() const
	{
		return pwEdit2->text();
	}

};

#endif // PWCHANGEDIALOG_H
