#include "QInfoDecorator.h"
#include "ui_QInfoDecorator.h"

#include "SemanticNet.h"
#include "GraphElements.h"

QInfoDecorator::QInfoDecorator(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QInfoDecorator)
{
  ui->setupUi(this);
}

QInfoDecorator::~QInfoDecorator()
{
  delete ui;
}

void QInfoDecorator::display(Decorator* d)
{
  Entity* parent=d->getParent();

  ui->Parent->setText(QString::fromStdString(parent->sLabel));
  QPalette *palette = new QPalette();
  palette->setColor(QPalette::Base,GraphElement::getColor(parent));
  ui->Parent->setPalette(*palette);

  ui->Type->setText(QString::fromStdString(d->typeToString()));
  ui->Partition->setChecked(d->bPartition);
}
