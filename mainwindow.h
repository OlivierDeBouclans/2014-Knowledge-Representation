/*
 * MainWindow
 *
 * This clas is used to build the program's main window.
 * It uses a .ui file for setup.
 *
 * V1:
 * The main window only display 1 QGraphicsView of the
 * Semantic Net.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Project includes
#include "SemanticNet.h"
#include "Graphview.h"
#include "QtUi/QDockInfoSemanticNet.h"
#include "QtUi/QDockInfoElement.h"
#include "QtUi/Widget/QWidgetSelectedItem.h"

//Qt includes
#include <QMainWindow>

namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  inline void pop() {m_GraphView.reArrangeSceneCircle();}

  int saveChangeBox();
  Ui::MainWindow *ui;

  public slots:
//    void setInformation(Entity *e);
//    void deleteInformation();

    void save();
    void saveAs();
    void open();
    void newNet();

    void fileHasChanged(bool b=true);
    void closeEvent(QCloseEvent* e);

  private:


    //Semantic net to represent
    SemanticNet             m_SemanticNet;
    //Graphical representation of the semantic net
    GraphView               m_GraphView;
    QDockInfoSemanticNet    m_SemanticInfo;
    QDockInfoElement        m_Information;

    int searchIndex;
    QString path;
    bool bfileHasChanged;

};

#endif // MAINWINDOW_H
