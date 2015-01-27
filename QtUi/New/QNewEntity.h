#ifndef QNEWENTITY_H
#define QNEWENTITY_H

#include <QWidget>

class QWheelEvent;


namespace Ui
{
  class QNewEntity;
}

class QNewEntity : public QWidget
{
    Q_OBJECT

  public:
    explicit QNewEntity(QWidget *parent = 0);
    ~QNewEntity();

    std::string getName();
    int getType(int i=-1);

    void setName(std::string);
    void setType(int);

    void setNumerical(bool b);
    void setPhysical(bool b);

    void wheelEvent(QWheelEvent *);

  public slots:
    void updateOptions(int i);
    void showAction();

  signals:
    void showNumerical(int i);
    void showPhysical(int i);
    void showAction(bool b);

  protected:
    void Numerical(bool b);
    void Physical(bool b);

    Ui::QNewEntity* ui;
};

#endif // QNEWENTITY_H
