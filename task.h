#ifndef TASK_H
#define TASK_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QMouseEvent>
#include <QDockWidget>
#include <QMenu>
#include <QAction>

class Task : public QWidget
{
    Q_OBJECT
public:
    explicit Task(QWidget *parent = nullptr);
    QAction *action;
    QCheckBox *checkBox;
    QLabel *label;

signals:
    void actionSignal();
    void deleteSignal(QString title);
    void checkedSignal();
    void editSignal();

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    QPushButton *button;
    QMenu *contextMenu;
};

#endif // TASK_H
