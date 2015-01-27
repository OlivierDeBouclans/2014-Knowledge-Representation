/*
 * GraphElements' Classes
 *
 * This file contain all the graphical items (QGraphicsItem) used for the
 * semantic net's representation.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GRAPHELEMENTS_H
#define GRAPHELEMENTS_H

//Project include
#include <SemanticNet.h>
#include <GraphElement.h>

//Qt include
#include <QGraphicsItem>
#include <QFont>
#include <QObject>

///////////////////////////////////////////////////////////////
//                      Graph Element                        //
///////////////////////////////////////////////////////////////
/**
 * A Graph Element is the graph representation of a semantic element
 */

///////////////////////////////////////////////////////////////
//                      Graph Entity                         //
///////////////////////////////////////////////////////////////
/**GraphEntity
 * Used to represent the different types of entity in the semantic net
**/

static const QRect ENTITY_ELLIPSE_RECT = QRect(-85,-45,170,90);
static const QColor COL_OBJECT = QColor(0,175,84);
static const QColor COL_OBJECT_CONCRETE = QColor(138,207,87);
static const QColor COL_ATTRIBUTE = QColor(0,111,189);
static const QColor COL_ATTRIBUTE_NUMERICAL = QColor(222,184,135);
static const QColor COL_VALUE = QColor(30,144,255);
static const QColor COL_VALUE_NUMERICAL = QColor(255,222,173);
static const QColor COL_ACTION = QColor(255,20,24);
static const QColor COL_ACTION_CONCRETE = QColor(200,15,18);

class GraphEntity : public GraphElement
{
  public:
  //Constructor
    GraphEntity(Entity* entity);

  //Destructor
    ~GraphEntity() {}

  //QT graphicsItem enum type
    enum { Type = UserType + 1};

  //Pure virtual function
    QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

  //Virtual function
    virtual QPainterPath shape() const;
    int type() const { return Type; }
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

    inline Entity* getEntity() const { return (Entity*) netItem;}

  //Setter
    void   setColor();
};

class GraphAction : public GraphEntity
{
  public:
  //Constructor
    GraphAction(Entity* entity) : GraphEntity(entity) {}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
};

///////////////////////////////////////////////////////////////
//                     Graph SubNode                         //
///////////////////////////////////////////////////////////////

class GraphSubNode: public GraphElement
{
  public:
  //Constructor
    GraphSubNode(SubNode* n);

  //Destructor
    virtual ~GraphSubNode();

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void updateLine();

    virtual QRectF boundingRect() const=0;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)=0;

  //Attribute
    QGraphicsLineItem* pLine;
    SubNode* pSubNode;
};

///////////////////////////////////////////////////////////////
//                   Graph Decorator                         //
///////////////////////////////////////////////////////////////
/**
 * Used to represent decorator of edges for a specific entity
**/

static const QRect  DECORATOR_RECT = QRect(-20,-20,40,37);
static const QColor DECORATOR_COL  = Qt::gray;

class GraphDecorator : public GraphSubNode
{
  public:
  //Constructor
    GraphDecorator(Decorator *decorator);

  //QT graphicsItem enum type
    enum { Type = UserType + 2};

  //Pure virtual function
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

  //Virtual functions
    QPainterPath shape() const;
    int type() const { return Type; }

    Decorator* getDecorator() {return (Decorator*) netItem;}
    void changeOwner(Node*);

  private:
    QPolygonF          m_Triangle;
};


///////////////////////////////////////////////////////////////
//                   Graph Terminal                          //
///////////////////////////////////////////////////////////////

static const QRect  TERMINAL_RECT = QRect(-65,-35,130,70);
static const QColor TERMINAL_COL  = Qt::gray;

class GraphTerminal : public  GraphSubNode
{
  public:
  //Constructor
    GraphTerminal(Terminal *terminal);

  //QT graphicsItem enum type
    enum { Type = UserType + 3};

  //Pure virtual function
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

  //Virtual functions
    QPainterPath shape() const;
    int type() const { return Type; }

    inline Terminal* getDecorator() {return (Terminal*) netItem;}

    QRectF rect;
    QFont font;
};

///////////////////////////////////////////////////////////////
//                       Graph Edge                          //
///////////////////////////////////////////////////////////////
/**
 * Used to draw an arrow between 2 graph Entities
**/

//Size of the triangle at the tip of the arrow
static const qreal ARROW_SIZE = 12;

class GraphEdge : public GraphElement
{
  public:
  //Constructor
    GraphEdge(Edge *edge, QColor color = Qt::black);

  //Destructor
    ~GraphEdge();

  //QT graphicsItem enum type
    enum { Type = UserType + 4};

  //Pure virtual function
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    int type() const { return Type; }
    QPainterPath shape() const;

    void updatePosition();

    Edge* getEdge() const {return (Edge*) netItem;}

  //Getter
    QGraphicsItem *From() const;
    QGraphicsItem *To() const;
    QPolygonF ArrowHead;
    QLineF Line;

    QFont font;
    QRectF rect;
};


#endif // GRAPHELEMENTS_H
