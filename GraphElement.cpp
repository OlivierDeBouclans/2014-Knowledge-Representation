#include "GraphElement.h"

#include "SemanticNet.h"
#include "GraphElements.h"
#include <qpainter.h>
#include <QStyle>
#include <qstyleoption.h>
#include <QGraphicsScene>

void GraphElement::drawSelectBox(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
  if(option->state & QStyle::State_Selected)
    {
      painter->save();

      painter->setPen(Qt::DashLine);
      painter->setBrush(Qt::NoBrush);
      painter->drawRect(boundingRect());

      painter->restore();
    }
}


GraphElement::~GraphElement()
{
  if(scene())
    scene()->removeItem(this);

  if(netItem)
    netItem->pGraphItem=NULL;
}

QColor GraphElement::getColor(SemanticElement* e)
{
  if(!e || TYPE_NODE(e->type())!=Node::ENTITY)
    return Qt::gray;

  switch (TYPE_ENTITY(e->type()))
    {
      case Entity::OBJECT:
        if(e->pConcrete)
          return COL_OBJECT_CONCRETE;
        else
          return COL_OBJECT;
      break;

      case Entity::ATTRIBUTE:
        if(e->pNumerical)
          return COL_ATTRIBUTE_NUMERICAL;
       else
          return COL_ATTRIBUTE;
      break;

      case Entity::VALUE:
        if(e->pNumerical)
          return COL_VALUE_NUMERICAL;
        else
          return COL_VALUE;
      break;

      case Entity::ACTION:
        if(e->pConcrete)
          return COL_ACTION_CONCRETE;
        else
          return COL_ACTION;
      break;

      default:
        return Qt::gray;
      break;
    }
}
