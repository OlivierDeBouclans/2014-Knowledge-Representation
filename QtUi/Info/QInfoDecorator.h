#ifndef QINFODECORATOR_H
#define QINFODECORATOR_H

#include <QWidget>

class Decorator;

namespace Ui {
  class QInfoDecorator;
}

class QInfoDecorator : public QWidget
{
    Q_OBJECT

  public:
    explicit QInfoDecorator(QWidget *parent = 0);
    ~QInfoDecorator();

    void display(Decorator* d);

  private:
    Ui::QInfoDecorator *ui;
};

#endif // QINFODECORATOR_H
