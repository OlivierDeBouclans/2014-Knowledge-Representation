#include "QNewNumerical.h"
#include "ui_QNewNumerical.h"

#include "SemanticNet.h"

QNewNumerical::QNewNumerical(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QNewNumerical)
{
  ui->setupUi(this);
}

QNewNumerical::~QNewNumerical()
{
  delete ui;
}

double QNewNumerical::getMin()
{
  if(!ui->GetMin->isChecked())
    return NUM_MIN;

  return ui->Min->value();
}

double QNewNumerical::getMax()
{
  if(!ui->GetMax->isChecked())
    return NUM_MAX;

  return ui->Max->value();
}

void QNewNumerical::setMax(double d)
{
  ui->GetMax->setChecked(d!=NUM_MAX);

  if(d!=NUM_MAX)
    ui->Max->setValue(d);
}

void QNewNumerical::setMin(double d)
{
  ui->GetMin->setChecked(d!=NUM_MIN);

  if(d!=NUM_MIN)
    ui->Min->setValue(d);
}
