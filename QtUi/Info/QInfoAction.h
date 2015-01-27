#ifndef QINFOACTION_H
#define QINFOACTION_H

#include <QWidget>

class Entity;

namespace Ui {
  class QInfoAction;
}

class QInfoAction : public QWidget
{
    Q_OBJECT

  public:
    explicit QInfoAction(QWidget *parent = 0);
    ~QInfoAction();

    void display(Entity* a);

  private:
    Ui::QInfoAction *ui;
};

#endif // QINFOACTION_H
