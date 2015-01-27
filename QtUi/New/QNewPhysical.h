#ifndef NEWPHYSICAL_H
#define NEWPHYSICAL_H

#include <QWidget>

namespace Ui {
  class QNewPhysical;
}

class QNewPhysical : public QWidget
{
    Q_OBJECT

  public:
    explicit QNewPhysical(QWidget *parent = 0);
    ~QNewPhysical();

    __int64 getStart();
    __int64 getEnd();

    void setStart(__int64);
    void setEnd(__int64);

  public slots:
    void showStart(int b);
    void showEnd(int b);

  private:
    Ui::QNewPhysical *ui;
};

#endif // NEWPHYSICAL_H
