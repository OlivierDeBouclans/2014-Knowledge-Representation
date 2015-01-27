#include "QDockInfoElement.h"
#include "ui_QDockInfoElement.h"

#include "Info/QInfoAction.h"
#include "Info/QInfoDecorator.h"
#include "Info/QInfoEdge.h"
#include "Info/QInfoEntity.h"
#include "Info/QInfoNumerical.h"
#include "Info/QInfoPhysical.h"
#include "Info/QInfoComposition.h"

#include "SemanticNet.h"

#include <QCloseEvent>

QDockInfoElement::QDockInfoElement(QWidget *parent) :
  QDockWidget(parent),
  ui(new Ui::QDockInfoElement),
  m_pElement(NULL)
{
  ui->setupUi(this);
}

QDockInfoElement::~QDockInfoElement()
{
  clear();
  delete ui;
}

void QDockInfoElement::clear()
{
  while(!m_vWidget.empty())
    {
      ui->layout->removeWidget(m_vWidget.back());
      delete m_vWidget.back();
      m_vWidget.pop_back();
    }
}

void QDockInfoElement::display(SemanticElement* e)
{
  clear();
  m_pElement=e;
  ui->spacer->changeSize(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  if(isHidden() && e)
      emit isShown(true);
  else if(!isHidden() && !e)
      emit isShown(false);

  if(!e)
    return;

  if(TYPE_ELEMENT(e->type())==SemanticElement::EDGE)
    {
      QInfoEdge* edge = new QInfoEdge(this);
      ui->layout->insertWidget(ui->layout->count()-1,edge);
      m_vWidget.push_back(edge);
      edge->display((Edge*) e);
    }

  else if(TYPE_SUBNODE(e->type())==SubNode::DECORATOR)
    {
      QInfoDecorator* decorator = new QInfoDecorator(this);
      ui->layout->insertWidget(ui->layout->count()-1,decorator);
      m_vWidget.push_back(decorator);
      decorator->display((Decorator*) e);
    }

  if(TYPE_NODE(e->type())==Node::ENTITY)
    {
      ui->spacer->changeSize(20, 40, QSizePolicy::Minimum, QSizePolicy::Ignored);
      QInfoEntity* entity = new QInfoEntity(this);
      ui->layout->insertWidget(ui->layout->count()-1,entity);
      m_vWidget.push_back(entity);
      entity->display((Entity*) e);
    }

  if(e->pNumerical)
    {
      QInfoNumerical* numerical = new QInfoNumerical(this);
      ui->layout->insertWidget(ui->layout->count()-1,numerical);
      m_vWidget.push_back(numerical);
      numerical->display(e->pNumerical);
    }

  if(e->pConcrete)
    {
      QInfoConcrete* Concrete = new QInfoConcrete(this);
      ui->layout->insertWidget(ui->layout->count()-1,Concrete);
      m_vWidget.push_back(Concrete);

      Concrete->display(e->pConcrete);
    }

  if(TYPE_NODE(e->type())==Node::ENTITY)
    {
      if(TYPE_ENTITY(e->type())==Entity::ACTION)
        {
          QInfoAction* action = new QInfoAction(this);
          ui->layout->insertWidget(ui->layout->count()-1,action);
          m_vWidget.push_back(action);
          action->display((Entity*) e);
        }

      QInfoComposition* composition = new QInfoComposition(this);
      ui->layout->insertWidget(ui->layout->count()-1,composition);
      m_vWidget.push_back(composition);
      composition->display((Entity*) e);
    }
}

void QDockInfoElement::updateInfo()
{
  if(m_pElement)
    display(m_pElement);
}

void QDockInfoElement::hideOrShow()
{
  if(isHidden())
    show();
  else
    hide();
}

void QDockInfoElement::closeEvent(QCloseEvent* e)
{
  e->ignore();
  emit isShown(false);
}


