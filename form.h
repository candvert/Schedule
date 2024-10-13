#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QHideEvent>
#include <QPushButton>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

signals:
    void buttonClicked();
    void escapePressed();

public slots:
    void slotButtonClicked();

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    QString titleText();
    QString contentText();
    bool setTitle(const QString &title);
    bool setContent(const QString &content);
    QWidget *lineEdit();

protected:
    virtual void hideEvent(QHideEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::Form *ui;
    QString title;
    QString content;
};

#endif // FORM_H
