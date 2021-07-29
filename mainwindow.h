#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QMainWindow>
#include <QtWidgets>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSql>
#include <QtSql/QSqlDriver>
#include <QDebug>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QTableView>
#include <QtSql/QSqlError>
#include <QFile>
#include <QTextStream>
#include <QListWidgetItem>
#include <QScroller>
#include <QScrollBar>
#include <QPixmap>
#include <QTimer>
#include <QSettings>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QSettings *ini = new QSettings("//servidor/Users/Public/Acquatron Compartida/Andones/Resources/requerimientos.ini", QSettings::IniFormat);




public:
    QGridLayout *layoutvert_tira = new QGridLayout();




    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void popular_requerimientos();

    void popular_items();

    void crear_tira(int fila,bool visto);

    bool organizador(int numero_fila);

    void agregar_header();

    void limpiar_layout();

    void Abrir_ODBC();

private slots:
      void boton_visto_clicked(bool state);

      void check_soloproduccion_checked(bool state);

      void check_solonuevos_checked(bool state);

      void boton_produccion_clicked(bool state);

      void boton_vistageneral_checked(bool state);

      void check_apedidos_checked(bool state);

      void check_astock_checked(bool state);

      void funcionActivacionTimer();



private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
