#include "QInfoPhysical.h"
#include "ui_QInfoPhysical.h"

#include "SemanticNet.h"

#include "QDateTime"

QInfoConcrete::QInfoConcrete(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QInfoPhysical)
{
  ui->setupUi(this);
}

QInfoConcrete::~QInfoConcrete()
{
  delete ui;
}

void QInfoConcrete::display(ConcreteInfo* e)
{
  if(e->iStartTime == TIME_INVALID)
    ui->Start->setText(QString("Unknown"));
  else
    {
      QDateTime t = QDateTime::fromMSecsSinceEpoch(e->iStartTime);
      ui->Start->setText(t.toString("yyyy/MM/dd hh:mm:ss"));
    }

  if(e->iEndTime == TIME_INVALID)
    ui->End->setText(QString("Unknown"));
  else
    {
      QDateTime t = QDateTime::fromMSecsSinceEpoch(e->iEndTime);
      ui->End->setText(t.toString("yyyy/MM/dd hh:mm:ss"));
    }
}
