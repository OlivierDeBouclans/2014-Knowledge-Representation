#include "QNewEntity.h"
#include "ui_QNewEntity.h"

#include "QtUi/New/QNewNumerical.h"
#include "QtUi/New/QNewAction.h"
#include "SemanticNet.h"

#include "QWheelEvent"

QNewEntity::QNewEntity(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QNewEntity)
{
  ui->setupUi(this);

  ui->Name->setFocus();

  connect(ui->Concrete, SIGNAL(stateChanged(int)),
          this,          SIGNAL(showPhysical(int)));

  connect(ui->Concrete, SIGNAL(stateChanged(int)),
          this,          SLOT(showAction()));

  connect(ui->Numerical, SIGNAL(stateChanged(int)),
          this,          SIGNAL(showNumerical(int)));

  connect(ui->Type, SIGNAL(currentIndexChanged(int)),
          this,     SLOT(updateOptions(int)));
}

QNewEntity::~QNewEntity()
{
  delete ui;
}

string QNewEntity::getName()
{
  return ui->Name->text().toStdString();
}

int QNewEntity::getType(int ind)
{
  if(ind==-1)
    ind=ui->Type->currentIndex();

  if(ind==0)
    return Entity::OBJECT;
  else if(ind==1)
    return Entity::ATTRIBUTE;
  else if(ind==2)
    return Entity::VALUE;
  else //if(ind==3)
    return Entity::ACTION;
}

void QNewEntity::setName(string s)
{
  ui->Name->setText(QString::fromStdString(s));
}

void QNewEntity::setType(int i)
{
  int ind=0;

  if(i==Entity::OBJECT)
    ind=0;
  else if(i==Entity::ATTRIBUTE)
    ind=1;
  else if(i==Entity::VALUE)
    ind=2;
  else if(i==Entity::ACTION)
    ind=3;

  ui->Type->setCurrentIndex(ind);
}

void QNewEntity::setNumerical(bool b)
{
  ui->Numerical->setChecked(b);
}

void QNewEntity::setPhysical(bool b)
{
  ui->Concrete->setChecked(b);
}

void QNewEntity::wheelEvent(QWheelEvent* e)
{
  int dy = e->angleDelta().y();
  int i=ui->Type->currentIndex();

  if(dy > 0 && i>0)
    ui->Type->setCurrentIndex(i-1);
  else if(dy < 0 && i < ui->Type->count()-1)
    ui->Type->setCurrentIndex(i+1);
}

void QNewEntity::updateOptions(int i)
{
  if(getType(i)==Entity::OBJECT||getType(i)==Entity::ACTION)
    {
      Numerical(false);
      Physical(true);
    }
  else
    {
      Numerical(true);
      Physical(false);
    }

  showAction();
}

void QNewEntity::showAction()
{
  int i=ui->Type->currentIndex();

  if(getType(i)==Entity::ACTION && !ui->Concrete->isChecked())
    emit showAction(true);
  else
    emit showAction(false);
}

void QNewEntity::Numerical(bool b)
{
  if(!b)
    ui->Numerical->setChecked(false);

  ui->Numerical->setEnabled(b);
}

void QNewEntity::Physical(bool b)
{
  if(!b)
    ui->Concrete->setChecked(false);

  ui->Concrete->setEnabled(b);
}
