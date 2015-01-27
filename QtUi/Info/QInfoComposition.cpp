#include "QInfoComposition.h"
#include "ui_QInfoComposition.h"

#include <SemanticNet.h>
#include <GraphElements.h>
#include <Qtui/QDockInfoElement.h>

GraphCompTreeItem::GraphCompTreeItem(Node* n) : m_pNode(n)
{

}

QInfoComposition::QInfoComposition(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QInfoComposition)
{
  ui->setupUi(this);

  connect(ui->Composition, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
          this, SLOT(display(QTreeWidgetItem*,int)));

  connect(ui->Hierarchy, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
          this, SLOT(display(QTreeWidgetItem*)));
}

QInfoComposition::~QInfoComposition()
{
  delete ui;
}

void QInfoComposition::display(QTreeWidgetItem* item,int i)
{
  if(i!=Property)
    return;

  Node* n=((GraphCompTreeItem*)item)->m_pNode;

  if(TYPE_NODE(n->type())==Node::ENTITY)
    displayComposition(((Entity*)n)->getComposition());
}

void QInfoComposition::display(QTreeWidgetItem* item)
{
  Node* n=((GraphCompTreeItem*)item)->m_pNode;

  displayComposition(((Entity*)n)->getComposition());
}

void QInfoComposition::display(Entity* entity)
{
  displayHierarchy(entity->getHierarchy());
  displayComposition(entity->getComposition());
}

void QInfoComposition::displayHierarchy(Tree<Entity*> t)
{
  ui->Hierarchy->clear();

  QTreeWidgetItem item;
  displayHierarchy(t,&item);

  ui->Hierarchy->addTopLevelItem(item.takeChild(0));
  ui->Hierarchy->expandAll();
  ui->Hierarchy->resizeColumnToContents(0);
  ui->Hierarchy->resizeColumnToContents(1);
  ui->Hierarchy->resizeColumnToContents(2);
}

void QInfoComposition::displayHierarchy(Tree<Entity*> t, QTreeWidgetItem* i)
{
  GraphCompTreeItem* item=new GraphCompTreeItem(t.node);
  item->setText(0,QString::fromStdString(t.node->sLabel));

  for(unsigned int j=0; j<t.child.size(); j++)
    displayHierarchy(t.child[j],item);

  i->addChild(item);
}

void QInfoComposition::displayComposition(Tree<Node*> t)
{
  ui->Composition->clear();

  QTreeWidgetItem item;

  vector<Tree<Node*> > v=t.child;
  for(unsigned int i=0; i<v.size(); i++)
    displayComposition(v[i],&item,0);

  ui->Composition->addTopLevelItems(item.takeChildren());

  ui->Composition->sortByColumn(Depth,Qt::AscendingOrder);
  ui->Composition->expandAll();
  ui->Composition->resizeColumnToContents(0);
  ui->Composition->resizeColumnToContents(1);
  ui->Composition->resizeColumnToContents(2);
  ui->Composition->resizeColumnToContents(3);
}

void QInfoComposition::displayComposition(Tree<Node*> t, QTreeWidgetItem* p, int depth)
{
  GraphCompTreeItem *item = new GraphCompTreeItem(t.node) ;
  item->setText(Depth,QString::number(depth));
  item->setText(Property,QString::fromStdString(t.node->sLabel));
  item->setText(Type,QString::fromStdString(t.node->typeToString()));

  if(TYPE_SUBNODE(t.node->type())==SubNode::DECORATOR)
    if(Entity* e= ((Decorator*)t.node)->pParent)
      item->setText(From,QString::fromStdString(e->sLabel));

  QBrush brush(GraphElement::getColor(t.node));

  for(int a=0; a<p->childCount(); a++)
    if(((GraphCompTreeItem*)p->child(a))->m_pNode == t.node)
      {
        brush.setStyle(Qt::FDiagPattern);
        item->setText(DefinedBy,p->child(a)->text(From));
        break;
      }

  item->setBackground(Property,brush);

  for(unsigned int j=0; j<t.child.size(); j++)
    displayComposition(t.child[j],item,depth);

  p->addChild(item);
}

