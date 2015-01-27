#ifndef QINFOEDGE_H
#define QINFOEDGE_H

#include <QWidget>

class Edge;

namespace Ui {
  class QInfoEdge;
}

class QInfoEdge : public QWidget
{
    Q_OBJECT

  public:
    explicit QInfoEdge(QWidget *parent = 0);
    ~QInfoEdge();

    void display(Edge* e);

  private:
    Ui::QInfoEdge *ui;
};

#endif // QINFOEDGE_H
