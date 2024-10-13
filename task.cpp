#include "task.h"

Task::Task(QWidget *parent)
    : QWidget{parent}
{
    label = new QLabel("learning", this);
    checkBox = new QCheckBox(this);
    button = new QPushButton("删除", this);

    // 添加组件
    QHBoxLayout *layout = new QHBoxLayout(this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    layout->addWidget(checkBox);
    layout->addWidget(button);

    // 设置布局
    layout->setAlignment(label, Qt::AlignLeft);
    layout->setAlignment(checkBox, Qt::AlignRight);
    setLayout(layout);

    // 设置信号与槽
    connect(button, &QPushButton::clicked, this, [this](){
        QString title = label->text();
        emit deleteSignal(title);
        delete this;
    });
    connect(checkBox, &QCheckBox::clicked, this, [this](){emit checkedSignal();});
}

void Task::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QWidget *child = childAt(pos);

    // 鼠标左击，跳转详情页；鼠标右击，跳出菜单
    if (child) {
        if (child->isWidgetType()) {
            if (qobject_cast<QLabel*>(child)) {
                if (event->button() == Qt::LeftButton) {
                    emit editSignal();
                } else if (event->button() == Qt::RightButton) {
                    contextMenu = new QMenu(this);

                    // 添加菜单项
                    action = contextMenu->addAction("成为当前任务");
                    contextMenu->move(event->globalPos());
                    contextMenu->show();
                    connect(action, &QAction::triggered, this, &Task::actionSignal);
                }
            }
        }
    }
}
