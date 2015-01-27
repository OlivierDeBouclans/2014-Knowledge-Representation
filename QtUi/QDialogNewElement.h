#ifndef QDIALOGNEWELEMENT_H
#define QDIALOGNEWELEMENT_H

#include <QDialog>

class QNewEntity;
class QNewNumerical;
class QNewAction;
class QNewPhysical;
class QNewEdge;
class QNewDecorator;

class Entity;
class Edge;
class Decorator;

class SemanticElement;
class Node;

namespace Ui {
  class QDialogNewElement;
}

class QDialogNewElement : public QDialog
{
    Q_OBJECT

  public:
    enum Type { ENTITY, EDGE, DECORATOR};

    explicit QDialogNewElement(Type type,QWidget *parent = 0);
    explicit QDialogNewElement(SemanticElement* e, QWidget *parent);
    ~QDialogNewElement();

    void newEntity();
    void newEdge();
    void newDecorator();

    void put(Entity*);
    void put(Edge*);
    void put(Decorator*);

    Entity* getEntity();
    Edge* getEdge(Node* from, Node* to);
    Decorator* getDecorator(Node* owner);

    void edit();
      void setEntity();
      void setEdge();
      void setDecorator();

    void wheelEvent(QWheelEvent *e);

  public slots:
    void newNumerical(int);
    void newPhysical(int);
    void newAction(bool);

  private:
    Ui::QDialogNewElement *ui;

    QNewEntity*    m_pEntity;
    QNewNumerical* m_pNumerical;
    QNewAction*    m_pAction;
    QNewPhysical*  m_pPhysical;

    QNewEdge*      m_pEdge;
    QNewDecorator* m_pDecorator;

    SemanticElement* m_pElement;
};

#endif // QDIALOGNEWELEMENT_H
