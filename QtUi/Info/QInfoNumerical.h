#ifndef QINFONUMERICAL_H
#define QINFONUMERICAL_H

#include <QWidget>

class NumericalInfo;

namespace Ui {
  class QInfoNumerical;
}

class QInfoNumerical : public QWidget
{
    Q_OBJECT

  public:
    explicit QInfoNumerical(QWidget *parent = 0);
    ~QInfoNumerical();

    void display(NumericalInfo* e);

  private:
    Ui::QInfoNumerical *ui;
};

#endif // QINFONUMERICAL_H
