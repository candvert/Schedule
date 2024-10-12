#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QCloseEvent>
#include <QPushButton>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

signals:
    void buttonClicked();

public slots:
    void slotButtonClicked();

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    QString titleText();
    QString contentText();

protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::Form *ui;
    QString title;
    QString content;
};

#endif // FORM_H
