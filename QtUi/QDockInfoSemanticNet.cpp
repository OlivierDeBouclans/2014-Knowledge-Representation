#include "QDockInfoSemanticNet.h"
#include "ui_QDockInfoSemanticNet.h"

#include <SemanticNet.h>
#include <GraphElements.h>

#include <QCloseEvent>

GraphInfoTreeItem::GraphInfoTreeItem(Entity *e) : m_pEntity(e)
{
  setText(0,QString::fromStdString(e->sLabel));
  setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

  if(e->pGraphItem)
    setCheckState(0,Qt::Checked);
  else
    setCheckState(0,Qt::Unchecked);
}

QDockInfoSemanticNet::QDockInfoSemanticNet(QWidget *parent) :
  QDockWidget(parent),
  ui(new Ui::QDockInfoSemanticNet)
{
  ui->setupUi(this);

  TreeObject = QTreeWidgetItem(QStringList(QString("Object")));
  TreeObject.setBackgroundColor(0,COL_OBJECT);
  TreeObject.setFlags(!Qt::ItemIsSelectable);
  TreeObject.setFlags(Qt::ItemIsEnabled);

  TreeAttribute = QTreeWidgetItem(QStringList(QString("Attribute")));
  TreeAttribute.setBackgroundColor(0,COL_ATTRIBUTE);
  TreeAttribute.setFlags(!Qt::ItemIsSelectable);
  TreeAttribute.setFlags(Qt::ItemIsEnabled);

  TreeAttributeNumerical =QTreeWidgetItem(QStringList(QString("Numerical")));
  TreeAttributeNumerical.setBackgroundColor(0,COL_ATTRIBUTE_NUMERICAL);
  TreeAttributeNumerical.setFlags(!Qt::ItemIsSelectable);
  TreeAttributeNumerical.setFlags(Qt::ItemIsEnabled);

  TreeValue = QTreeWidgetItem(QStringList(QString("Value")));
  TreeValue.setBackgroundColor(0,COL_VALUE);
  TreeValue.setFlags(!Qt::ItemIsSelectable);
  TreeValue.setFlags(Qt::ItemIsEnabled);

  TreeValueNumerical = QTreeWidgetItem(QStringList(QString("Numerical")));
  TreeValueNumerical.setBackgroundColor(0,COL_VALUE_NUMERICAL);
  TreeValueNumerical.setFlags(!Qt::ItemIsSelectable);
  TreeValueNumerical.setFlags(Qt::ItemIsEnabled);

  TreeAction = QTreeWidgetItem(QStringList(QString("Action")));
  TreeAction.setBackgroundColor(0,COL_ACTION);
  TreeAction.setFlags(!Qt::ItemIsSelectable);
  TreeAction.setFlags(Qt::ItemIsEnabled);

  TreeObjectPhysical = QTreeWidgetItem(QStringList(QString("Object")));
  TreeObjectPhysical.setBackgroundColor(0,COL_OBJECT_CONCRETE);
  TreeObjectPhysical.setFlags(!Qt::ItemIsSelectable);
  TreeObjectPhysical.setFlags(Qt::ItemIsEnabled);

  TreeActionPhysical = QTreeWidgetItem(QStringList(QString("Action")));
  TreeActionPhysical.setBackgroundColor(0,COL_ACTION_CONCRETE);
  TreeActionPhysical.setFlags(!Qt::ItemIsSelectable);
  TreeActionPhysical.setFlags(Qt::ItemIsEnabled);

  ui->TreeAbstract->addTopLevelItem(&TreeObject);
  ui->TreeAbstract->addTopLevelItem(&TreeAttribute);
  ui->TreeAbstract->addTopLevelItem(&TreeValue);
  ui->TreeAbstract->addTopLevelItem(&TreeAction);

  ui->TreePhysical->addTopLevelItem(&TreeObjectPhysical);
  ui->TreePhysical->addTopLevelItem(&TreeActionPhysical);

  connect(ui->TreeAbstract, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
          this, SLOT(hideShow(QTreeWidgetItem*)));

  connect(ui->TreePhysical, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
          this, SLOT(hideShow(QTreeWidgetItem*)));
}

QDockInfoSemanticNet::~QDockInfoSemanticNet()
{
  delete ui;
}

void QDockInfoSemanticNet::setSemanticNet(SemanticNet* s)
{
  m_SemanticNet = s;
  createTree();
  select(NULL);
  selectPointOfView(NULL);
}

void QDockInfoSemanticNet::setPath(QString s)
{
  ui->Path->setText(s);
}

void QDockInfoSemanticNet::clear()
{
  vector<QTreeWidgetItem*> l;
  TreeAttribute.removeChild(&TreeAttributeNumerical);
  TreeValue.removeChild(&TreeValueNumerical);

  l.push_back(&TreeObject);
  l.push_back(&TreeAttribute);
  l.push_back(&TreeAttributeNumerical);
  l.push_back(&TreeValue);
  l.push_back(&TreeValueNumerical);
  l.push_back(&TreeAction);
  l.push_back(&TreeObjectPhysical);
  l.push_back(&TreeActionPhysical);

  for(unsigned int i=0; i<l.size();i++)
      while(l[i]->childCount())
        {
          QTreeWidgetItem* item=l[i]->child(0);
          l[i]->removeChild(item);
          delete item;
        }
}

void QDockInfoSemanticNet::createTree()
{
  clear();

  vector<Entity *> entity = m_SemanticNet->getEntity();
  vector<Edge   *> edge   = m_SemanticNet->getEdge();

  ui->Entities->setText(QString::number(entity.size()));
  ui->Edge->setText(QString::number(edge.size()));

  int iPhysicalEntity=0;
  int iAbstractEntity=0;

  for(unsigned int i=0; i< entity.size(); i++)
    {
      GraphInfoTreeItem *item = new GraphInfoTreeItem(entity[i]);

      if(TYPE_ENTITY(entity[i]->type())==Entity::OBJECT)
        {
          if(entity[i]->pConcrete)
            {
              TreeObjectPhysical.addChild(item);
              ++iPhysicalEntity;
            }
          else
            {
              TreeObject.addChild(item);
              ++iAbstractEntity;
            }
        }
      else if(TYPE_ENTITY(entity[i]->type())==Entity::ATTRIBUTE)
        {
          ++iAbstractEntity;

          if(entity[i]->pNumerical)
            TreeAttributeNumerical.addChild(item);
          else
            TreeAttribute.addChild(item);
        }
      else if(TYPE_ENTITY(entity[i]->type())==Entity::VALUE)
        {
          ++iAbstractEntity;

          if(entity[i]->pNumerical)
            TreeValueNumerical.addChild(item);
          else
            TreeValue.addChild(item);
        }
      else if(TYPE_ENTITY(entity[i]->type())==Entity::ACTION)
        {
          if(entity[i]->pConcrete)
            {
              TreeActionPhysical.addChild(item);
              ++iPhysicalEntity;
            }
          else
            {
              TreeAction.addChild(item);
              ++iAbstractEntity;
            }
        }
     }

  if(TreeAttributeNumerical.childCount())
    TreeAttribute.insertChild(0,&TreeAttributeNumerical);
  if(TreeValueNumerical.childCount())
    TreeValue.insertChild(0,&TreeValueNumerical);

  ui->TreeAbstract->expandAll();
  ui->TreePhysical->expandAll();

  ui->tab->setTabText(0,QString("Abstract (%1)").arg(iAbstractEntity));
  ui->tab->setTabText(1,QString("Concrete (%1)").arg(iPhysicalEntity));
}

void QDockInfoSemanticNet::updateTree()
{
  ui->TreeAbstract->blockSignals(true);
  ui->TreePhysical->blockSignals(true);

  vector<QTreeWidgetItem*> l;
  TreeAttribute.removeChild(&TreeAttributeNumerical);
  TreeValue.removeChild(&TreeValueNumerical);

  l.push_back(&TreeObject);
  l.push_back(&TreeAttribute);
  l.push_back(&TreeAttributeNumerical);
  l.push_back(&TreeValue);
  l.push_back(&TreeValueNumerical);
  l.push_back(&TreeAction);
  l.push_back(&TreeObjectPhysical);
  l.push_back(&TreeActionPhysical);

  for(unsigned int i=0; i<l.size();i++)
      for(int j=0; j<l[i]->childCount();j++)
        {
          GraphInfoTreeItem* t= (GraphInfoTreeItem*) l[i]->child(j);
          if(t->m_pEntity->pGraphItem)
            t->setCheckState(0,Qt::Checked);
          else
            t->setCheckState(0,Qt::Unchecked);
        }

  if(TreeAttributeNumerical.childCount())
    TreeAttribute.insertChild(0,&TreeAttributeNumerical);
  if(TreeValueNumerical.childCount())
    TreeValue.insertChild(0,&TreeValueNumerical);

  ui->TreeAbstract->expandAll();
  ui->TreePhysical->expandAll();

  ui->TreeAbstract->blockSignals(false);
  ui->TreePhysical->blockSignals(false);
}

void QDockInfoSemanticNet::hideShow(QTreeWidgetItem* item)
{
  if( item->checkState(0) == Qt::Checked)
    {
      GraphInfoTreeItem* t=(GraphInfoTreeItem*) item;
      emit addOnGraph(t->m_pEntity);
    }
  else
    {
      GraphInfoTreeItem* t=(GraphInfoTreeItem*) item;
      GraphEntity* i= (GraphEntity*) t->m_pEntity->pGraphItem;
      emit deleteFromGraph(i);
    }
}

void QDockInfoSemanticNet::hideOrShow()
{
  if(isHidden())
    show();
  else
    hide();
}

void QDockInfoSemanticNet::selectPointOfView(Entity* e)
{
  QColor c;
  if(e)
    {
      ui->PointOfView->setText(QString::fromStdString(e->sLabel));
      c=GraphElement::getColor(e);
    }
  else
    {
      ui->PointOfView->setText("(None)");
      c=Qt::white;
    }

  QPalette *palette = new QPalette();
  palette->setColor(QPalette::Base,c);
  ui->PointOfView->setPalette(*palette);
}

void QDockInfoSemanticNet::closeEvent(QCloseEvent* e)
{
  e->ignore();
  emit isShown(false);
}

void QDockInfoSemanticNet::select(Entity* e)
{
  QColor c;
  if(e)
    {
      ui->SelectedItem->setText(QString::fromStdString(e->sLabel));
      c=GraphElement::getColor(e);
    }
  else
    {
      ui->SelectedItem->setText("(None)");
      c=Qt::white;
    }

  QPalette *palette = new QPalette();
  palette->setColor(QPalette::Base,c);
  ui->SelectedItem->setPalette(*palette);

  m_pSelectedElement=e;
}
