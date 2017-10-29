#include "ui_windowmain.h"
#include "windowmain.h"

#include <Qt>
#include <QComboBox>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>
#include <QKeySequence>

WindowMain::WindowMain(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::Main) {
  ui->setupUi(this);

  ui->comboBox->addItems(QStringList() << "+" << "-" << "*" << "/");

  connect(ui->pushButton, &QPushButton::clicked, [this](bool) {
    auto a = ui->numA->text().toFloat();
    auto b = ui->numB->text().toFloat();
    switch (ui->comboBox->currentIndex()) {
    case 0: a += b; break;
    case 1: a -= b; break;
    case 2: a *= b; break;
    case 3: a /= b; break;
    default: break;
    }
    ui->result->setText(QString::number(a));
  });
}

WindowMain::~WindowMain() {
  delete ui;
}
