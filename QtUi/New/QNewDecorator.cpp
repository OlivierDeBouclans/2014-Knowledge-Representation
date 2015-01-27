#include "QNewDecorator.h"
#include "ui_QNewDecorator.h"
#include "SemanticNet.h"

QNewDecorator::QNewDecorator(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QNewDecorator)
{
  ui->setupUi(this);
}

QNewDecorator::~QNewDecorator()
{
  delete ui;
}

int QNewDecorator::getType()
{
  int ind=ui->Type->currentIndex();

  if(ind==0)
    return Decorator::OR;
  else if(ind==1)
    return Decorator::AND;
  else// if(ind==2)
    return Decorator::XOR;
}

bool QNewDecorator::getPartition()
{
  return ui->Partition->isChecked();
}

void QNewDecorator::setType(int i)
{
  int ind=0;

  if(i==Decorator::OR)
    ind=0;
  else if(i==Entity::ATTRIBUTE)
    ind=1;
  else if(i==Decorator::AND)
    ind=2;
  else if(i==Decorator::XOR)
    ind=3;

  ui->Type->setCurrentIndex(ind);
}

void QNewDecorator::setPartition(bool b)
{
  ui->Partition->setChecked(b);
}
