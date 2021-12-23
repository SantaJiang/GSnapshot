#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
     void closeEvent(QCloseEvent *event);
     void changeEvent(QEvent * event);

private slots:
    void readyReadStandardOutput(const QString&);
    void readyReadStandardError(const QString&);
    void on_pushButton_select_clicked();
    void on_pushButton_ok_clicked(bool checked);
    void commit();

private:
    void initForm();

private:
    Ui::MainWindow *ui;
    QTimer* m_timer;
};

#endif // MAINWINDOW_H
