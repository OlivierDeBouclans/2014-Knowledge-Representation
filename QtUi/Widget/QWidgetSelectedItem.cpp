#include "QWidgetSelectedItem.h"
#include "ui_QWidgetSelectedItem.h"

#include <SemanticNet.h>
#include <GraphElements.h>

QWidgetSelectedItem::QWidgetSelectedItem(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QWidgetSelectedItem)
{
  ui->setupUi(this);

  connect(ui->Quit, SIGNAL(clicked()), this, SLOT(unselect()));
}

QWidgetSelectedItem::~QWidgetSelectedItem()
{
  delete ui;
}

void QWidgetSelectedItem::display(SemanticElement* e)
{
  if(!e)
    return;

  ui->Entity->setText(QString::fromStdString(e->sLabel));
  QPalette *palette = new QPalette();
  palette->setColor(QPalette::Base,GraphElement::getColor(e));
  ui->Entity->setPalette(*palette);

  m_pSelectedElement=e;

  show();
}

void QWidgetSelectedItem::unselect()
{
  m_pSelectedElement = NULL;
  hide();
  emit unselectElement();
}
