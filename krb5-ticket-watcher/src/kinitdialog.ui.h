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

void KinitDialog::init()
{
    changeDetailsState(QButton::Off);
    changeRenewState(QButton::On);
}

void KinitDialog::changeDetailsState( int state )
{
    qDebug("changeDetailsState called");
    if(state == QButton::On)
    {
	detailsBox->show();
    }
    else if(state == QButton::Off)
    {
	detailsBox->hide();
    }
    adjustSize();
}


void KinitDialog::changeRenewState(int state)
{
    qDebug("changeRenewState called");
    if(state == QButton::On)
    {
	renewtimeSpinBox->setEnabled(true);
	renewUnitComboBox->setEnabled(true);
    }
    else if(state == QButton::Off)
    {
	renewtimeSpinBox->setEnabled(false);
	renewUnitComboBox->setEnabled(false);
    }
}
