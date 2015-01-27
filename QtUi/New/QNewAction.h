#ifndef QNEWACTION_H
#define QNEWACTION_H

#include <QWidget>

class QListWidget;

namespace Ui {
  class QNewAction;
}

class QNewAction : public QWidget
{
    Q_OBJECT

  public:
    explicit QNewAction(QWidget *parent = 0);
    ~QNewAction();

    QListWidget* getParameter();

    void setParameter(const QListWidget& l);

  public slots:
    void removeTerminal();
    void addTerminal();

  private:
    Ui::QNewAction *ui;
};

#endif // QNEWACTION_H
