#include "QInfoNumerical.h"
#include "ui_QInfoNumerical.h"

#include "SemanticNet.h"

QInfoNumerical::QInfoNumerical(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QInfoNumerical)
{
  ui->setupUi(this);
}

QInfoNumerical::~QInfoNumerical()
{
  delete ui;
}

void QInfoNumerical::display(NumericalInfo* e)
{
  if(e->dMin == NUM_MIN)
    ui->Min->setText(QString("-∞"));
  else
    ui->Min->setText(QString::number(e->dMin));

  if(e->dMax == NUM_MAX)
    ui->Max->setText(QString("+∞"));
  else
    ui->Max->setText(QString::number(e->dMax));
}
