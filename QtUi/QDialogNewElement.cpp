#include "QDialogNewElement.h"
#include "ui_QDialogNewElement.h"

#include "QtUi/New/QNewEntity.h"
#include "QtUi/New/QNewNumerical.h"
#include "QtUi/New/QNewAction.h"
#include "QtUi/New/QNewPhysical.h"
#include "QtUi/New/QNewEdge.h"
#include "QtUi/New/QNewDecorator.h"
#include "SemanticNet.h"

#include "qlistwidget.h"
#include "QWheelEvent"

#include "assert.h"


QDialogNewElement::QDialogNewElement(Type type, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::QDialogNewElement),
  m_pEntity(NULL),
  m_pNumerical(NULL),
  m_pAction(NULL),
  m_pPhysical(NULL),
  m_pEdge(NULL),
  m_pDecorator(NULL),
  m_pElement(NULL)
{
  ui->setupUi(this);

  if(type == ENTITY)
    newEntity();
  else if(type == EDGE)
    newEdge();
  else if(type == DECORATOR)
    newDecorator();
}

QDialogNewElement::QDialogNewElement(SemanticElement* e, QWidget* parent):
  QDialog(parent),
  ui(new Ui::QDialogNewElement),
  m_pEntity(NULL),
  m_pNumerical(NULL),
  m_pAction(NULL),
  m_pPhysical(NULL),
  m_pEdge(NULL),
  m_pDecorator(NULL),
  m_pElement(e)
{
  ui->setupUi(this);

  if(TYPE_ELEMENT(e->type())==SemanticElement::EDGE)
    {
      newEdge();
      put((Edge*) e);
    }

  else if(TYPE_NODE(e->type())==Node::ENTITY)
    {
      newEntity();
      put((Entity*) e);
    }
  else if(TYPE_SUBNODE(e->type())==SubNode::DECORATOR)
    {
      newDecorator();
      put((Decorator*) e);
    }
}

QDialogNewElement::~QDialogNewElement()
{
  delete m_pEntity;
  delete m_pNumerical;
  delete m_pAction;
  delete m_pPhysical;
  delete m_pEdge;
  delete m_pDecorator;
  delete ui;
}

void QDialogNewElement::newEntity()
{
  m_pEntity = new QNewEntity(this);
  ui->CentralLayout->addWidget(m_pEntity);

  connect(m_pEntity, SIGNAL(showNumerical(int)),
          this,      SLOT(newNumerical(int)));

  connect(m_pEntity, SIGNAL(showAction(bool)),
          this,      SLOT(newAction(bool)));

  connect(m_pEntity, SIGNAL(showPhysical(int)),
          this,      SLOT(newPhysical(int)));
}

void QDialogNewElement::newEdge()
{
  m_pEdge = new QNewEdge(this);
  ui->CentralLayout->addWidget(m_pEdge);

  connect(m_pEdge, SIGNAL(showPhysical(int)),
          this,      SLOT(newPhysical(int)));
}

void QDialogNewElement::newDecorator()
{
  m_pDecorator = new QNewDecorator(this);
  ui->CentralLayout->addWidget(m_pDecorator);
}

void QDialogNewElement::put(Entity* e)
{
  m_pEntity->setName(e->sLabel);
  m_pEntity->setType(e->type());

  if(TYPE_ENTITY(e->type())==Entity::ACTION)
    {
      QListWidget l;
      vector<SubNode*> sub=e->getSubNode();
      for(unsigned int i=0; i<sub.size(); i++)
        if(TYPE_SUBNODE(sub[i]->type())==SubNode::TERMINAL)
          l.addItem(QString::fromStdString(sub[i]->sLabel));

      m_pAction->setParameter(l);
    }

  if(e->pNumerical)
    {
      m_pEntity->setNumerical(true);
      m_pNumerical->setMin(e->pNumerical->dMin);
      m_pNumerical->setMax(e->pNumerical->dMax);
    }

  if(e->pConcrete)
    {
      m_pEntity->setPhysical(true);
      m_pPhysical->setStart(e->pConcrete->iStartTime);
      m_pPhysical->setEnd(e->pConcrete->iEndTime);
    }
}

void QDialogNewElement::put(Edge* e)
{
  m_pEdge->setType(e->type());
  m_pEdge->setCard(e->iCard);

  if(e->pConcrete)
    {
      m_pEdge->setPhysical(true);
      m_pPhysical->setStart(e->pConcrete->iStartTime);
      m_pPhysical->setEnd(e->pConcrete->iEndTime);
    }
}

void QDialogNewElement::put(Decorator* d)
{
  m_pDecorator->setType(d->type());
  m_pDecorator->setPartition(d->bPartition);
}

Entity* QDialogNewElement::getEntity()
{
  if(!m_pEntity)
    return NULL;

  string name = m_pEntity->getName();
  Entity::Type type = (Entity::Type) m_pEntity->getType();
  Entity* e=new Entity(name,type);

  if(m_pAction)
    {
      QListWidget *l=m_pAction->getParameter();
      for(int i=0; i<l->count(); i++)
        e->add(new Terminal(l->item(i)->text().toStdString(),e));
    }

  if(m_pNumerical)
    {
      double min = m_pNumerical->getMin();
      double max = m_pNumerical->getMax();
      e->setNumerical(min, max);
    }

  if(m_pPhysical)
    {
      __int64 start= m_pPhysical->getStart();
      __int64 end =  m_pPhysical->getEnd();
      e->setConcrete(start, end);
    }

  return e;
}

Edge* QDialogNewElement::getEdge(Node* from, Node* to)
{
  if(!m_pEdge)
    return NULL;

  Edge::Type type = (Edge::Type) m_pEdge->getType();
  Edge* e=Edge::newEdge(from,type,to);
  e->iCard=m_pEdge->getCard();

  if(m_pPhysical)
    {
      __int64 start= m_pPhysical->getStart();
      __int64 end =  m_pPhysical->getEnd();
      e->setConcrete(start, end);
    }

  return e;
}

Decorator* QDialogNewElement::getDecorator(Node* owner)
{
  if(!m_pDecorator)
    return NULL;

  Decorator::Type type = (Decorator::Type) m_pDecorator->getType();
  bool partition = m_pDecorator->getPartition();
  Decorator *d= new Decorator(owner,type,partition);

  return d;
}

void QDialogNewElement::edit()
{
  if(TYPE_ELEMENT(m_pElement->type())==SemanticElement::EDGE)
    setEdge();
  else if(TYPE_NODE(m_pElement->type())==Node::ENTITY)
    setEntity();
  else if(TYPE_SUBNODE(m_pElement->type())==SubNode::DECORATOR)
    setDecorator();
}

void QDialogNewElement::setEntity()
{
  *((Entity*) m_pElement) = *getEntity();
}

void QDialogNewElement::setEdge()
{
  Edge* t= (Edge*) m_pElement;
  *t = *getEdge(t->from,t->to);
}

void QDialogNewElement::setDecorator()
{
  Decorator* d= (Decorator*) m_pElement;
  *d = *getDecorator(d->pOwner);
}

void QDialogNewElement::wheelEvent(QWheelEvent* e)
{
  if(m_pEntity)
    m_pEntity->wheelEvent(e);
}

void QDialogNewElement::newNumerical(int i)
{
  if(i)
    {
      m_pNumerical = new QNewNumerical(this);
      ui->CentralLayout->addWidget(m_pNumerical);
    }
  else
    {
      ui->CentralLayout->removeWidget(m_pNumerical);
      delete m_pNumerical;
      m_pNumerical = NULL;
    }

  adjustSize();
}

void QDialogNewElement::newPhysical(int i)
{
  if(i)
    {
      m_pPhysical = new QNewPhysical(this);
      ui->CentralLayout->addWidget(m_pPhysical);
    }
  else
    {
      ui->CentralLayout->removeWidget(m_pPhysical);
      delete m_pPhysical;
      m_pPhysical = NULL;
    }

  adjustSize();
}

void QDialogNewElement::newAction(bool i)
{
  if(i)
    {
      m_pAction = new QNewAction(this);
      ui->CentralLayout->addWidget(m_pAction);
    }
  else
    {
      ui->CentralLayout->removeWidget(m_pAction);
      delete m_pAction;
      m_pAction = NULL;
    }

  adjustSize();
}
