#include "QNewAction.h"
#include "ui_QNewAction.h"


QNewAction::QNewAction(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QNewAction)
{
  ui->setupUi(this);

  connect(ui->Add, SIGNAL(clicked()),
          this, SLOT(addTerminal()));

  connect(ui->Remove, SIGNAL(clicked()),
          this, SLOT(removeTerminal()));
}

QNewAction::~QNewAction()
{
  delete ui;
}

QListWidget* QNewAction::getParameter()
{
  return ui->ParameterList;
}

void QNewAction::setParameter(const QListWidget& l)
{
  ui->ParameterList->clear();

  for(int i=0; i<l.count();i++)
    ui->ParameterList->addItem(l.item(i));
}

void QNewAction::addTerminal()
{
  QListWidgetItem *item = new QListWidgetItem("New");
  item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  ui->ParameterList->addItem(item);
}

void QNewAction::removeTerminal()
{
  if(ui->ParameterList->selectedItems().empty())
    return;

  QListWidgetItem *item = ui->ParameterList->selectedItems().front();
  int row = ui->ParameterList->row(item);
  delete ui->ParameterList->takeItem(row);
}
