#include "GraphView.h"
#include "ui_GraphView.h"

//Project include
#include "QtUi/New/QNewEntity.h"
#include "QtUi/QDialogNewElement.h"

#include "SemanticNet.h"
#include "GraphElements.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

//Qt include
#include <qmessagebox.h>
#include <qlayout.h>
#include <qmenu.h>
#include <qscrollbar.h>
#include <qstate.h>
#include <QtWidgets>

//STD include
#include <assert.h>
#include <math.h>

/**
 * @brief GraphView::GraphView
 * @param semanticNet
 * @param parent
 */
GraphView::GraphView(SemanticNet* semanticNet, MainWindow *parent) :
           QGraphicsView(parent),
           m_pSemanticNet(semanticNet),
           pMainWindow(parent),
           m_MousePos(0,0)
{
  //Scene creation
  m_Scene.setSceneRect(-5000,-5000,10000,10000);
  setScene(&m_Scene);

  connect(&m_Scene,SIGNAL(selectionChanged()),
          this, SLOT(updateAction()));

  m_fps = new QGraphicsTextItem("FPS: x");
  m_fps->setFlag(QGraphicsItem::ItemIsMovable);
  m_fps->setFont(QFont("Times",20));
  m_fps->setZValue(100);
  m_frameCount = 0;
  iMaxZValue=4;
  iMinZValue=0;

  pTo=NULL;
  m_pSelectedElement=NULL;

  //QGraphicsView settings
  setDragMode(QGraphicsView::RubberBandDrag);
  setRenderHint(QPainter::Antialiasing);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setMouseTracking(true);
}

/**
 * @brief GraphView::~GraphView
 */
GraphView::~GraphView()
{

}

/**
 * @brief GraphView::newElement
 */
void GraphView::newElement()
{
  if(pMainWindow->ui->actionNew_Edge->isEnabled())
    newEdge();
  else if(pMainWindow->ui->actionNew_Decorator->isEnabled())
    newDecorator();
  else if(pMainWindow->ui->actionNew_Entity->isEnabled())
    newEntity();
}

/**
 * @brief GraphView::NewEntity
 */
void GraphView::newEntity()
{  
  QDialogNewElement win(QDialogNewElement::ENTITY,this);
  if(win.exec() == QDialog::Accepted)
    {
      m_Scene.clearSelection();
      Entity* e = win.getEntity();

      if(!e)
        {
          QMessageBox messageBox;
          messageBox.critical(this,"Error","Can't create entity");
          return;
        }

      m_pSemanticNet->add(e);
      emit netHasChanged();

      addOnGraph(e);
      e->pGraphItem->setSelected(true);
      reArrangeEntity((GraphEntity*)(e->pGraphItem));
    }
}

void GraphView::newDecorator()
{
  QList<QGraphicsItem *> selectedItems = m_Scene.selectedItems();

  GraphElement* item=(GraphElement*) selectedItems[0];
  Node* owner;
  if(bFromEdges)
    owner=((Edge*)item->netItem)->from;
  else if(bFromDecorators)
    owner= ((Decorator*)item->netItem)->pOwner;
  else
    owner=(Node*) item->netItem;

  QDialogNewElement win(QDialogNewElement::DECORATOR,this);
  if(win.exec() == QDialog::Accepted)
    {
      Decorator* d = win.getDecorator(owner);

      if(!d)
        {
          QMessageBox messageBox;
          QString error("Unable to create the decorator");
          messageBox.critical(this,"Error",error);
          return;
        }

      if(m_pSelectedElement)
        d->pParent=m_pSelectedElement;

      owner->add(d);
      m_pSemanticNet->add(d);

      emit netHasChanged();
      addOnGraph(d);

      if(bFromEdges)
        {
          for(int i=0; i<selectedItems.size();i++)
            {
              Edge* e=((GraphEdge*)selectedItems[i])->getEdge();
              owner->disconnect(e);
              e->from=d;
              d->add(e);
            }
        }
      else if(bFromDecorators)
        {
          for(int i=0; i<selectedItems.size();i++)
            {
              Decorator* e=((GraphDecorator*)selectedItems[i])->getDecorator();
              owner->remove(e);
              e->pOwner=d;
              d->add(e);
            }
        }

      GraphAnimation* Animation = new GraphAnimation();
      Animation->assignProperty(d->pGraphItem,d->pGraphItem->pos());
      d->pGraphItem->setPos(owner->pGraphItem->pos());
      Animation->start();

      m_Scene.clearSelection();
      d->pGraphItem->setSelected(true);
    }
}

void GraphView::newEdge()
{
  QList<QGraphicsItem *> selectedItems = m_Scene.selectedItems();
  assert(selectedItems.size()==2);

  GraphElement* from = (GraphElement*) selectedItems[0];
  GraphElement* to= (GraphElement*) selectedItems[1];

  newEdge(from,to);
}

/**
 * @brief GraphView::NewEdge
 * @param from
 * @param to
 */
void GraphView::newEdge(GraphElement* from, GraphElement* to)
{

  if( !from || !to || from->type() == GraphEdge::Type || to->type() == GraphEdge::Type)
    {
      QMessageBox messageBox;
      QString error("Wrong selection");
      messageBox.critical(this,"Error",error);
      return;
    }

  QDialogNewElement win(QDialogNewElement::EDGE,this);
  if(win.exec() == QDialog::Accepted)
    {
      m_Scene.clearSelection();
      Edge* e;

      if(pTo == from)
        e=win.getEdge((Node*)to->netItem,(Node*)from->netItem);
      else
        e=win.getEdge((Node*)from->netItem,(Node*)to->netItem);

      pTo=NULL;

      if(m_pSelectedElement && m_pSelectedElement!=from->netItem)
        e->pParent=m_pSelectedElement;

      if(!e)
        {
          QMessageBox messageBox;
          QString error("Unable to create the relation");
          messageBox.critical(this,"Error",error);
          return;
        }

      m_pSemanticNet->add(e);
      emit netHasChanged();

      addOnGraph(e);
      e->from->pGraphItem->setSelected(true);
    }
}

void GraphView::editElement()
{
  SemanticElement* e = NULL;

  if(pTo)
      e=((GraphElement*) pTo)->netItem;
  else if(m_Scene.selectedItems().size() == 1)
      e=((GraphElement*) m_Scene.selectedItems().front())->netItem;

  assert(e);

  if(TYPE_SUBNODE(e->type())==SubNode::TERMINAL)
    return;

  QDialogNewElement win(e,this);
  if(win.exec() == QDialog::Accepted)
    {
      win.edit();

      if(TYPE_ELEMENT(e->type())==SemanticElement::EDGE)
        {
          deleteGraph((GraphEdge*)e->pGraphItem);
          addOnGraph((Edge*)e);
        }
      else if(TYPE_NODE(e->type())==Node::ENTITY)
        {
          deleteGraph((GraphEntity*)e->pGraphItem);
          addOnGraph((Entity*)e);
        }
      else if(TYPE_SUBNODE(e->type())==SubNode::DECORATOR)
        {
          deleteGraph((GraphSubNode*)e->pGraphItem);
          addOnGraph((SubNode*)e);
        }

      emit netHasChanged();
    }

}

/**
 * @brief GraphView::deleteElementFromGraph
 */
void GraphView::deleteElementFromGraph()
{
  QList<QGraphicsItem*> selectedItems = m_Scene.selectedItems();

  while(!selectedItems.empty())
    {
      QGraphicsItem *i = selectedItems.front();

      //Delete the element acording to its type
      if(i->type() == GraphDecorator::Type)
        deleteGraph((GraphDecorator*)(i),true);

      else if(i->type() == GraphTerminal::Type)
        deleteGraph((GraphTerminal*)(i),true);

      else if(i->type() == GraphEdge::Type)
        deleteGraph((GraphEdge*)(i),true);

      else if(i->type() == GraphEntity::Type)
        deleteGraph((GraphEntity*)(i),true);


      //Check the remaining items selected
      selectedItems = m_Scene.selectedItems();
    }
}

/**
 * @brief GraphView::deleteGraph
 * @param item
 */
void GraphView::deleteGraph(GraphEdge *item, bool blacklist)
{
  assert(item);

  if(blacklist)
    item->getEdge()->blackList(m_pSelectedElement);

  if(pTo==item)
    pTo=NULL;

  item->netItem->pGraphItem=NULL;

  m_Scene.removeItem(item);
  delete item;

  emit graphHasChanged();
}

/**
 * @brief GraphView::deleteGraph
 * @param item
 */
void GraphView::deleteGraph(GraphSubNode* item, bool blacklist)
{
  assert(item);

  if(blacklist)
    ((SubNode*)item->netItem)->blackList(m_pSelectedElement);

  if(pTo==item)
    pTo=NULL;

  SubNode* t=item->pSubNode;

  t->x = item->pos().x();
  t->y = item->pos().y();

  vector<Edge*> edge=t->getInEdge();
  for(unsigned int i=0; i<edge.size(); i++)
    if(edge[i]->pGraphItem)
        deleteGraph(edge[i]->getGraph(),blacklist);

  edge=t->getOutEdge();
  for(unsigned int i=0; i<edge.size(); i++)
    if(edge[i]->pGraphItem)
        deleteGraph(edge[i]->getGraph(),blacklist);

  vector<SubNode*> subnode=t->getSubNode();
  for(unsigned int i=0; i<subnode.size(); i++)
    if(subnode[i]->pGraphItem)
        deleteGraph(subnode[i]->getGraph(),blacklist);

  m_Scene.removeItem(item);
  m_Scene.removeItem(item->pLine);

  delete item;
  t->pGraphItem = NULL;

  emit graphHasChanged();
}

/**
 * @brief GraphView::deleteGraph
 * @param item
 */
void GraphView::deleteGraph(GraphEntity *item, bool blacklist)
{
  assert(item);

  if(pTo==item)
    pTo=NULL;

  Entity* e=item->getEntity();

  e->x = item->pos().x();
  e->y = item->pos().y();

  vector<Edge *> edge=e->getInEdge();
  for(unsigned int i=0; i<edge.size(); i++)
    if(edge[i]->pGraphItem)
        deleteGraph(edge[i]->getGraph(),blacklist);

  edge=e->getOutEdge();
  for(unsigned int i=0; i<edge.size(); i++)
    if(edge[i]->pGraphItem)
        deleteGraph(edge[i]->getGraph(),blacklist);

  vector<SubNode*> subnode=e->getSubNode();
  for(unsigned int i=0; i<subnode.size(); i++)
    if(subnode[i]->pGraphItem)
        deleteGraph(subnode[i]->getGraph(),blacklist);

  m_Scene.removeItem(item);

  delete item;
  e->pGraphItem = NULL;

  emit graphHasChanged();
}

/**
 * @brief GraphView::deleteElementFromNet
 */
void GraphView::deleteElementFromNet()
{
  QList<QGraphicsItem*> selectedItems = m_Scene.selectedItems();

  while(!selectedItems.empty())
    {
      QGraphicsItem *i = selectedItems.front();

      //Delete the element acording to its type
      if(i->type() == GraphDecorator::Type)
        deleteNet((GraphDecorator*)i);

      else if(i->type() == GraphTerminal::Type)
        deleteNet((GraphTerminal*)(i));

      else if(i->type() == GraphEdge::Type)
        deleteNet((GraphEdge*)i);

      else if(i->type() == GraphEntity::Type)
        deleteNet((GraphEntity*)i);

      //Check the remaining items selected
      selectedItems = m_Scene.selectedItems();
    }
}

/**
 * @brief GraphView::deleteNet
 * @param item
 */
void GraphView::deleteNet(GraphEdge *item)
{

  Edge* e = item->getEdge();

  deleteGraph(item);
  delete e;

  emit netHasChanged();
}

/**
 * @brief GraphView::deleteNet
 * @param item
 */
void GraphView::deleteNet(GraphSubNode* item)
{

  SubNode* t = item->pSubNode;

  deleteGraph(item);
  delete t;

  emit netHasChanged();
}

/**
 * @brief GraphView::deleteNet
 * @param item
 */
void GraphView::deleteNet(GraphEntity *item)
{
  Entity* e = item->getEntity();

  deleteGraph(item);
  delete e;

  emit netHasChanged();
}

/**
 * @brief GraphView::populateScene
 */
void GraphView::populateScene()
{
  //empty the scene
  clearScene();

  vector<Entity*> Entities = m_pSemanticNet->getEntity();
  //Represent all the entities
  for(unsigned int i=0; i<Entities.size(); i++)
    {
      addOnGraph(Entities[i], QPointF(0,0));

      vector<SubNode*> l=Entities[i]->getSubNode();
      for(unsigned j=0; j< l.size(); j++)
        if(l[j]->pGraphItem)
          l[j]->pGraphItem->setPos(0,0);
    }

  //Then add the edges
  vector<Edge*> Edges=m_pSemanticNet->getEdge();
  for(unsigned int i=0; i<Edges.size(); i++)
      addOnGraph(Edges[i]);

  emit graphHasChanged();
}

/**
 * @brief GraphView::clearScene
 */
void GraphView::clearScene()
{
  //To delete all the elements of the scene
  //we only need to delete all the entities
  vector<Entity *> entities = m_pSemanticNet->getEntity();

  for(unsigned int i=0; i<entities.size(); i++)
    if(GraphEntity *e = (GraphEntity*)entities[i]->pGraphItem)
      deleteGraph(e);

  emit graphHasChanged();
}

void GraphView::reArrangeSceneCircle()
{
  if(m_Scene.selectedItems().empty())
    {
      populateScene();
      reArrangeSceneAll(true);
    }
  else
    reArrangeSelection(true);
}

void GraphView::reArrangeSceneSquare()
{
  if(m_Scene.selectedItems().empty())
    {
      populateScene();
      reArrangeSceneAll(false);
    }
  else
    reArrangeSelection(false);
}

void GraphView::reArrangeSceneAll(bool circle)
{
  vector<Entity*> Entities = m_pSemanticNet->getEntity();
  vector<GraphEntity*> l;

  for(unsigned int i=0; i<Entities.size(); i++)
    if(Entities[i]->pGraphItem)
      l.push_back((GraphEntity*)Entities[i]->pGraphItem);

  if(circle)
    reArrange_Circle(l,QPoint(0,0));
  else
    reArrange_Square(l,QPoint(0,0));

  centerOn(0,0);
}

/**
 * @brief GraphView::reArrangeSelection
 */
void GraphView::reArrangeSelection(bool circle)
{  
  vector<GraphEntity*> l;
  foreach(QGraphicsItem* item, m_Scene.selectedItems())
    if(item->type() == GraphEntity::Type)
        l.push_back((GraphEntity*)item);

  if(l.size()==1)
    reArrangeEntity(l[0]);
  else if(circle)
    reArrange_Circle(l,m_MousePos);
  else
    reArrange_Square(l,m_MousePos);
}

void GraphView::reArrange_Circle(std::vector<GraphEntity*> l, QPoint center)
{
  if(l.empty())
    return;

  //Calculate the number of entity with a graph item (i.e represented)
    int size=l.size();

  //Adjust the pattern parameters
    qreal EntityAngleStep = 2*M_PI/size;
    qreal EntityR = 350;

    if(size > 2)
      EntityR = 350/sin(EntityAngleStep);

  //Change the position of all the entity...
  GraphAnimation* Animation = new GraphAnimation();
  for(unsigned int i=0; i<l.size(); i++)
    {
      //Set the item's new position
        qreal x = center.x()+EntityR * cos(i*EntityAngleStep);
        qreal y = center.y()+EntityR * sin(i*EntityAngleStep);

        Animation->assignProperty(l[i], QPointF(x,y));
      //Change the position of all the terminals of the entity
        reArrangeEntity(l[i],QPointF(x,y));
    }

  Animation->start();
}

void GraphView::reArrange_Square(std::vector<GraphEntity*> l, QPoint center)
{
  if(l.empty())
    return;

  //Calculate the number of entity with a graph item (i.e represented)
    int size=round(sqrt(l.size()));

  //Adjust the pattern parameters
    qreal EntityR = 450;

    int c=(size-1)*EntityR;

  //Change the position of all the entity...
  GraphAnimation* Animation = new GraphAnimation();
  for(unsigned int i=0; i<l.size(); i++)
    {
      //Set the item's new position
        qreal x = center.x()+EntityR*(i/size)-c/2;
        qreal y = center.y()+EntityR*(i%size)-c/2;

        Animation->assignProperty(l[i], QPointF(x,y));
      //Change the position of all the terminals of the entity
        reArrangeEntity(l[i],QPointF(x,y));
    }

  Animation->start();
}

/**
 * @brief GraphView::reArrangeEntity
 * @param e
 */
void GraphView::reArrangeEntity(GraphEntity *e,QPointF pos)
{
  vector<SubNode*> SubNode = e->getEntity()->getSubNode();

  if(!SubNode.empty())
    {
      //Calculate the number of terminal with a graph item (i.e represented)
      int size2=0;
      for(unsigned int k=0; k<SubNode.size(); k++)
        if(SubNode[k]->pGraphItem)
          size2++;

      //Adjust the pattern parameters
      qreal DecoratorAngleStep = 2*M_PI/size2;
      qreal DecoratorR=(ENTITY_ELLIPSE_RECT.width()
                        +DECORATOR_RECT.width()+30)/2;

      // Values
      GraphAnimation* Animation = new GraphAnimation();
      int j=0;
      for(unsigned int i=0; i<SubNode.size(); i++)
        if(GraphElement *item = SubNode[i]->pGraphItem)
          {
            Animation->assignProperty(item, QPointF(pos.x()+DecoratorR * cos(j*DecoratorAngleStep), pos.y()+DecoratorR * sin(j*DecoratorAngleStep)));
            j++;
          }

      Animation->start();
    }
}

void GraphView::reArrangeEntity(GraphEntity* e)
{
  reArrangeEntity(e,e->pos());
}

void GraphView::addOnGraph(Edge*e)
{
  if(e->pGraphItem || !e->from->pGraphItem || !e->to->pGraphItem)
    return;

  GraphEdge *item = new GraphEdge(e);

  e->pGraphItem=item;
  m_Scene.addItem(item);

  emit graphHasChanged();
}

void GraphView::addOnGraph(SubNode *n, bool all)
{
  if(n->x==COOR_INVALID || n->y==COOR_INVALID)
    addOnGraph(n,m_MousePos,all);
  else
    addOnGraph(n,QPointF(n->x,n->y),all);
}

void GraphView::addOnGraph(SubNode *n, QPointF pos, bool all)
{
  if(n->pGraphItem)
    return;

  GraphSubNode* item;

  if(TYPE_SUBNODE(n->type())==SubNode::DECORATOR)
    item=new GraphDecorator((Decorator*)n);
  else
    item=new GraphTerminal((Terminal*)n);

  item->setPos(pos);
  n->pGraphItem = item;
  m_Scene.addItem(item);
  m_Scene.addItem(item->pLine);

  if(all)
    {
      addOnGraphAllEdge(n);
      addOnGraphAllSubNode(n);
    }

  emit graphHasChanged();
}

void GraphView::addOnGraphAllEdge(Node* n)
{
  vector<Edge *> InEdge=n->getInEdge();
  for(unsigned int i=0; i<InEdge.size(); i++)
    if(!InEdge[i]->pGraphItem)
        addOnGraph(InEdge[i]);

  vector<Edge *> OutEdge=n->getOutEdge();
  for(unsigned int i=0; i<OutEdge.size(); i++)
    if(!OutEdge[i]->pGraphItem)
      addOnGraph(OutEdge[i]);
}

void GraphView::addOnGraphAllSubNode(Node* n)
{
  vector<SubNode*> SubNode=n->getSubNode();
  for(unsigned int i=0; i<SubNode.size(); i++)
    if(!SubNode[i]->pGraphItem)
      addOnGraph(SubNode[i]);
}

void GraphView::addOnGraphAllSubNode(Node* n, QPointF pos)
{
  vector<SubNode*> SubNode=n->getSubNode();
  for(unsigned int i=0; i<SubNode.size(); i++)
    if(!SubNode[i]->pGraphItem)
      addOnGraph(SubNode[i],pos);
}

void GraphView::addOnGraph(Entity *e, bool all)
{
  if(e->x==COOR_INVALID || e->y==COOR_INVALID)
    addOnGraph(e,m_MousePos,all);
  else
    addOnGraph(e,QPointF(e->x,e->y),all);
}

void GraphView::addOnGraph(Entity *e, QPointF pos, bool all)
{
  if(e->pGraphItem)
    return;

  GraphEntity* item= new GraphEntity(e);

  item->setPos(pos);

  e->pGraphItem = item;
  m_Scene.addItem(item);

  if(all)
    {
      addOnGraphAllEdge(e);
      addOnGraphAllSubNode(e);
    }

  emit graphHasChanged();
}

void GraphView::getInfo()
{
  assert(!m_Scene.selectedItems().isEmpty());

  GraphElement* item = (GraphElement*) m_Scene.selectedItems().front();
  emit informationOn(item->netItem);
}

void GraphView::getInfo(QGraphicsItem *item)
{
  if(!item)
    emit informationOn(NULL);
  else
    {
     // centerOn(item->pos());
      emit informationOn(((GraphElement*) item)->netItem);
    }
}

void GraphView::selectElement(QGraphicsItem *item)
{
  if(!item)
    {
      m_pSelectedElement = NULL;
      emit(select(NULL));
      return;
    }

  SemanticElement* e=((GraphElement*) item)->netItem;

  if(TYPE_NODE(e->type())==Node::ENTITY)
      m_pSelectedElement = (Entity*)e;

  emit(select(m_pSelectedElement));
}

void GraphView::selectPointOfView(QGraphicsItem* item)
{
  if(!item)
    {
      m_pPointOfView = NULL;
      adaptView(m_pPointOfView);
      emit(selectPOV(NULL));
      return;
    }

  SemanticElement* e=((GraphElement*) item)->netItem;

  if(TYPE_NODE(e->type())==Node::ENTITY)
      m_pPointOfView = (Entity*)e;

  adaptView(m_pPointOfView);
  emit(selectPOV(m_pPointOfView));
}

void GraphView::updateAction()
{
  if(!pMainWindow->ui)
    return;

  QList<QGraphicsItem *> selectedItems = m_Scene.selectedItems();

  bFromNode=false;
  bFromEdges=false;
  bFromDecorators=false;

  int count = selectedItems.size();
  int countEntity=0;
  int countNode=0;
  int countDecorator=0;
  for(int i=0; i<selectedItems.size();i++)
    if(selectedItems.at(i)->type() != GraphEdge::Type)
      {
        GraphElement* g=(GraphElement*)selectedItems.at(i);
        ++countNode;
        if(TYPE_SUBNODE(g->netItem->type())==SubNode::DECORATOR)
          ++countDecorator;
        else if(TYPE_NODE(g->netItem->type())==Node::ENTITY)
          ++countEntity;
      }

  if(selectedItems.size()>=1)
    {

      GraphElement* item=(GraphElement*) selectedItems[0];

      bFromNode=selectedItems.size()==1&&TYPE_ELEMENT(item->netItem->type())==SemanticElement::NODE;
      bFromEdges=selectedItems[0]->type()==GraphEdge::Type && selectedItems.size()>1;
      bFromDecorators=selectedItems[0]->type()==GraphDecorator::Type && selectedItems.size()>1;

      Node* from;
      if(bFromEdges)
        from=((GraphEdge*)selectedItems[0])->getEdge()->from;

      Node* dParent;
      if(bFromDecorators)
        dParent=((GraphDecorator*)selectedItems[0])->getDecorator()->pOwner;

      int i=1;
      while( (bFromEdges||bFromDecorators) && i<selectedItems.size())
        {
          if(selectedItems[i]->type()==GraphEdge::Type)
            {
              bFromDecorators=false;
              bFromEdges=bFromEdges&&((GraphEdge*)selectedItems[i])->getEdge()->from==from;
            }
          else if(selectedItems[i]->type()==GraphDecorator::Type)
            {
              bFromEdges=false;
              bFromDecorators=bFromDecorators&&((GraphDecorator*)selectedItems[i])->getDecorator()->pOwner==dParent;
            }
          else
            {
              bFromDecorators=false;
              bFromEdges=false;
            }
          i++;
        }
    }

  pMainWindow->ui->actionHide->setEnabled(count);
  pMainWindow->ui->actionDelete->setEnabled(count);
  pMainWindow->ui->actionInformation->setEnabled(count);
  pMainWindow->ui->actionBring_to_back->setEnabled(count);
  pMainWindow->ui->actionBring_to_front->setEnabled(count);
  pMainWindow->ui->actionNew_Edge->setEnabled(countNode == count && count== 2);
  pMainWindow->ui->actionNew_Decorator->setEnabled(bFromNode||bFromEdges||bFromDecorators);
  pMainWindow->ui->actionEdit->setEnabled(count == 1);
  pMainWindow->ui->actionShow->setEnabled(m_pPointOfView || countEntity);
}

void GraphView::showIn()
{
  if(!m_pPointOfView)
    {
      GraphEntity* e=(GraphEntity*)m_Scene.selectedItems().front();
      m_pPointOfView=e->getEntity();
      emit(selectPOV(m_pPointOfView));
    }

  adaptView(m_pPointOfView, SHOW_IN);
}

void GraphView::showOut()
{
  if(!m_pPointOfView)
    {
      GraphEntity* e=(GraphEntity*)m_Scene.selectedItems().front();
      m_pPointOfView=e->getEntity();
      emit(selectPOV(m_pPointOfView));
    }

  adaptView(m_pPointOfView, SHOW_OUT);
}

void GraphView::showInOut()
{
  if(!m_pPointOfView)
    {
      GraphEntity* e=(GraphEntity*)m_Scene.selectedItems().front();
      m_pPointOfView=e->getEntity();
      emit(selectPOV(m_pPointOfView));
    }

  adaptView(m_pPointOfView, SHOW_IN_OUT);
}

void GraphView::adaptView(Entity* parent, ShowEdge s)
{
  clearScene();

//  if(parent)
//    {
//      vector<Entity*> h;
//      h=parent->getVectorHierachy();
//      adaptView(parent,h,s);
//    }
//  else
//    {
//      vector<Entity*> l=m_pSemanticNet->getEntity();
//      for(unsigned int i=0; i<l.size();i++)
//        addOnGraph(l[i]);
//    }

  reArrangeSceneAll(true);
  emit graphHasChanged();
}

void GraphView::adaptView(Node* n, std::vector<Entity*> h, ShowEdge s)
{
  if(TYPE_NODE(n->type())==Node::ENTITY)
    adaptView((Entity*)n,h,s);
  else
    adaptView((SubNode*)n,h,s);
}

void GraphView::adaptView(Entity* n, std::vector<Entity*> h, ShowEdge s)
{
  if(n->pGraphItem)
    return;

  addOnGraph(n,false);

  vector<Edge*> e;

  if(s==SHOW_IN || s==SHOW_IN_OUT)
    {
      e=n->getInEdge();
      for(unsigned int j=0; j< e.size(); j++)
        adaptView(e[j],h,s);
    }

  if(s==SHOW_OUT || s==SHOW_IN_OUT)
    {
      e=n->getOutEdge();
      for(unsigned int j=0; j< e.size(); j++)
        adaptView(e[j],h,s);
    }

  vector<SubNode*> l=n->getSubNode();
  for(unsigned int j=0; j< l.size(); j++)
    adaptView(l[j],h,s);
}

void GraphView::adaptView(SubNode* n, vector<Entity*> h, ShowEdge s)
{
  if(n->pGraphItem)
    return;

  if(n->canBeAccessedBy(h))
    {
      adaptView(n->pOwner,h,s);

      addOnGraph(n,false);

      vector<Edge*> e;

      if(s==SHOW_IN || s==SHOW_IN_OUT)
        {
          e=n->getInEdge();
          for(unsigned int j=0; j< e.size(); j++)
            adaptView(e[j],h,s);
        }

      if(s==SHOW_OUT || s==SHOW_IN_OUT)
        {
          e=n->getOutEdge();
          for(unsigned int j=0; j< e.size(); j++)
            adaptView(e[j],h,s);
        }

      vector<SubNode*> l=n->getSubNode();
      for(unsigned int j=0; j< l.size(); j++)
        adaptView(l[j],h,s);
    }
}

void GraphView::adaptView(Edge* e, vector<Entity*> h,ShowEdge s)
{
  if(e->pGraphItem)
    return;

  if(e->canBeAccessedBy(h))
    {
      adaptView(e->to,h,s);
      adaptView(e->from,h,s);

      addOnGraph(e);
    }
}

void GraphView::bringFront()
{
  ++iMaxZValue;

  QList<QGraphicsItem *> l =m_Scene.selectedItems();
  for(int i=0; i<l.size(); i++)
    l.at(i)->setZValue(iMaxZValue);
}

void GraphView::bringBack()
{
  --iMinZValue;

  QList<QGraphicsItem *> l =m_Scene.selectedItems();
  for(int i=0; i<l.size(); i++)
    l.at(i)->setZValue(iMinZValue);
}

void GraphView::mouseMoveEvent(QMouseEvent *e)
{
  //Keep track of the mouse
  m_MousePos = mapToScene(e->pos()).toPoint();
  QGraphicsView::mouseMoveEvent(e);
}

/**
 * @brief GraphView::mousePressEvent
 * @param e
 */
void GraphView::mousePressEvent( QMouseEvent *e)
{
  QGraphicsItem *item = itemAt(e->pos());

  if (e->modifiers() & Qt::ControlModifier)
    {
      if(!item)
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
  else if (item)
    {
      if (item->type()==GraphEntity::Type && (e->modifiers() & Qt::AltModifier) )
        {selectDecorator((GraphEntity*)item);}
      else if (e->modifiers() & Qt::ShiftModifier)
        {
          if(item->type()==GraphEntity::Type || item->type()==GraphDecorator::Type || item->type()==GraphTerminal::Type)
            {
              QList<QGraphicsItem *> l =m_Scene.selectedItems();
              if(l.size() == 1)
                {
                  newEdge((GraphElement*)l[0],(GraphElement*)item);
                  return;
                }
            }
        }
      else if( e->button() & Qt::RightButton)
        {
          if(pTo)
            pTo->setSelected(false);

          item->setSelected(true);
          pTo=item;
          return;
        }
    }
  else if( e->button() & Qt::RightButton)
    {
      pTo = NULL;
      return;
    }

   QGraphicsView::mousePressEvent(e);
}

/**
 * @brief GraphView::mouseReleaseEvent
 * @param e
 */
void GraphView::mouseReleaseEvent(QMouseEvent *e)
{
  QGraphicsView::mouseReleaseEvent(e);

  if(dragMode() == QGraphicsView::ScrollHandDrag)
      setDragMode(QGraphicsView::RubberBandDrag);
}

/**
 * @brief GraphView::mouseDoubleClickEvent
 * @param e
 */
void GraphView::mouseDoubleClickEvent(QMouseEvent *e)
{
  QGraphicsItem *item = itemAt(e->pos());
  QList<QGraphicsItem*> l=m_Scene.selectedItems();

  if(!item && e->modifiers() & Qt::ControlModifier)
    {
      selectElement(NULL);
      return;
    }
  if(!item && e->modifiers() & Qt::AltModifier)
    {
      selectPointOfView(NULL);
      return;
    }
  else if(!item)
    {
      getInfo(NULL);
      return;
    }

  if(item->type()<QGraphicsItem::UserType)
    return;

  SemanticElement* el=((GraphElement*)item)->netItem;

  if(e->modifiers() & Qt::ControlModifier)
    {
      if(l.size()==2 && l.first()->type()==GraphDecorator::Type && TYPE_ELEMENT(el->type())==SemanticElement::NODE)
        {
          m_pSemanticNet->changeOwner(((GraphDecorator*)l.first())->getDecorator(),(Node*) el);
          ((GraphDecorator*)l.first())->changeOwner((Node*)el);
          item->setSelected(false);
        }
      else
        selectElement(item);
    }
  else if(e->modifiers() & Qt::AltModifier)
    {
      selectPointOfView(item);
    }
  else
    getInfo(item);
}

/**
 * @brief GraphView::wheelEvent
 * @param e
 */
void GraphView::wheelEvent(QWheelEvent *e)
{
  if (e->modifiers() & Qt::ControlModifier)
    {
        if (e->delta() > 0)
            scale(1.25,1.25);
        else
            scale(0.75,0.75);
    }
  else
    QGraphicsView::wheelEvent(e);
}

/**
 * @brief GraphView::keyPressEvent
 * @param e
 */
void GraphView::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_F1)
      {
        if(m_fps->scene() == &m_Scene)
          m_Scene.removeItem(m_fps);
        else
          m_Scene.addItem(m_fps);
      }
    else
      QGraphicsView::keyPressEvent(e);
}

void GraphView::paintEvent(QPaintEvent *e)
{
  if (m_frameCount == 0)
  {
       m_time.start();
  }
  else if(m_time.elapsed() >= 100)
  {
      QString s("FPS: %1");
      m_fps->setPlainText(s.arg(m_frameCount*1000/m_time.elapsed()));
      m_frameCount = -1;
  }
  m_frameCount++;

 if(m_fps->scene())
   {
     QPointF tl(horizontalScrollBar()->value(), verticalScrollBar()->value());
     QPointF br = tl + viewport()->rect().bottomRight();
     QMatrix mat = matrix().inverted();
     QRectF rect= mat.mapRect(QRectF(tl,br));
     m_fps->setPos(rect.topLeft());
   }

  QGraphicsView::paintEvent(e);
}

void GraphView::contextMenuEvent(QContextMenuEvent* event)
{
  QMenu menu(this);
  menu.addMenu(pMainWindow->ui->menuNew);
  menu.addAction(pMainWindow->ui->actionEdit);
  menu.addAction(pMainWindow->ui->actionDelete);
  menu.addSeparator();
  menu.addAction(pMainWindow->ui->actionHide);
  menu.addAction(pMainWindow->ui->actionBring_to_back);
  menu.addAction(pMainWindow->ui->actionBring_to_front);
  menu.addSeparator();
  menu.addAction(pMainWindow->ui->actionRestart_Scene);
  menu.addAction(pMainWindow->ui->actionShow);
  menu.addAction(pMainWindow->ui->actionInformation);
  menu.exec(event->globalPos());
}

void GraphView::selectDecorator(GraphEntity *e)
{
  vector<SubNode*> SubNode = e->getEntity()->getSubNode();

  for(unsigned int k=0; k<SubNode.size(); k++)
    if(GraphElement *t = SubNode[k]->pGraphItem)
      t->setSelected(true);
}

void GraphAnimation::assignProperty(GraphElement* item, QPointF pos)
{
  QPropertyAnimation *anim = new QPropertyAnimation(item, "pos");

  anim->setEndValue(pos);
  anim->setDuration(ANIMATION_DURATION);
  anim->setEasingCurve(QEasingCurve::OutBack);
  addAnimation(anim);
}

void GraphAnimation::start()
{
  QAbstractAnimation::start(QAbstractAnimation::DeleteWhenStopped);
}





























