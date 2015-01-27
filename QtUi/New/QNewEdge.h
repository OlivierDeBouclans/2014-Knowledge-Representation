#ifndef QNEWEDGE_H
#define QNEWEDGE_H

#include <QWidget>

namespace Ui {
  class QNewEdge;
}

class QNewEdge : public QWidget
{
    Q_OBJECT

  public:
    explicit QNewEdge(QWidget *parent = 0);
    ~QNewEdge();

    int getType();
    int getCard();

    void setType(int);
    void setCard(int);
    void setPhysical(bool);

  signals:
    void showPhysical(int);

  protected:
    void wheelEvent(QWheelEvent* e);

  private:
    Ui::QNewEdge *ui;
};

#endif // QNEWEDGE_H
