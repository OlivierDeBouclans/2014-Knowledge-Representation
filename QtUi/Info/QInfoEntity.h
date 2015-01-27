#ifndef QINFOENTITY_H
#define QINFOENTITY_H

#include <QWidget>

class Entity;

namespace Ui {
  class QInfoEntity;
}

class QInfoEntity : public QWidget
{
    Q_OBJECT

  public:
    explicit QInfoEntity(QWidget *parent = 0);
    ~QInfoEntity();

    void display(Entity* e);

  protected:
    Ui::QInfoEntity *ui;
};

#endif // QINFOENTITY_H
