
//Project include
#include "GraphElements.h"

//STD include
#include "math.h"
#include <iostream>
#include <assert.h>

//Qt include
#include <QtWidgets>

///////////////////////////////////////////////////////////////
//                      Graph Element                        //
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
//                      Graph Entity                         //
///////////////////////////////////////////////////////////////
/**
 * @brief GraphEntity::GraphEntity
 * @param entity : entity to represent
 */
GraphEntity::GraphEntity(Entity *entity)
{
  netItem     = entity;
  label = QString::fromStdString(entity->sLabel);

  setColor();

  setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
  setAcceptHoverEvents(true);
  setZValue(1);
}

/**
 * @brief GraphEntity::boundingRect
 * @return bounding rectangle of the graphic item
 */
QRectF GraphEntity::boundingRect() const
{
    return ENTITY_ELLIPSE_RECT;
}

/**
 * @brief GraphEntity::paint
 * @param painter
 * @param option
 * @param widget
 */
void GraphEntity::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

  Q_UNUSED(widget);

  //Save the current painter's state...
  painter->save();

  //Change the color if the item is selected or has the mouse over it
  QColor col = color;
  if(option->state & QStyle::State_MouseOver)
    col = col.lighter(110);

    QBrush b(col);
    QPen   p(Qt::black);

    painter->setBrush(b);
    painter->setPen(p);

    painter->drawEllipse(ENTITY_ELLIPSE_RECT);

    QFont font("Times", 12);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(ENTITY_ELLIPSE_RECT, Qt::AlignCenter | Qt::TextWordWrap, label);

    drawSelectBox(painter,option);

  //...and then restore its previous state after use
  painter->restore();
}

/**
 * @brief GraphEntity::shape
 * @return Precise shape of the graphic item
 */
QPainterPath GraphEntity::shape() const
{
  QPainterPath p;
  p.addEllipse(ENTITY_ELLIPSE_RECT);

  return p;
}

QVariant GraphEntity::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  if(change == QGraphicsItem::ItemPositionHasChanged)
    {
      Entity *e=getEntity();
      vector<SubNode*> l=e->getSubNode();
      for(unsigned int i=0; i<l.size(); i++)
        if(GraphSubNode *t = l[i]->getGraph())
            t->updateLine();

      vector<Edge*> edges=e->getInEdge();
      for(unsigned int i=0; i<edges.size(); i++)
        if(GraphEdge *t = edges[i]->getGraph())
            t->updatePosition();

      edges=e->getOutEdge();
      for(unsigned int i=0; i<edges.size(); i++)
        if(GraphEdge *t = edges[i]->getGraph())
            t->updatePosition();
    }

  return value;
}

/**
 * @brief GraphEntity::setColor
 */
void GraphEntity::setColor()
{
  Entity *e = getEntity();

  switch (TYPE_ENTITY(e->type()))
    {
      case Entity::OBJECT:
        if(e->pConcrete)
          color = COL_OBJECT_CONCRETE;
        else
          color = COL_OBJECT;
      break;

      case Entity::ATTRIBUTE:
        if(e->pNumerical)
        color = COL_ATTRIBUTE_NUMERICAL;
       else
        color = COL_ATTRIBUTE;
      break;

      case Entity::VALUE:
        if(e->pNumerical)
          color = COL_VALUE_NUMERICAL;
        else
          color = COL_VALUE;
      break;

      case Entity::ACTION:
        if(e->pConcrete)
          color = COL_ACTION_CONCRETE;
        else
          color = COL_ACTION;
      break;

      default:
        color = Qt::gray;
      break;
    }
}

///////////////////////////////////////////////////////////////
//                     Graph SubNode                         //
///////////////////////////////////////////////////////////////

GraphSubNode::GraphSubNode(SubNode* n): pSubNode(n)
{
  assert(n);
  assert(n->pOwner);
  assert(n->pOwner->pGraphItem);

  pLine = new QGraphicsLineItem();
  pLine->setPen(n->pOwner->pGraphItem->color);
  updateLine();

  setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
  setAcceptHoverEvents(true);
  setZValue(2);
}

GraphSubNode::~GraphSubNode()
{
  delete pLine;
}

void GraphSubNode::updateLine()
{
  GraphElement *parent = pSubNode->pOwner->pGraphItem;
  QPointF p1= parent->pos()+ENTITY_ELLIPSE_RECT.center();
  QPointF p2= pos();

  pLine->setLine(QLine(p1.toPoint(),p2.toPoint()));
  pLine->update();
}

QVariant GraphSubNode::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  if( change == QGraphicsItem::ItemPositionHasChanged)
    {
      if(pLine)
        updateLine();

      vector<SubNode*> l=pSubNode->getSubNode();
      for(unsigned int i=0; i<l.size(); i++)
        if(GraphSubNode *t = l[i]->getGraph())
            t->updateLine();

      vector<Edge*> edges=pSubNode->getInEdge();
      for(unsigned int i=0; i<edges.size(); i++)
        if(GraphEdge *t = edges[i]->getGraph())
            t->updatePosition();

      edges=pSubNode->getOutEdge();
      for(unsigned int i=0; i<edges.size(); i++)
        if(GraphEdge *t = edges[i]->getGraph())
            t->updatePosition();
    }

  return value;
}


///////////////////////////////////////////////////////////////
//                   Graph Decorator                         //
///////////////////////////////////////////////////////////////

GraphDecorator::GraphDecorator(Decorator *decorator): GraphSubNode(decorator)
{
  netItem     = decorator;
  label  = QString::fromStdString(netItem->sLabel);
  color  = DECORATOR_COL;

  //Creation of the triangle
  qreal y= DECORATOR_RECT.height();
  qreal x= DECORATOR_RECT.width() / 2;

  if(!decorator->pParent)
    {
      QPen p=pLine->pen();
      p.setWidthF(1.75);
      pLine->setPen(p);
    }

  m_Triangle.clear();
  m_Triangle << QPointF(0,0) << QPointF(x,y) << QPointF(-x,y);
  m_Triangle.translate(0,-DECORATOR_RECT.width()/2);
}

void GraphDecorator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(widget);

  //Save the current painter's state...
  painter->save();

  //Change the color if the item is selected or has the mouse over it
  QColor col = color;
  if(option->state & QStyle::State_MouseOver)
    col = col.lighter(110);

    QBrush b(col);
    QPen   p(Qt::black);

    if(!getDecorator()->pParent)
      p.setWidthF(1.75);

    painter->setBrush(b);
    painter->setPen(p);

    painter->drawPolygon(m_Triangle);

    QFont font("Times", 8);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(DECORATOR_RECT, Qt::AlignCenter, label);

    if(getDecorator()->pParent && (option->state & QStyle::State_MouseOver))
      {
        font.setPointSize(13);
        painter->setFont(font);
        QFontMetrics fm(font);
        QRectF rect2=QRectF(DECORATOR_RECT.topLeft().x(),DECORATOR_RECT.topLeft().y(),fm.width(QString::fromStdString(getDecorator()->pParent->sLabel))+p.width()+5,fm.height());
        rect2.translate(0,-rect2.height());
        painter->setBrush(GraphElement::getColor(getDecorator()->pParent));
        painter->drawRect(rect2);
        painter->drawText(rect2, Qt::AlignCenter, QString::fromStdString(getDecorator()->pParent->sLabel));
      }

    drawSelectBox(painter,option);

  //...and then restore its previous state after use
    painter->restore();
}

QRectF GraphDecorator::boundingRect() const
{
  return DECORATOR_RECT;
}

QPainterPath GraphDecorator::shape() const
{
  QPainterPath p;
  p.addPolygon(m_Triangle);

  return p;
}

void GraphDecorator::changeOwner(Node* n)
{
  pLine->setPen(GraphElement::getColor(n));

  if(!getDecorator()->pParent)
    {
      QPen p=pLine->pen();
      p.setWidthF(1.75);
      pLine->setPen(p);
    }

  updateLine();
}

///////////////////////////////////////////////////////////////
//                   Graph Terminal                          //
///////////////////////////////////////////////////////////////

GraphTerminal::GraphTerminal (Terminal  *terminal): GraphSubNode(terminal)
{
  netItem     = terminal;
  label  = QString::fromStdString(netItem->sLabel);
  color  = TERMINAL_COL;

  font =QFont ("Times", 13);
  font.setBold(true);

  QFontMetrics fm(font);
  QPen p;

  rect = QRectF(0,0,fm.width(label)+p.width()+5,fm.height());
  rect.translate(-rect.width()/2,-rect.height()/2);

}

void GraphTerminal::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(widget);

  //Save the current painter's state...
  painter->save();

  //Change the color if the item is selected or has the mouse over it
  QColor col = color;
  if(option->state & QStyle::State_MouseOver)
    col = col.lighter(110);

    QBrush b(col);
    QPen   p(Qt::black);

    painter->setBrush(b);
    painter->setPen(Qt::black);

    painter->drawRect(rect);

    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, label);

    drawSelectBox(painter,option);

  //...and then restore its previous state after use
    painter->restore();
}

QRectF GraphTerminal::boundingRect() const
{
  return rect;
}

QPainterPath GraphTerminal::shape() const
{
  QPainterPath p;
  p.addRect(rect);

  return p;
}


///////////////////////////////////////////////////////////////
//                       Graph Edge                          //
///////////////////////////////////////////////////////////////
/**
 * @brief GraphEdge::GraphEdge
 * @param edge
 * @param Color
 */
GraphEdge::GraphEdge(Edge* edge, QColor Color)
{
  netItem = edge;
  color = Color;
  label = QString::fromStdString(edge->sLabel);

  updatePosition();

  font =QFont ("Times", 13);
  font.setBold(true);

  setFlags(ItemIsSelectable);
  setAcceptHoverEvents(true);
  setZValue(4);
}

GraphEdge::~GraphEdge()
{
}

/**
 * @brief GraphEdge::boundingRect
 * @return bounding rectangle of the graphic item
 */
QRectF GraphEdge::boundingRect() const
{
    qreal extra = ARROW_SIZE/2;
    qreal xextra = label.size()*13;

    QPointF p1 = mapFromScene(From()->scenePos());
    QPointF p2 = mapFromScene(To()->scenePos());

    QRectF r(p1, QSizeF(p2.x() - p1.x(),
                        p2.y() - p1.y()));
    r=r.normalized();
    r=r.adjusted(-extra, -extra, extra, extra);

    if(r.width() < xextra)
      {
        xextra = (xextra-r.width())/2;
        return r.adjusted(-xextra, 0, xextra,0);
      }

    return r;
}

/**
 * @brief GraphEdge::paint
 * @param painter
 * @param option
 * @param widget
 */
void GraphEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(widget);

  if(From()->collidesWithItem(To()))
    return;

  //Save the current painter's state...
  painter->save();

  //painter->drawRect(boundingRect());

    QPen p(Qt::black);
    QBrush b(color);
    p.setJoinStyle(Qt::MiterJoin);

    painter->setPen(p);
    painter->setBrush(Qt::black);

    if(!getEdge()->pParent)
      p.setWidthF(2);

    if( option->state & QStyle::State_Selected)
        p.setStyle(Qt::DashLine);

    painter->setBrush(b);
    painter->setPen(p);

    //Calucluate the start and end point
      QLineF centerLine(From()->pos(), To()->pos());

      QPolygonF FromPolygon = From()->shape().toFillPolygon();
      QPolygonF ToPolygon   = To()  ->shape().toFillPolygon();
      QPointF intersectPoint1, intersectPoint2;

      //Start point
      QPointF p2, p1 = FromPolygon.first() + From()->pos();
      for (int i = 1; i < FromPolygon.count(); ++i)
        {
          p2 = FromPolygon.at(i) + From()->pos();
          QLineF polyLine(p1, p2);
          QLineF::IntersectType intersectType = polyLine.intersect(centerLine, &intersectPoint1);

          if (intersectType == QLineF::BoundedIntersection)
              break;

          p1 = p2;
        }

      //End point
      p1 = ToPolygon.first() + To()->pos();
      for (int i = 1; i < ToPolygon.count(); ++i)
        {
          p2 = ToPolygon.at(i) + To()->pos();
          QLineF polyLine(p1, p2);
          QLineF::IntersectType intersectType = polyLine.intersect(centerLine, &intersectPoint2);

          if (intersectType == QLineF::BoundedIntersection)
              break;

          p1 = p2;
        }

    QPointF lp1=intersectPoint2-QPointF(centerLine.unitVector().dx()*2,centerLine.unitVector().dy()*2);
    QPointF lp2=intersectPoint1+QPointF(centerLine.unitVector().dx(),centerLine.unitVector().dy());

    Line=QLineF(mapFromScene(lp1), mapFromScene(lp2));
    painter->drawLine(Line);


    double angle = acos(Line.dx() / Line.length());
    if (Line.dy() >= 0)
      angle = (M_PI * 2) - angle;

    QPointF arrowP1 = mapFromScene(intersectPoint2) + QPointF(sin(angle + M_PI / 3) * ARROW_SIZE,
                                            cos(angle + M_PI / 3) * ARROW_SIZE);
    QPointF arrowP2 = mapFromScene(intersectPoint2) + QPointF(sin(angle + M_PI - M_PI / 3) * ARROW_SIZE,
                                            cos(angle + M_PI - M_PI / 3) * ARROW_SIZE);

    ArrowHead.clear();
    ArrowHead << mapFromScene(intersectPoint2) << arrowP1 << arrowP2;
    painter->drawPolygon(ArrowHead);

    painter->setBrush(Qt::white);

    QFontMetrics fm(font);
    rect = QRectF(0,0,fm.width(label)+p.width()+5,fm.height());
    rect.translate(-rect.width()/2,-rect.height()/2);
    rect.translate(QGraphicsLineItem(Line).boundingRect().center());

    painter->drawRect(rect);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, label);

    if(getEdge()->pParent && (option->state & QStyle::State_MouseOver))
      {
        QRectF rect2=QRectF(rect.topLeft().x(),rect.topLeft().y(),fm.width(QString::fromStdString(getEdge()->pParent->sLabel))+p.width()+5,fm.height());
        rect2.translate(0,-rect.height());
        painter->setBrush(GraphElement::getColor(getEdge()->pParent));
        painter->drawRect(rect2);
        painter->drawText(rect2, Qt::AlignCenter, QString::fromStdString(getEdge()->pParent->sLabel));
      }

  //...and then restore its previous state after use
    painter->restore();
}

QPainterPath GraphEdge::shape() const
{
  QPainterPath p;

  p.addPath(QGraphicsLineItem(Line).shape());
  p.addPolygon(ArrowHead);
  p.addRect(rect);

  return p;
}

void GraphEdge::updatePosition()
{
  setPos((To()->scenePos()+From()->scenePos())/2);
}

/**
 * @brief GraphEdge::From
 * @return the QGraphicsItem corresponding to from
 */
QGraphicsItem *GraphEdge::From() const
{
  return getEdge()->from->pGraphItem;
}

/**
 * @brief GraphEdge::To
 * @return the QGraphicsItem corresponding to to
 */
QGraphicsItem *GraphEdge::To() const
{
  return getEdge()->to->pGraphItem;
}



void GraphAction::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

  Q_UNUSED(widget);

  //Save the current painter's state...
  painter->save();

  //Change the color if the item is selected or has the mouse over it
  QColor col = color;
  if(option->state & QStyle::State_MouseOver)
    col = col.lighter(110);

    QBrush b(col);
    QPen   p(Qt::black);

    painter->setBrush(b);
    painter->setPen(p);

    painter->drawRoundedRect(ENTITY_ELLIPSE_RECT,ENTITY_ELLIPSE_RECT.height()/5,ENTITY_ELLIPSE_RECT.height()/5);

    QFont font("Times", 12);
    painter->setFont(font);
    painter->drawText(ENTITY_ELLIPSE_RECT, Qt::AlignCenter | Qt::TextWordWrap, label);

    drawSelectBox(painter,option);

  //...and then restore its previous state after use
    painter->restore();
}

QPainterPath GraphAction::shape() const
{
  QPainterPath p;
  p.addRoundedRect(ENTITY_ELLIPSE_RECT,ENTITY_ELLIPSE_RECT.height()/5,ENTITY_ELLIPSE_RECT.height()/5);

  return p;
}




