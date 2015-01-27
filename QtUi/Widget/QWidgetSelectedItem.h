#ifndef QWIDGETSELECTEDITEM_H
#define QWIDGETSELECTEDITEM_H

#include <QWidget>

class SemanticElement;

namespace Ui {
  class QWidgetSelectedItem;
}

class QWidgetSelectedItem : public QWidget
{
    Q_OBJECT

  public:
    explicit QWidgetSelectedItem(QWidget *parent = 0);
    ~QWidgetSelectedItem();

  public slots:
    void display(SemanticElement* e);
    void unselect();

  signals:
    void unselectElement();

  private:
    Ui::QWidgetSelectedItem *ui;
    SemanticElement* m_pSelectedElement;

};

#endif // QWIDGETSELECTEDITEM_H
