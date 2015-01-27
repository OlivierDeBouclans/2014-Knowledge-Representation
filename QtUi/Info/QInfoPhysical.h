#ifndef QINFOPHYSICAL_H
#define QINFOPHYSICAL_H

#include <QWidget>

class ConcreteInfo;

namespace Ui {
  class QInfoPhysical;
}

class QInfoConcrete : public QWidget
{
    Q_OBJECT

  public:
    explicit QInfoConcrete(QWidget *parent = 0);
    ~QInfoConcrete();

    void display(ConcreteInfo* e);

  private:
    Ui::QInfoPhysical *ui;
};

#endif // QINFOPHYSICAL_H
