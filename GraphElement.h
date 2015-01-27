#ifndef GRAPHELEMENT_H
#define GRAPHELEMENT_H

#include <QObject>
#include <QGraphicsItem>
class SemanticElement;

class GraphElement : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

  public:
  //Constructor
    GraphElement() : QObject(),netItem(NULL) {}

    virtual ~GraphElement();

    static QColor getColor(SemanticElement* e);

  //Painting function
    void drawSelectBox(QPainter *painter, const QStyleOptionGraphicsItem *item);

  //Public attributes
    QString label;
    QColor  color;
    SemanticElement* netItem;

  signals:

  public slots:
};
#endif // GRAPHELEMENT_H
