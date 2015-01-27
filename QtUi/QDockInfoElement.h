#ifndef QDOCKINFOELEMENT_H
#define QDOCKINFOELEMENT_H

#include <QDockWidget>

class QWidget;
class SemanticElement;

namespace Ui
{
  class QDockInfoElement;
}

class QDockInfoElement : public QDockWidget
{
    Q_OBJECT

  public:
    explicit QDockInfoElement(QWidget *parent = 0);
    ~QDockInfoElement();

    void clear();

  public slots:
    void display(SemanticElement* e=NULL);
    void updateInfo();

    void hideOrShow();

  signals:
    void isShown(bool);

  protected:
    Ui::QDockInfoElement *ui;
    std::vector<QWidget*> m_vWidget;
    SemanticElement* m_pElement;
    void closeEvent(QCloseEvent* e);
};

#endif // QDOCKINFOELEMENT_H
