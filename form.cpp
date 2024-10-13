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

QWidget *Form::lineEdit()
{
    return ui->lineEdit;
}

void Form::hideEvent(QHideEvent *)
{
    ui->lineEdit->setText("");
    ui->textEdit->setPlainText("");
    emit escapePressed();
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
    } else if (event->key() == Qt::Key_Escape) {
        ui->lineEdit->setText("");
        ui->textEdit->setPlainText("");
        emit escapePressed();
    }
    QWidget::keyPressEvent(event);
}

bool Form::setTitle(const QString &title)
{
    ui->lineEdit->setText(title);
    return true;
}

bool Form::setContent(const QString &content)
{
    ui->textEdit->setPlainText(content);
    return true;
}
