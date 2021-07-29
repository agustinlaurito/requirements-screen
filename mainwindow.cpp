#include "mainwindow.h"
#include "ui_mainwindow.h"

int row,column;

int numero_columnas = 1;

#define NUMERO 0
#define EMPRESA 1
#define PEDIDO 2
#define EMPRESAPEDIDO 3
#define PLAZOENTREGA 4
#define NOTASADICIONALES 5
#define PRIORIDAD 6

int a = 0;
bool flag_minimizar = false, flag_enproduccion = false, flag_solonuevos = false, flag_solopedidos = false, flag_astock = false;
int cantidadrequerimientos = 0;

//----------- Variables ---------//
QStringList requerimiento[1000], elementos[10000];
QString items[1000][1000];
QSqlQuery consultaitems;




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Abrir_ODBC();
    QWidget *centralWidget = new QWidget(this);

    QVBoxLayout *verticalLayout = new QVBoxLayout(centralWidget);
    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);
    verticalLayout->addWidget(ui->scrollArea);
    MainWindow::setWindowTitle("Requerimientos");
    QTimer *cronometro=new QTimer(this);
    connect(cronometro, SIGNAL(timeout()), this, SLOT(funcionActivacionTimer()));
    cronometro->start(10000);

  ui->scrollArea->setWidgetResizable(true);
  agregar_header();
  popular_items();
  popular_requerimientos();

}

MainWindow::~MainWindow()
{
    delete ui;



}

void MainWindow::Abrir_ODBC(){


    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC3");
    db.setHostName("servidor");
    db.setDatabaseName("DRIVER={MYSQL ODBC 3.51 Driver};Server=servidor;database=ACQUATRON");
    db.setPassword("");
    db.setUserName("root");

    if (db.open())
    {
    qDebug() << "Enhorabuena";
    }
    else
    {

       qDebug() << db.lastError().text();
    }

}

void MainWindow::popular_items(){

    QString consultaprevia = "SELECT requerimientos.id,empresas.empresa, requerimientositems.cantidad, requerimientositems.codigo, requerimientositems.descripcion FROM empresas INNER JOIN contactos ON (contactos.idempresa=empresas.idempresa) INNER JOIN requerimientos ON (requerimientos.idref=contactos.idcontacto) INNER JOIN requerimientositems ON (requerimientositems.idrequerimiento=requerimientos.recid) LEFT JOIN info on info.IDRef=requerimientos.recid WHERE empresas.empresa = \"Trabajos Internos ##\" AND requerimientos.estado = 0 ORDER BY (CASE info.campo2 WHEN '' THEN 5 ELSE info.campo2 END) ASC, requerimientos.id DESC";
    QSqlQuery consulta;
    int columnas = 0,fila = 0,columnacolocada = 0;

    consulta.exec(consultaprevia);
    while(consulta.next()){
        int pedidoactual = consulta.value(0).toInt();


        while(!consulta.value(columnas).isNull()){
            items[fila][columnacolocada] = consulta.value(columnas).toString();

            columnas++;
            columnacolocada++;
        }
        columnas = 0;

        int pedidosiguiente = 0;

        if(consulta.next()){

            pedidosiguiente = consulta.value(0).toInt();
            consulta.previous();
        }

        if(pedidoactual != pedidosiguiente){
            fila++;
            columnacolocada = 0;
        }
    }
}

void MainWindow::popular_requerimientos(){

    int fila = 0;

    QSqlQuery consulta;
    consulta.prepare("SELECT requerimientos.id AS RequerimientoNumero,empresas.empresa AS Encargados,COALESCE(pedidos.id, ''), COALESCE(empresapedido.Empresa, ''),COALESCE(comercial.plazoentrega, ''),COALESCE(info.campo1, ''),COALESCE(info.campo2, 5) FROM empresas INNER JOIN contactos ON (contactos.idempresa=empresas.idempresa)INNER JOIN requerimientos ON (requerimientos.idref=contactos.idcontacto)INNER JOIN requerimientositems ON (requerimientositems.idrequerimiento=requerimientos.recid)LEFT JOIN requerimientositemsclientes ON requerimientositemsclientes.IDReqProd=requerimientositems.RecID LEFT JOIN pedidositems ON requerimientositemsclientes.IDPedProd=pedidositems.RecID LEFT JOIN pedidos ON pedidositems.IDPedido=pedidos.RecID LEFT JOIN contactos contactopedido ON contactopedido.IDContacto=pedidos.IDRef LEFT JOIN empresas empresapedido ON empresapedido.IDEmpresa=contactopedido.IDEmpresa LEFT JOIN comercial ON comercial.idref=requerimientos.recid LEFT JOIN info on info.IDRef=requerimientos.recid WHERE empresas.empresa = \"Trabajos Internos ##\" AND requerimientos.estado = 0 GROUP BY requerimientos.id ORDER BY (CASE info.campo2 WHEN '' THEN 5 ELSE info.campo2 END) ASC, requerimientos.id DESC");
    consulta.exec();
    column = 0;
    row = 1;

    cantidadrequerimientos = 0; // Para contabilizar la posicion 0 tambien ya que en esta tambien hay datos


     while(consulta.next())
        cantidadrequerimientos++;


    consulta.seek(-1); // Voy a la -1 para que despues al darle a next vaya a la 0

    while(consulta.next()){
        int iteracion_menor = 0;


        while(!consulta.value(iteracion_menor).toString().isNull()){
            if(iteracion_menor < requerimiento[fila].size()) //Con este IF limpio la proxima posicion del vector
                requerimiento[fila].removeAt(iteracion_menor);

            requerimiento[fila].insert(iteracion_menor,consulta.value(iteracion_menor).toString());
            iteracion_menor++;
            while(int l = requerimiento[fila].size() > iteracion_menor)
                requerimiento[fila].removeAt(l--);

        }

        crear_tira(fila, organizador(fila));
        fila++;


    }


}


void MainWindow::crear_tira(int fila,bool visto){



    //QVBoxLayout *layoutvert_tira = new QVBoxLayout();
    QPixmap Acqualogo (":/images/cajarequerimiento.png");
    QPixmap conentregalogo (":/images/calendario.png");
    QPixmap sinentregalogo (":/images/sinfecha");
    QPixmap infologo (":/images/info.png");
    QPixmap checkboxempty(":/images/checkboxempty.png");
    QPixmap checkboxcheck(":/images/checkboxcheck.png");
    QPixmap minimize(":/images/minimizar.png");



    QGroupBox *box = new QGroupBox();
    layoutvert_tira->setSpacing(20);

    // Declaro los Layout //

    QHBoxLayout *layouthor_logo = new QHBoxLayout();

    QHBoxLayout *layouthor_encabezado = new QHBoxLayout();
    QHBoxLayout *layouthor_cuerpo = new QHBoxLayout();
    QHBoxLayout *layouthor_info = new QHBoxLayout();
    QVBoxLayout *tira = new QVBoxLayout();
    QHBoxLayout *layouthor_checkbox = new QHBoxLayout();
    tira->setSpacing(3);

    // Levanto las variables //



    QString numero_requerimiento = requerimiento[fila][NUMERO];
    QString pedido_requerimiento = requerimiento[fila][PEDIDO];
    QString empresapedido_requerimiento = requerimiento[fila][EMPRESAPEDIDO];
    QString plazoentrega_requerimiento = requerimiento[fila][PLAZOENTREGA];
    QString notas_requerimiento = requerimiento[fila][NOTASADICIONALES];
    QString prioridad_requerimiento = requerimiento[fila][PRIORIDAD];

    ini->beginGroup(numero_requerimiento);
    bool producir = ini->value("/Producir").toBool();
    ini->endGroup();


    if(flag_enproduccion and !producir)
            return;

    if(flag_solonuevos and visto)
            return;

    if(flag_solopedidos and empresapedido_requerimiento.isEmpty())
            return;
    if(flag_astock and !empresapedido_requerimiento.isEmpty() and !pedido_requerimiento.isEmpty())
            return;

     box->setObjectName("Box_normal");

    if(empresapedido_requerimiento.isEmpty()){
        empresapedido_requerimiento = "Stock";
    }else{
        pedido_requerimiento = "Responde a Pedido: " + pedido_requerimiento;
    }


    int prioridad = prioridad_requerimiento.toInt();
    if(prioridad_requerimiento.isEmpty())
        prioridad = 5;
    if(prioridad < 1){
        prioridad_requerimiento = "Urgente";
        box->setObjectName("Box_urgente");
    }else{
        if(prioridad <= 2){
            prioridad_requerimiento = "Prioridad Alta";
            box->setObjectName("Box_media");
            }
        }
    if(prioridad > 2)
        prioridad_requerimiento = "Prioridad Normal";


    // Preparo los botones //

    QCheckBox *check_visto = new QCheckBox("Visto");
    check_visto->setObjectName("Check_visto." + numero_requerimiento);
    check_visto->setChecked(visto);


    connect(check_visto, SIGNAL(clicked(bool)),
           this,SLOT(boton_visto_clicked(bool)));

    QCheckBox *check_producir = new QCheckBox("En Produccion");
    check_producir->setObjectName("check_producir." + numero_requerimiento);
    check_producir->setChecked(producir);

    connect(check_producir, SIGNAL(clicked(bool)),
           this,SLOT(boton_produccion_clicked(bool)));


    //Preparo los labels //

    QLabel *lbl_plazologo = new QLabel();
    lbl_plazologo->setObjectName("lbl_plazologo." + numero_requerimiento);
    lbl_plazologo->setScaledContents(true);
    lbl_plazologo->setFixedSize(30,30);
    if(plazoentrega_requerimiento.isEmpty()){
        lbl_plazologo->setPixmap(sinentregalogo);
    }else{
        lbl_plazologo->setPixmap(conentregalogo);
    }

    QLabel *lbl_infologo = new QLabel();
    lbl_infologo->setObjectName("lbl_infologo." + numero_requerimiento);
    lbl_infologo->setPixmap(infologo);
    lbl_infologo->setFixedSize(30,30);
    lbl_infologo->setScaledContents(true);


    QLabel *lbl_notas = new QLabel(notas_requerimiento);
    lbl_notas->setObjectName("lbl_notas");
    lbl_notas->setWordWrap(true);

    QLabel *lbl_numero = new QLabel("Requerimiento Interno N°: " + numero_requerimiento);
    lbl_numero->setObjectName("lbl_numero");
    QLabel *lbl_pedido = new QLabel(pedido_requerimiento);
    lbl_pedido->setObjectName("lbl_pedido");

    QLabel *lbl_empresapedido = new QLabel(empresapedido_requerimiento);

    lbl_empresapedido->setObjectName("lbl_empresapedido");
    if(producir){
        lbl_empresapedido->setText("[En Produccion] " + lbl_empresapedido->text());
    }
    if(!visto){
        lbl_empresapedido->setText("[NUEVO] " + lbl_empresapedido->text());
    }
    if(flag_minimizar)
        lbl_empresapedido->setWordWrap(true);


    QLabel *lbl_plazoentrega = new QLabel(plazoentrega_requerimiento);
    if(plazoentrega_requerimiento.isEmpty()){
        lbl_plazoentrega->setObjectName("lbl_sinplazoentrega");
        lbl_plazoentrega->setText("Sin Plazo de Entrega Definido");
    }else{
        lbl_plazoentrega->setObjectName("lbl_conplazoentrega");
    }

    QLabel *lbl_prioridad = new QLabel(prioridad_requerimiento);
    lbl_prioridad->setObjectName("lbl_prioridad");



    // Label del logo //

    QLabel *lbl_acquatron = new QLabel();
    lbl_acquatron->setPixmap(Acqualogo);
    lbl_acquatron->setScaledContents(true);
    lbl_acquatron->setFixedSize(46,46);

    //-------------------------------------------- Items -----------------------------------------------------//


        QGroupBox *box_item = new QGroupBox;
        box_item->setObjectName("box_item");
        QVBoxLayout *layoutvert_items = new QVBoxLayout;

        QString cant = "";

        int u = 0;

        while(!items[fila][u].isEmpty()){
            QLabel *lbl_item = new QLabel;
            lbl_item->setObjectName("lbl_item");
            QString v = items[fila][u];


            if(((u-2) % 5 == 0 or u == 2) and u){
              cant.append(" -->   " + v );
              cant.append("  •   " );
            }
            if(((u-3) % 5 == 0 or u == 3) and u){
              cant.append(v );
              cant.append("  •   " );
            }
            if(((u-4) % 5 == 0 or u == 4) and u){
                cant.append(v);
                QString n = "";
                if(cant.contains("\n")){
                    for(int a = cant.length() - v.length() + 9; a >= 0 ;a--)
                        n.append(" ");
                    cant.replace("\n\n","\n");
                    cant.replace("\n",("\n" + n));
                    //cant.append("\n");
                }


              lbl_item->setText(cant);
              lbl_item->setWordWrap(true);
              layoutvert_items->addWidget(lbl_item);
              cant = "";
            }
            u++;
        }

        box_item->setLayout(layoutvert_items);


    //--------------------------------------------------------------------------------------------------------//


    // Agrego los widgets al layout //

    layouthor_logo->addWidget(lbl_acquatron);
    if(!flag_minimizar)
        layouthor_logo->addStretch();
    layouthor_logo->addWidget(lbl_empresapedido);
    layouthor_logo->addStretch();
    layouthor_logo->addWidget(check_visto);

    layouthor_encabezado->addWidget(lbl_numero);
    layouthor_encabezado->addStretch();
    if(!flag_minimizar)
        layouthor_encabezado->addWidget(lbl_pedido);

    layouthor_checkbox->addWidget(check_producir);
    layouthor_checkbox->addStretch();

    layouthor_cuerpo->addWidget(lbl_plazologo);
    layouthor_cuerpo->addSpacing(10);
    layouthor_cuerpo->addWidget(lbl_plazoentrega);
    layouthor_cuerpo->addStretch();
    layouthor_cuerpo->addWidget(lbl_prioridad);

    if(!notas_requerimiento.isEmpty()){
        layouthor_info->addWidget(lbl_infologo);
        layouthor_info->addSpacing(10);
        layouthor_info->addWidget(lbl_notas);
       }

    tira->addLayout(layouthor_logo);
    tira->addLayout(layouthor_encabezado);
    tira->addSpacing(2);
    if(!flag_minimizar)
        tira->addLayout(layouthor_checkbox);
    if(!flag_minimizar)
        tira->addLayout(layouthor_cuerpo);
    if(!flag_minimizar)
        tira->addLayout(layouthor_info);
    if(!flag_minimizar)
        tira->addWidget(box_item);


    box->setLayout(tira);

    if(!visto)
        box->setObjectName("Box_urgente");

    if(layoutvert_tira->itemAtPosition(row,column)){
        QLayoutItem* item;
        item = layoutvert_tira->itemAtPosition(row,column);
        delete item->widget();
        delete item;
    }


    layoutvert_tira->addWidget(box,row,column);



    row++;
    if(row > cantidadrequerimientos/numero_columnas){
        row = 1;
        column++;
    }

    ui->scrollContents->setLayout(layoutvert_tira);

}

void MainWindow::agregar_header(){

    QPixmap Acquatronconlogo(":/images/acquatron con nombre.png");

    QLabel *lbl_acquatronconlogo = new QLabel();
    lbl_acquatronconlogo->setPixmap(Acquatronconlogo);
    lbl_acquatronconlogo->setScaledContents(true);
    lbl_acquatronconlogo->setFixedSize(1031/7,325/7);

    QGridLayout *layout_opciones = new QGridLayout;


    QCheckBox *check_minimize = new QCheckBox("Vista General");
    QCheckBox *check_soloproduccion = new QCheckBox("En Produccion");
    QCheckBox *check_solonuevos = new QCheckBox("Solo Nuevos");
    QCheckBox *check_apedidos = new QCheckBox("Solo para Pedidos");
    QCheckBox *check_astock = new QCheckBox("Solo para Stock");

    QGroupBox *group_opciones = new QGroupBox();
    group_opciones->setMaximumHeight(75);


    connect(check_minimize, SIGNAL(clicked(bool)),
           this,SLOT(boton_vistageneral_checked(bool)));

    connect(check_soloproduccion, SIGNAL(clicked(bool)),
           this,SLOT(check_soloproduccion_checked(bool)));

    connect(check_solonuevos, SIGNAL(clicked(bool)),
           this,SLOT(check_solonuevos_checked(bool)));

    connect(check_apedidos, SIGNAL(clicked(bool)),
           this,SLOT(check_apedidos_checked(bool)));

    connect(check_astock, SIGNAL(clicked(bool)),
           this,SLOT(check_astock_checked(bool)));

    group_opciones->setObjectName("group_opciones");

    layout_opciones->addWidget(check_minimize,0,0);
    layout_opciones->spacerItem();
    layout_opciones->addWidget(check_soloproduccion,0,2);
    layout_opciones->addWidget(check_solonuevos,0,3);

    layout_opciones->addWidget(check_apedidos,1,2);
    layout_opciones->addWidget(check_astock,1,3);

    group_opciones->setLayout(layout_opciones);

    layoutvert_tira->addWidget(group_opciones);

}


bool MainWindow::organizador(int numero_fila){  //Setea el .INI y devuelve TRUE en caso de ser un pedido nuevo y/o no haberlo visto

    QString numero_requerimiento = requerimiento[numero_fila][NUMERO];
    QString numero_prioridad = requerimiento[numero_fila][PRIORIDAD];


    if(!ini->childGroups().contains(numero_requerimiento)){
        ini->beginGroup(numero_requerimiento);
        ini->setValue("/nuevo",true);
        ini->setValue("/visto",false);
        ini->setValue("/Producir",false);
        ini->setValue("/Pin",false);
        ini->endGroup();
        return(ini->value("/" + numero_requerimiento + "/visto").toBool());

    }else{
        return(ini->value("/" + numero_requerimiento + "/visto").toBool());
    }

}


void MainWindow::boton_visto_clicked(bool state){


    QObject *obj = sender();
    QString requirement = obj->objectName().split(".")[1];
    ini->beginGroup(requirement);
    ini->setValue("/visto",state);
    ini->endGroup();
    popular_requerimientos();
}


void MainWindow::boton_produccion_clicked(bool state){

    QObject *obj = sender();
    QString requirement = obj->objectName().split(".")[1];
    ini->beginGroup(requirement);
    ini->setValue("/Producir",state);
    ini->endGroup();
    popular_requerimientos();
}


void MainWindow::boton_vistageneral_checked(bool state){

    flag_minimizar = state;

    numero_columnas = 1;
    if(state)
        numero_columnas = 2;

    limpiar_layout();
    popular_requerimientos();


}


void MainWindow::check_soloproduccion_checked(bool state){

    flag_enproduccion = state;
    limpiar_layout();
    popular_requerimientos();

}

void MainWindow::check_solonuevos_checked(bool state){


    flag_solonuevos = state;
    limpiar_layout();
    popular_requerimientos();

}

void MainWindow::check_astock_checked(bool state){

    flag_astock = state;
    limpiar_layout();
    popular_requerimientos();

}


void MainWindow::check_apedidos_checked(bool state){

    flag_solopedidos = state;
    limpiar_layout();
    popular_requerimientos();

}

void MainWindow::limpiar_layout(){

    for(int z = layoutvert_tira->rowCount(); z > 0;z--){

        for(int y = layoutvert_tira->columnCount(); y >= 0; y--){
            if(layoutvert_tira->itemAtPosition(z,y)){
                QLayoutItem* item;
                item = layoutvert_tira->itemAtPosition(z,y);
                delete item->widget();
                delete item;
            }
        }


    }
}


void MainWindow::funcionActivacionTimer(){

   if(flag_astock or flag_enproduccion or flag_minimizar or flag_solonuevos or flag_solopedidos) // Solucion al problema de que se bloquea al activar un filtro
       limpiar_layout();
   popular_items();
   popular_requerimientos();

}
