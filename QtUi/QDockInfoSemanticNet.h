#ifndef QDOCKINFOSEMANTICNET_H
#define QDOCKINFOSEMANTICNET_H

#include <QDockWidget>
#include <QTreeWidgetItem>

class Entity;
class SemanticNet;
class GraphEntity;

class GraphInfoTreeItem : public QTreeWidgetItem
{
  public:
    GraphInfoTreeItem(Entity *);

    Entity* m_pEntity;
};

namespace Ui
{
  class QDockInfoSemanticNet;
}

class QDockInfoSemanticNet : public QDockWidget
{
    Q_OBJECT

  public:
    explicit QDockInfoSemanticNet(QWidget *parent = 0);
    ~QDockInfoSemanticNet();

    void setSemanticNet(SemanticNet *s);
    void setPath(QString);
    void clear();

  public slots:
    void createTree();
    void updateTree();
    void hideShow(QTreeWidgetItem*);
    void hideOrShow();

    void select(Entity* e);
    void selectPointOfView(Entity* e);

  signals:
    void addOnGraph(Entity*);
    void deleteFromGraph(GraphEntity*);
    void isShown(bool);
    void unselectElement();

  protected:
    void closeEvent(QCloseEvent* e);

    Ui::QDockInfoSemanticNet *ui;
    SemanticNet *m_SemanticNet;
    Entity* m_pSelectedElement;

    QTreeWidgetItem TreeObject;
    QTreeWidgetItem TreeAttribute;
    QTreeWidgetItem TreeAttributeNumerical;
    QTreeWidgetItem TreeValue;
    QTreeWidgetItem TreeValueNumerical;
    QTreeWidgetItem TreeAction;
    QTreeWidgetItem TreeObjectPhysical;
    QTreeWidgetItem TreeActionPhysical;

};

#endif // QDOCKINFOSEMANTICNET_H
