#include "QNewEdge.h"
#include "ui_QNewEdge.h"
#include "SemanticNet.h"

#include <QWheelEvent>

QNewEdge::QNewEdge(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QNewEdge)
{
  ui->setupUi(this);

  connect(ui->Concrete, SIGNAL(stateChanged(int)),
          this,         SIGNAL(showPhysical(int)));
}

QNewEdge::~QNewEdge()
{
  delete ui;
}

int QNewEdge::getType()
{
  int ind=ui->Type->currentIndex();

  if(ind==0)
    return Edge::HAS;
  else// if(ind==1)
    return Edge::IS;
}

int QNewEdge::getCard()
{
  return ui->Card->value();
}

void QNewEdge::setType(int i)
{
  int ind=0;

  if(i==Edge::HAS)
    ind=0;
  else if(i==Edge::IS)
    ind=1;

  ui->Type->setCurrentIndex(ind);
}

void QNewEdge::setCard(int i)
{
  ui->Card->setValue(i);
}

void QNewEdge::setPhysical(bool b)
{
  ui->Concrete->setChecked(b);
}

void QNewEdge::wheelEvent(QWheelEvent* e)
{
  int dy = e->angleDelta().y();
  int i=ui->Type->currentIndex();

  if(dy > 0 && i>0)
    ui->Type->setCurrentIndex(--i);
  else if(dy < 0 && i < ui->Type->count()-1)
    ui->Type->setCurrentIndex(++i);
}
