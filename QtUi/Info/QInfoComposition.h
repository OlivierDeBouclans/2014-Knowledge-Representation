#ifndef QINFOCOMPOSITION_H
#define QINFOCOMPOSITION_H

#include <QWidget>
#include <QTreeWidgetItem>

class Node;
class Entity;

template<class T> class Tree;

class GraphCompTreeItem : public QTreeWidgetItem
{
  public:
    GraphCompTreeItem(Node *);

    Node* m_pNode;
};

namespace Ui
{
  class QInfoComposition;
}

class QInfoComposition : public QWidget
{
    Q_OBJECT

  public:
    explicit QInfoComposition(QWidget *parent = 0);
    ~QInfoComposition();

    void display(Entity* e);
      void displayHierarchy(Tree<Entity*> t);
        void displayHierarchy(Tree<Entity*> t, QTreeWidgetItem* item);
      void displayComposition(Tree<Node*> t);
        void displayComposition(Tree<Node*> t, QTreeWidgetItem* item, int depth, Node* parent);
        void displayComposition(Tree<Node*> t, QTreeWidgetItem* p, int depth);

  public slots:
        void display(QTreeWidgetItem* item);
        void display(QTreeWidgetItem* item, int i);
  private:
    Ui::QInfoComposition *ui;

    enum
    {
      Property=0,
      From=1,
      Type=2,
      Depth=3,
      DefinedBy=4
    };

};

#endif // QINFOCOMPOSITION_H
