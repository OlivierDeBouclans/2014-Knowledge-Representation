
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <string>
#include <qdockwidget.h>

#include <assert.h>

#include <QFileDialog>
#include <qmessagebox.h>

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */

MainWindow::MainWindow(QWidget *parent) :
            QMainWindow(parent),
            ui(NULL),
            m_GraphView(&m_SemanticNet,this),
            m_SemanticInfo(this),
            m_Information(this),
            searchIndex(0),
            path(""),
            bfileHasChanged(false)
{
  //Build window, based on the .ui
  ui = new Ui::MainWindow();
  ui->setupUi(this);
  ui->centralWidget->layout()->addWidget(&m_GraphView);

  m_GraphView.updateAction();

  m_SemanticInfo.setSemanticNet(&m_SemanticNet);

  addDockWidget(Qt::LeftDockWidgetArea, &m_SemanticInfo);
  addDockWidget(Qt::RightDockWidgetArea,&m_Information);
  m_Information.hide();

  QMenu* m=new QMenu(this);
  m->addAction(ui->actionIn_circle);
  m->addAction(ui->actionIn_square);
  ui->actionRestart_Scene->setMenu(m);

  QMenu* m2=new QMenu(this);
  m2->addAction(ui->actionShow_In_Out);
  m2->addAction(ui->actionShow_In);
  m2->addAction(ui->actionShow_Out);
  ui->actionShow->setMenu(m2);

  ui->actionSave->setEnabled(false);
  ui->actionShow->setEnabled(false);

  //QT connection
  connect(&m_Information, SIGNAL(isShown(bool)),
          ui->actionDockInforamtion, SLOT(setChecked(bool)));

  connect(ui->actionDockInforamtion, SIGNAL(toggled(bool)),
          &m_Information, SLOT(hideOrShow()));

  connect(&m_SemanticInfo, SIGNAL(isShown(bool)),
          ui->actionDockSemanticNet, SLOT(setChecked(bool)));

  connect(ui->actionDockSemanticNet, SIGNAL(toggled(bool)),
          &m_SemanticInfo, SLOT(hideOrShow()));

  connect(&m_GraphView, SIGNAL(select(Entity*)),
          &m_SemanticInfo, SLOT(select(Entity*)));

  connect(&m_GraphView, SIGNAL(selectPOV(Entity*)),
          &m_SemanticInfo, SLOT(selectPointOfView(Entity*)));

  connect(ui->actionNew_Entity,SIGNAL(triggered()),
          &m_GraphView,        SLOT(newEntity()));

  connect(ui->actionIn_square, SIGNAL(triggered()),
          &m_GraphView,        SLOT(reArrangeSceneSquare()));

  connect(ui->actionIn_circle,SIGNAL(triggered()),
          &m_GraphView,           SLOT(reArrangeSceneCircle()));

  connect(ui->actionRestart_Scene,SIGNAL(triggered()),
          ui->actionIn_circle, SIGNAL(triggered()));

  connect(ui->actionShow_In, SIGNAL(triggered()),
          &m_GraphView,        SLOT(showIn()));

  connect(ui->actionShow_In_Out,SIGNAL(triggered()),
          &m_GraphView,           SLOT(showInOut()));

  connect(ui->actionShow_Out,SIGNAL(triggered()),
          &m_GraphView,           SLOT(showOut()));

  connect(ui->actionShow,SIGNAL(triggered()),
          ui->actionShow_In_Out, SIGNAL(triggered()));

  connect(ui->actionDelete,SIGNAL(triggered()),
          &m_GraphView,    SLOT(deleteElementFromNet()));

  connect(ui->actionHide,SIGNAL(triggered()),
          &m_GraphView,    SLOT(deleteElementFromGraph()));

  connect(ui->actionBring_to_back,SIGNAL(triggered()),
          &m_GraphView,    SLOT(bringBack()));

  connect(ui->actionBring_to_front,SIGNAL(triggered()),
          &m_GraphView,    SLOT(bringFront()));

  connect(ui->actionNew_Decorator,SIGNAL(triggered()),
          &m_GraphView,    SLOT(newDecorator()));

  connect(ui->actionNew_Edge,SIGNAL(triggered()),
          &m_GraphView,    SLOT(newEdge()));

  connect(ui->actionEdit,SIGNAL(triggered()),
          &m_GraphView,    SLOT(editElement()));

  connect(ui->actionInformation,SIGNAL(triggered()),
          &m_GraphView,    SLOT(getInfo()));

  connect(&m_GraphView,   SIGNAL(informationOn(SemanticElement*)),
          &m_Information, SLOT(display(SemanticElement*)));

  connect(&m_GraphView,   SIGNAL(netHasChanged()),
          &m_Information, SLOT(updateInfo()));

  connect(&m_GraphView,    SIGNAL(graphHasChanged()),
          &m_SemanticInfo, SLOT(updateTree()));

  connect(&m_GraphView,    SIGNAL(netHasChanged()),
          &m_SemanticInfo, SLOT(createTree()));

  connect(&m_GraphView,    SIGNAL(netHasChanged()),
          this, SLOT(fileHasChanged()));

  connect(ui->actionSave, SIGNAL(triggered()),
          this,           SLOT(save()));

  connect(ui->actionSaveAs, SIGNAL(triggered()),
          this,           SLOT(saveAs()));

  connect(ui->actionOpen, SIGNAL(triggered()),
          this,           SLOT(open()));

  connect(ui->actionNew, SIGNAL(triggered()),
          this,           SLOT(newNet()));

  connect(&m_SemanticInfo, SIGNAL(addOnGraph(Entity*)),
          &m_GraphView, SLOT(addOnGraph(Entity*)));

  connect(&m_SemanticInfo, SIGNAL(deleteFromGraph(GraphEntity*)),
          &m_GraphView, SLOT(deleteGraph(GraphEntity*)));
}


/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
  delete ui;
  ui=NULL;
}

void MainWindow::save()
{
  if(path == "")
   path = QFileDialog::getSaveFileName(this, "Save Net", QDir::currentPath(), "Text File (*.txt)");

  if(path.isEmpty())
    return;

  m_SemanticNet.saveNet(path.toStdString());
  fileHasChanged(false);
  m_SemanticInfo.setPath(path);

  setStatusTip("File saved");
}

void MainWindow::saveAs()
{
  QString s = QFileDialog::getSaveFileName(this, "Save Net", QDir::currentPath(), "Text File (*.txt)");

  if(s.isEmpty())
    return;

  path = s;
  m_SemanticNet.saveNet(path.toStdString());
  fileHasChanged(false);
  m_SemanticInfo.setPath(path);

  setStatusTip("File saved at "+path);
}

void MainWindow::open()
{
  if(bfileHasChanged)
    {
      int ret = saveChangeBox();

      if( ret == QMessageBox::Save)
        save();
      else if( ret == QMessageBox::Cancel)
        return;
    }

  path = QFileDialog::getOpenFileName(this, "Open Net", QDir::currentPath(), "Text File (*.txt)");

  if(!path.size())
    return;

  m_GraphView.clearScene();
  m_SemanticNet.openNet(path.toStdString());
  m_SemanticInfo.createTree();
  fileHasChanged(false);

  m_SemanticInfo.setPath(path);
  m_GraphView.reArrangeSceneCircle();

  setStatusTip(path+"'s File opened");
}

void MainWindow::newNet()
{
  if(bfileHasChanged)
    {
      int ret = saveChangeBox();

      if( ret == QMessageBox::Save)
        save();
      else if( ret == QMessageBox::Cancel)
        return;
    }

  m_GraphView.clearScene();
  m_SemanticNet = SemanticNet();
  m_SemanticInfo.createTree();
  m_Information.display();

  fileHasChanged(false);
  path="";
}

void MainWindow::fileHasChanged(bool b)
{
  bfileHasChanged = b;
  ui->actionSave->setEnabled(b);

  if(b)
    setStatusTip("");
}

void MainWindow::closeEvent(QCloseEvent* e)
{
  e->accept();

  if(bfileHasChanged)
    {

      int ret = saveChangeBox();

      if( ret == QMessageBox::Save)
        save();
      else if( ret == QMessageBox::Cancel)
        e->ignore();
    }
}

int MainWindow::saveChangeBox()
{
  QMessageBox msgBox;
  msgBox.setText("The net has been modified.");
  msgBox.setInformativeText("Do you want to save the changes?");
  msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Save);
  msgBox.setIcon(QMessageBox::Question);

  return msgBox.exec();
}
























