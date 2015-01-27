#ifndef QNEWDECORATOR_H
#define QNEWDECORATOR_H

#include <QWidget>

namespace Ui {
  class QNewDecorator;
}

class QNewDecorator : public QWidget
{
    Q_OBJECT

  public:
    explicit QNewDecorator(QWidget *parent = 0);
    ~QNewDecorator();

    int getType();
    bool getPartition();

    void setType(int);
    void setPartition(bool);

  private:
    Ui::QNewDecorator *ui;
};

#endif // QNEWDECORATOR_H
