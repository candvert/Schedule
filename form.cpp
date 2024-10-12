#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &Form::slotButtonClicked);
}

Form::~Form()
{
    delete ui;
}

QString Form::titleText()
{
    return title;
}

QString Form::contentText()
{
    return content;
}

void Form::closeEvent(QCloseEvent *)
{
    ui->lineEdit->setText("");
    ui->textEdit->setPlainText("");
}

void Form::slotButtonClicked()
{
    title = ui->lineEdit->text();
    content = ui->textEdit->toPlainText();
    ui->lineEdit->setText("");
    ui->textEdit->setPlainText("");
    emit buttonClicked();
}

void Form::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        title = ui->lineEdit->text();
        content = ui->textEdit->toPlainText();
        ui->lineEdit->setText("");
        ui->textEdit->setPlainText("");
        emit buttonClicked();
    }
    QWidget::keyPressEvent(event);
}
