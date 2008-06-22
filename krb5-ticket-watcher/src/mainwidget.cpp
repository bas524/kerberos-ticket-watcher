
#include "mainwidget.h"

MainWidget::MainWidget( QWidget* parent, Qt::WindowFlags fl )
	: QWidget(parent, fl)
{
	setupUi(this);

	connect(refreshButton, SIGNAL(clicked()), this, SLOT(emitRefresh()));

}

MainWidget::~MainWidget() {}

void
MainWidget::emitRefresh()
{
	emit refreshClicked();
}

void
MainWidget::setCommonLabel(const QString &text)
{
	commonLabel->setText("<qt><pre>" + text + "</pre></qt>");
}
