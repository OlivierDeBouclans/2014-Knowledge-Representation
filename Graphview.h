/*
 * GraphView
 *
 * This class implement the graphical representation of the semantic net.
 * It's a QGraphicsView possessing the SemanticNet to represent and the
 * QGraphicsScene to be displayed(and on which the semtanticnet's
 * representation will be build).
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

//Qt include
#include <QWheelEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTime>
#include <QTreeWidgetItem>
#include <QParallelAnimationGroup>

class Entity;
class Decorator;
class Edge;
class Terminal;
class Node;
class SubNode;
class SemanticNet;
class SemanticElement;

class GraphEntity;
class GraphElement;
class GraphDecorator;
class GraphEdge;
class GraphSubNode;

class MainWindow;

class QState;
class QStateMachine;

static const int ANIMATION_DURATION=450;

class GraphAnimation : public QParallelAnimationGroup
{
  public:
    GraphAnimation(): QParallelAnimationGroup() {}

    void assignProperty(GraphElement* item, QPointF pos);
    void start();
};

class GraphView : public QGraphicsView
{
  Q_OBJECT

  public:
  //Constructor
    GraphView(SemanticNet* semanticNet, MainWindow *parent);

  //Destructor
    ~GraphView();

    enum ShowEdge{ SHOW_IN, SHOW_OUT, SHOW_IN_OUT};

  //Comunication with the information window
  signals:
    void informationOn(SemanticElement *e);
    void select(Entity* e);
    void selectPOV(Entity* e);
    void netHasChanged();
    void graphHasChanged();

  public slots:
  //Scene editting functions
    //Creation
      void newElement();
        void newEntity();
        void newDecorator();
        void newEdge(GraphElement *from, GraphElement *to);
        void newEdge();

      void editElement();

    //Deletion
      void deleteElementFromGraph();
        void deleteGraph(GraphEdge* item, bool blacklist=false);
        void deleteGraph(GraphSubNode* item, bool blacklist=false);
        void deleteGraph(GraphEntity* item, bool blacklist=false);

      void deleteElementFromNet();
        void deleteNet(GraphEdge* item);
        void deleteNet(GraphSubNode* item);
        void deleteNet(GraphEntity* item);

  //Scene representation
    void populateScene();
    void clearScene();

    void reArrangeSceneCircle();
    void reArrangeSceneSquare();
    void reArrangeSceneAll(bool circle);
    void reArrangeSelection(bool circle);
      void reArrange_Circle(std::vector<GraphEntity*>,QPoint);
      void reArrange_Square(std::vector<GraphEntity*>,QPoint);
    void reArrangeEntity(GraphEntity *e);
    void reArrangeEntity(GraphEntity *e, QPointF pos);

    void addOnGraph(Edge *e);
    void addOnGraph(SubNode *e, QPointF pos, bool all=true);
      void addOnGraph(SubNode *e, bool all=true);
    void addOnGraph(Entity *e, QPointF pos, bool all=true);
      void addOnGraph(Entity *e, bool all=true);
      void addOnGraphAllEdge(Node*);
      void addOnGraphAllSubNode(Node*);
      void addOnGraphAllSubNode(Node*, QPointF);

    void getInfo();
    void getInfo(QGraphicsItem*item);
    void selectElement(QGraphicsItem* item);
    void selectPointOfView(QGraphicsItem* item);
    void updateAction();

    void showIn();
    void showOut();
    void showInOut();

    void adaptView(Entity* parent,ShowEdge s=SHOW_OUT);
      void adaptView(Node* n, std::vector<Entity*> h,ShowEdge);
      void adaptView(Entity* n, std::vector<Entity*> h,ShowEdge);
      void adaptView(SubNode* n, std::vector<Entity*> h,ShowEdge);
      void adaptView(Edge* e, std::vector<Entity*> h,ShowEdge);

    void bringFront();
    void bringBack();

  protected:
  //Event handling
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

    void keyPressEvent(QKeyEvent *e);

    void paintEvent(QPaintEvent *e);
    void contextMenuEvent(QContextMenuEvent *event);

  private:
  //Attribute
    QGraphicsScene m_Scene; //Scene on which build the representation
    SemanticNet*   m_pSemanticNet;
    const MainWindow* pMainWindow;
    Entity* m_pSelectedElement;
    Entity* m_pPointOfView;

    QPoint         m_MousePos;
    QGraphicsItem* pTo;
    int m_frameCount;
    QTime m_time;
    QGraphicsTextItem *m_fps;

    bool bFromNode;
    bool bFromEdges;
    bool bFromDecorators;

    int iMaxZValue;
    int iMinZValue;

    /************TODO***********/
public:
    void selectDecorator(GraphEntity *e);
};

#endif // SEMANTICVIEW_H


































