/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include "kinitdialog.h"

KinitDialog::KinitDialog( QWidget* parent, const char* name,
                         bool modal, Qt::WindowFlags fl)
	: QDialog(parent, fl)
{
	setModal(modal);
	setName(name);
	setupUi(this);
}

KinitDialog::~KinitDialog() {}

void KinitDialog::init()
{
    changeDetailsState(QCheckBox::Off);
    changeRenewState(QCheckBox::On);
}

void KinitDialog::changeDetailsState( int state )
{
	qDebug("changeDetailsState called");
	if(state == QCheckBox::On)
	{
		detailsBox->show();
	}
	else if(state == QCheckBox::Off)
	{
		detailsBox->hide();
	}
	adjustSize();
}


void KinitDialog::changeRenewState(int state)
{
	qDebug("changeRenewState called");
	if(state == QCheckBox::On)
	{
		renewtimeSpinBox->setEnabled(true);
		renewUnitComboBox->setEnabled(true);
	}
	else if(state == QCheckBox::Off)
	{
		renewtimeSpinBox->setEnabled(false);
		renewUnitComboBox->setEnabled(false);
	}
}
