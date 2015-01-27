#include "QNewPhysical.h"
#include "ui_QNewPhysical.h"

#include "SemanticNet.h"

QNewPhysical::QNewPhysical(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QNewPhysical)
{
  ui->setupUi(this);

  connect(ui->Start, SIGNAL(stateChanged(int)),
          this,      SLOT(showStart(int)));

  connect(ui->End,  SIGNAL(stateChanged(int)),
          this,     SLOT(showEnd(int)));
}

QNewPhysical::~QNewPhysical()
{
  delete ui;
}

void QNewPhysical::showStart(int b)
{
  ui->StartTime->setEnabled(b);
}

void QNewPhysical::showEnd(int b)
{
  ui->EndTime->setEnabled(b);
}

__int64 QNewPhysical::getStart()
{
  if(!ui->Start->isChecked())
    return (__int64) TIME_INVALID;

  return (__int64) ui->StartTime->dateTime().toMSecsSinceEpoch();
}

__int64 QNewPhysical::getEnd()
{
  if(!ui->End->isChecked())
    return (__int64) TIME_INVALID;

  return (__int64) ui->EndTime->dateTime().toMSecsSinceEpoch();
}

void QNewPhysical::setStart(__int64 start)
{
  ui->Start->setChecked(start != TIME_INVALID);

  if(start != TIME_INVALID)
    ui->StartTime->setDateTime(QDateTime::fromMSecsSinceEpoch(start));
}

void QNewPhysical::setEnd(__int64 end)
{
  ui->End->setChecked(end != TIME_INVALID);

  if(end != TIME_INVALID)
    ui->EndTime->setDateTime(QDateTime::fromMSecsSinceEpoch(end));

}


