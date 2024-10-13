#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , isEditing(false)
    , db(QSqlDatabase::addDatabase("QSQLITE", "conn"))
    , query(db)
{
    ui->setupUi(this);

    form = new Form();
    db.setDatabaseName("record.db");
    if (!db.open()) {
        qDebug() << "Error: unalbe to open database";
    }

    // 加载数据
    todayLayout = new QVBoxLayout();
    weekLayout = new QVBoxLayout();
    monthLayout = new QVBoxLayout();
    loadData(todayLayout, "today");
    loadData(weekLayout, "week");
    loadData(monthLayout, "month");

    // 信号连接
    connect(ui->todayButton, &QPushButton::clicked, this, &Widget::todayButtonClicked);
    connect(ui->weekButton, &QPushButton::clicked, this, &Widget::weekButtonClicked);
    connect(ui->monthButton, &QPushButton::clicked, this, &Widget::monthButtonClicked);
    connect(form, &Form::buttonClicked, this, &Widget::addButtonClicked);
    connect(form, &Form::escapePressed, this, [this](){
        form->hide();
        this->move(form->pos());
        this->show();
    });

    // 设置布局
    ui->todayTasks->setLayout(todayLayout);
    ui->weekTasks->setLayout(weekLayout);
    ui->monthTasks->setLayout(monthLayout);

    // 读取当前任务
    QFile file("data.json");
    file.open(QIODevice::ReadOnly);
    QByteArray arr = file.readAll();
    ui->currentTask->setText(QJsonDocument::fromJson(arr).object()["currentTask"].toString());
    if (ui->currentTask->text() == "") {
        ui->currentTask->setText("当前任务：");
    }
    file.close();
}

Widget::~Widget()
{
    delete form;
    delete task;
    delete todayLayout;
    delete weekLayout;
    delete monthLayout;
    delete ui;
}

void Widget::loadData(QVBoxLayout *layout, QString tabel)
{
    // 加载数据
    query.exec(QString("CREATE TABLE IF NOT EXISTS %1 (id INTEGER PRIMARY KEY "
                            "AUTOINCREMENT, title TEXT, content TEXT, "
                            "checked INTEGER)").arg(tabel));
    query.exec(QString("SELECT title, content, checked FROM %1").arg(tabel));
    while (query.next()) {
        Task *task = new Task();
        task->label->setText(query.value(0).toString());
        task->checkBox->setChecked(query.value(2).toInt());
        layout->addWidget(task);
        connect(task, &Task::actionSignal, this, &Widget::actionClicked);
        connect(task, &Task::deleteSignal, this, &Widget::deleteTask);
        connect(task, &Task::checkedSignal, this, &Widget::checkBox);
        connect(task, &Task::editSignal, this, &Widget::editTask);
    }

    // 设置布局
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignHCenter);
    layout->setAlignment(Qt::AlignTop);
}

void Widget::actionClicked()
{
    Task *task = qobject_cast<Task*>(sender());
    if (task) {
        ui->currentTask->setText("当前任务：" + task->label->text());
    }
}

void Widget::checkBox()
{
    sqlOperation("UPDATE", qobject_cast<Task*>(sender()));
}

// 每个+按钮都会显示一个新界面
void Widget::todayButtonClicked()
{
    openedForm = 0;
    this->hide();
    form->move(this->pos());
    form->show();
    form->lineEdit()->setFocus();
}

void Widget::weekButtonClicked()
{
    openedForm = 1;
    this->hide();
    form->move(this->pos());
    form->show();
    form->lineEdit()->setFocus();
}

void Widget::monthButtonClicked()
{
    openedForm = 2;
    this->hide();
    form->move(this->pos());
    form->show();
    form->lineEdit()->setFocus();
}

void Widget::closeEvent(QCloseEvent*)
{
    // 将当前任务存放在data.json文件中
    QJsonObject obj;
    obj["currentTask"] = ui->currentTask->text();
    QFile file("data.json");
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(obj).toJson());
    file.close();
}

void Widget::addButtonClicked()
{
    if (isEditing == true) {
        QString table = sqlOperation("NULL", nullptr);

        query.prepare(QString("SELECT id FROM %1 WHERE title = ?").arg(table));
        query.addBindValue(task->label->text());
        query.exec();
        query.next();
        int id = query.value("id").toInt();

        if (form->titleText() == "") {
            query.prepare(QString("DELETE FROM %1 WHERE id = ?").arg(table));
            query.addBindValue(id);
            query.exec();
            delete task;
        } else {
            query.prepare(QString("UPDATE %1 SET title = ?, content = ? where id = ?").arg(table));
            query.addBindValue(form->titleText());
            query.addBindValue(form->contentText());
            query.addBindValue(id);
            query.exec();
            task->label->setText(form->titleText());
        }

        task = nullptr;
        isEditing = false;

        form->hide();
        this->move(form->pos());
        this->show();
        return;
    }

    if (form->titleText() == "") {
        form->hide();
        this->move(form->pos());
        this->show();
        return;
    }

    sqlOperation("INSERT", nullptr);

    Task *task = new Task();
    task->label->setText(form->titleText());
    connect(task, &Task::actionSignal, this, &Widget::actionClicked);
    connect(task, &Task::deleteSignal, this, &Widget::deleteTask);
    connect(task, &Task::checkedSignal, this, &Widget::checkBox);
    connect(task, &Task::editSignal, this, &Widget::editTask);
    if (openedForm == 0)
        todayLayout->addWidget(task);
    else if (openedForm == 1)
        weekLayout->addWidget(task);
    else if (openedForm == 2)
        monthLayout->addWidget(task);
    form->hide();
    this->move(form->pos());
    this->show();
}

void Widget::deleteTask(QString title)
{
    sqlOperation("DELETE", qobject_cast<Task*>(sender()));

    if (ui->currentTask->text() == "当前任务："+title) {
        ui->currentTask->setText("当前任务：");
    }
}

QString Widget::sqlOperation(QString operation, Task *task)
{
    // task不为空使用其parent判断操作的是哪张表
    QString current;
    if (task) {
        if (task->parent() == qobject_cast<QObject*>(ui->todayTasks)) {
            current = "today";
        } else if (task->parent() == qobject_cast<QObject*>(ui->weekTasks)) {
            current = "week";
        } else if (task->parent() == qobject_cast<QObject*>(ui->monthTasks)) {
            current = "month";
        }
    } else {
        if (openedForm == 0) {
            current = "today";
        } else if (openedForm == 1) {
            current = "week";
        } else if (openedForm == 2) {
            current = "month";
        }
    }

    if (operation == "NULL") return current;

    // 实际操纵数据库
    if (operation == "UPDATE") {
        query.prepare(QString("UPDATE %1 SET checked = ? where title = ?").arg(current));
        query.addBindValue(task->checkBox->isChecked());
        query.addBindValue(task->label->text());
        query.exec();
    } else if (operation == "INSERT") {
        query.prepare(QString("INSERT INTO %1 (title, content, checked)"
                                   " VALUES (?, ?, ?)").arg(current));
        query.addBindValue(form->titleText());
        query.addBindValue(form->contentText());
        query.addBindValue(0);
        query.exec();
    } else if (operation == "DELETE") {
        query.prepare(QString("DELETE FROM %1 WHERE title = ?").arg(current));
        query.addBindValue(task->label->text());
        query.exec();
    } else if (operation == "SELECT") {
        query.prepare(QString("SELECT title, content FROM %1 WHERE title = ?").arg(current));
        query.addBindValue(task->label->text());
        query.exec();
    }
    return current;
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_1) {
        todayButtonClicked();
    } else if (event->key() == Qt::Key_2) {
        weekButtonClicked();
    } else if (event->key() == Qt::Key_3) {
        monthButtonClicked();
    } else if (event->key() == Qt::Key_Escape) {
        this->close();
    }
}

void  Widget::editTask()
{
    isEditing = true;
    task = qobject_cast<Task*>(sender());

    QString table = sqlOperation("SELECT", qobject_cast<Task*>(sender()));
    query.next();
    form->setTitle(query.value("title").toString());
    form->setContent(query.value("content").toString());

    if (table == "today") openedForm = 0;
    else if (table == "week") openedForm = 1;
    else if (table == "month") openedForm = 2;

    this->hide();
    form->move(this->pos());
    form->show();
    form->lineEdit()->setFocus();
}
