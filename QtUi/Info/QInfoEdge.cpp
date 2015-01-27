#include "QInfoEdge.h"
#include "ui_QInfoEdge.h"

#include "SemanticNet.h"

#include "GraphElements.h"

QInfoEdge::QInfoEdge(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QInfoEdge)
{
  ui->setupUi(this);
}

QInfoEdge::~QInfoEdge()
{
  delete ui;
}

void QInfoEdge::display(Edge* e)
{
  Entity* parent=e->getParent();

  ui->Parent->setText(QString::fromStdString(parent->sLabel));
  QPalette *palette = new QPalette();
  palette->setColor(QPalette::Base,GraphElement::getColor(parent));
  ui->Parent->setPalette(*palette);

  ui->Label->setText(QString::fromStdString(e->sLabel));
  ui->Type->setText(QString::fromStdString(e->typeToString()));
  ui->Card->setText(QString::number(e->iCard));
  ui->Concrete->setChecked(e->pConcrete);
}
