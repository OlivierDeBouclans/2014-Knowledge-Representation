#include "QInfoAction.h"
#include "ui_QInfoAction.h"

#include "SemanticNet.h"

QInfoAction::QInfoAction(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QInfoAction)
{
  ui->setupUi(this);
}

QInfoAction::~QInfoAction()
{
  delete ui;
}

void QInfoAction::display(Entity* a)
{
  vector<SubNode*> t= a->getSubNode();
  for(unsigned int i=0; i<t.size(); ++i)
    if(TYPE_SUBNODE(t[i]->type()) == SubNode::TERMINAL)
      ui->ParameterList->addItem(QString::fromStdString(t[i]->sLabel));
}
