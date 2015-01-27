#include "QInfoEntity.h"
#include "ui_QInfoEntity.h"

#include "SemanticNet.h"

QInfoEntity::QInfoEntity(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QInfoEntity)
{
  ui->setupUi(this);
}

QInfoEntity::~QInfoEntity()
{
  delete ui;
}

void QInfoEntity::display(Entity* e)
{
  ui->Name->setText(QString::fromStdString(e->sLabel));
  ui->Type->setText(QString::fromStdString(e->typeToString()));
  ui->Concrete->setChecked(e->pConcrete);
  ui->Numerical->setChecked(e->pNumerical);
}
