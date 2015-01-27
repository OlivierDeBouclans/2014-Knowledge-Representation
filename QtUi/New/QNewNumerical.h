#ifndef QNEWNUMERICAL_H
#define QNEWNUMERICAL_H

#include <QWidget>

namespace Ui {
  class QNewNumerical;
}

class QNewNumerical : public QWidget
{
    Q_OBJECT

  public:
    explicit QNewNumerical(QWidget *parent = 0);
    ~QNewNumerical();

    double getMin();
    double getMax();

    void setMin(double);
    void setMax(double);

  private:
    Ui::QNewNumerical *ui;
};

#endif // QNEWNUMERICAL_H
