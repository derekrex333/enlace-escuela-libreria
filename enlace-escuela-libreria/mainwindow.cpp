/*  MainWindow.cpp
    Sistema Enlace Escuela-Libreria
    Interfaz Qt con login por rol, galeria de libros y PostgreSQL.
*/

#include "MainWindow.h"
#include "Escuela.h"

#include <QApplication>
#include <QString>
#include <QDate>
#include <QHeaderView>
#include <QNetworkRequest>
#include <QUrl>
#include <QPainter>
#include <QPainterPath>

//  TarjetaLibro
TarjetaLibro::TarjetaLibro(const QString &titulo,
                           const QString &autor,
                           const QString &categoria,
                           const QString &imagenUrl,
                           QWidget *parent)
    : QFrame(parent)
{
    setObjectName("tarjetaLibro");
    setFixedSize(210, 310);
    setFrameShape(QFrame::NoFrame);

    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setSpacing(6);

    // Portada
    lblPortada = new QLabel;
    lblPortada->setFixedSize(186, 190);
    lblPortada->setAlignment(Qt::AlignCenter);
    lblPortada->setObjectName("portadaLibro");
    lblPortada->setText("Cargando...");
    lay->addWidget(lblPortada);

    // Titulo
    lblTitulo = new QLabel(titulo);
    lblTitulo->setObjectName("lblTituloLibro");
    lblTitulo->setWordWrap(true);
    lblTitulo->setMaximumWidth(186);
    lay->addWidget(lblTitulo);

    // Autor
    lblAutor = new QLabel(autor);
    lblAutor->setObjectName("lblAutorLibro");
    lay->addWidget(lblAutor);

    // Categoria
    lblCategoria = new QLabel(categoria);
    lblCategoria->setObjectName("lblCategoriaLibro");
    lay->addWidget(lblCategoria);

    lay->addStretch();

    // Sombra
    QGraphicsDropShadowEffect *sombra = new QGraphicsDropShadowEffect(this);
    sombra->setBlurRadius(22);
    sombra->setOffset(0, 4);
    sombra->setColor(QColor(0, 0, 0, 38));
    setGraphicsEffect(sombra);

    // Imagen
    nam = new QNetworkAccessManager(this);
    if (!imagenUrl.isEmpty())
        cargarImagen(imagenUrl);
    else
        lblPortada->setText("Sin imagen");
}

void TarjetaLibro::cargarImagen(const QString &url)
{
    connect(nam, &QNetworkAccessManager::finished,
            this, &TarjetaLibro::onImagenDescargada);
    nam->get(QNetworkRequest(QUrl(url)));
}

void TarjetaLibro::onImagenDescargada(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QPixmap px;
        if (px.loadFromData(reply->readAll()))
        {
            lblPortada->setPixmap(
                px.scaled(186, 190, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)
                    .copy(0, 0, 186, 190)
                );
        }
    }
    else
    {
        lblPortada->setText("Sin imagen");
    }
    reply->deleteLater();
}

//  MainWindow — Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Enlace Escuela-Libreria");
    setMinimumSize(960, 680);
    resize(1100, 750);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Crear paginas en el orden de Idx
    paginaLogin     = crearPaginaLogin();
    paginaMenu      = crearPaginaMenu();
    paginaLibros    = crearPaginaLibros();
    paginaRegLibro  = crearPaginaRegLibro();
    paginaRegEscuela= crearPaginaRegEscuela();
    paginaRegVale   = crearPaginaRegVale();
    paginaBuscar    = crearPaginaBuscar();
    paginaEscuelas  = crearPaginaEscuelas();
    paginaVales     = crearPaginaVales();
    paginaReporte   = crearPaginaReporte();
    paginaResumen   = crearPaginaResumen();
    paginaPedir     = crearPaginaPedir();

    stackedWidget->addWidget(paginaLogin);      // 0
    stackedWidget->addWidget(paginaMenu);       // 1
    stackedWidget->addWidget(paginaLibros);     // 2
    stackedWidget->addWidget(paginaRegLibro);   // 3
    stackedWidget->addWidget(paginaRegEscuela); // 4
    stackedWidget->addWidget(paginaRegVale);    // 5
    stackedWidget->addWidget(paginaBuscar);     // 6
    stackedWidget->addWidget(paginaEscuelas);   // 7
    stackedWidget->addWidget(paginaVales);      // 8
    stackedWidget->addWidget(paginaReporte);    // 9
    stackedWidget->addWidget(paginaResumen);    // 10
    stackedWidget->addWidget(paginaPedir);      // 11

    stackedWidget->setCurrentIndex(IDX_LOGIN);
    aplicarEstilos();

    sistema.conectarDB("localhost", "5432",
                       "enlace_libreria",
                       "postgres",
                       "Dek3");
}

MainWindow::~MainWindow() {}

//  Helpers visuales
void MainWindow::agregarSombra(QWidget *w, int radio, qreal op)
{
    QGraphicsDropShadowEffect *s = new QGraphicsDropShadowEffect(w);
    s->setBlurRadius(radio);
    s->setOffset(0, 3);
    s->setColor(QColor(0, 0, 0, static_cast<int>(op * 255)));
    w->setGraphicsEffect(s);
}

QLabel* MainWindow::crearTitulo(const QString &texto)
{
    QLabel *lbl = new QLabel(texto);
    lbl->setObjectName("tituloPagina");
    return lbl;
}

QPushButton* MainWindow::crearBotonMenu(const QString &texto)
{
    QPushButton *btn = new QPushButton(texto);
    btn->setObjectName("btnMenu");
    btn->setMinimumHeight(52);
    btn->setMinimumWidth(340);
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
}

//  Estilos globales
void MainWindow::aplicarEstilos()
{
    qApp->setStyleSheet(R"(

/* ---- Base ---- */
QMainWindow, QDialog {
    background-color: #f0f4f8;
    font-family: "Georgia", "Times New Roman", serif;
    font-size: 15px;
    color: #1a202c;
}

QWidget {
    background-color: transparent;
    color: #1a202c;
    font-family: "Georgia", "Times New Roman", serif;
    font-size: 15px;
}

/* ---- Pagina de Login ---- */
QWidget#paginaLoginRoot {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #1a1a2e, stop:0.5 #16213e, stop:1 #0f3460);
}

QFrame#cardLogin {
    background-color: rgba(255,255,255,0.06);
    border: 1px solid rgba(255,255,255,0.14);
    border-radius: 24px;
    padding: 12px;
}

QLabel#loginTitulo {
    font-size: 30px;
    font-weight: bold;
    color: #e8d5b0;
    letter-spacing: 2px;
}

QLabel#loginSub {
    font-size: 14px;
    color: rgba(232,213,176,0.65);
    letter-spacing: 1px;
}

QLabel#loginEtiqueta {
    color: rgba(232,213,176,0.8);
    font-size: 13px;
}

QLineEdit#inpPasswordLogin {
    background-color: rgba(255,255,255,0.10);
    border: 1px solid rgba(232,213,176,0.30);
    border-radius: 10px;
    padding: 10px 14px;
    color: #f5ebe0;
    font-size: 15px;
}

QLineEdit#inpPasswordLogin:focus {
    border: 1.5px solid #e8d5b0;
}

/* Botones de rol */
QPushButton#btnRol {
    background-color: rgba(255,255,255,0.07);
    color: rgba(232,213,176,0.75);
    border: 1px solid rgba(232,213,176,0.20);
    border-radius: 10px;
    padding: 10px 18px;
    font-size: 14px;
    font-weight: 600;
}

QPushButton#btnRol:checked, QPushButton#btnRol:pressed {
    background-color: #e8d5b0;
    color: #1a1a2e;
    border: none;
}

QPushButton#btnRol:hover {
    background-color: rgba(232,213,176,0.15);
}

QPushButton#btnLoginAccion {
    background-color: #e8d5b0;
    color: #1a1a2e;
    border: none;
    border-radius: 12px;
    padding: 14px 0;
    font-size: 16px;
    font-weight: bold;
    letter-spacing: 1px;
}

QPushButton#btnLoginAccion:hover {
    background-color: #f5ebe0;
}

QPushButton#btnLoginAccion:pressed {
    background-color: #d4b896;
}

/* ---- Menu principal ---- */
QWidget#paginaMenuRoot {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #f0f4f8, stop:1 #dce8f0);
}

QLabel#tituloPagina {
    font-size: 27px;
    font-weight: bold;
    color: #1a3a5c;
    padding: 14px 0 8px;
    letter-spacing: 1px;
}

QLabel#subtituloMenu {
    font-size: 15px;
    color: #4a6a8a;
    padding-bottom: 8px;
}

QPushButton#btnMenu {
    background-color: #ffffff;
    color: #1a3a5c;
    border: 1px solid #cddcec;
    border-radius: 14px;
    padding: 14px 28px;
    font-size: 15px;
    font-weight: 600;
    text-align: left;
}

QPushButton#btnMenu:hover {
    background-color: #e8f0fa;
    border: 1.5px solid #3b82c4;
    color: #1a3a5c;
}

QPushButton#btnMenu:pressed {
    background-color: #dbeafe;
}

/* ---- Paginas internas ---- */
QFrame#panelInterno {
    background-color: #ffffff;
    border-radius: 18px;
    border: 1px solid #dce3ec;
}

QGroupBox {
    border: 1.5px solid #dce3ec;
    border-radius: 14px;
    margin-top: 20px;
    padding-top: 16px;
    background-color: #ffffff;
    font-size: 14px;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 0 12px;
    color: #1a3a5c;
    font-weight: bold;
    background-color: #ffffff;
    border-radius: 4px;
}

/* ---- Inputs ---- */
QLineEdit, QTextEdit, QPlainTextEdit,
QSpinBox, QDoubleSpinBox, QComboBox {
    background-color: #f7fafc;
    color: #1a202c;
    border: 1.5px solid #cdd8e3;
    border-radius: 9px;
    padding: 9px 13px;
    font-size: 14px;
}

QLineEdit:focus, QTextEdit:focus,
QSpinBox:focus, QDoubleSpinBox:focus {
    border: 2px solid #3b82c4;
    background-color: #ffffff;
}

/* ---- Botones generales ---- */
QPushButton {
    background-color: #e8f0fa;
    color: #1a3a5c;
    border: 1px solid #cddcec;
    border-radius: 11px;
    padding: 10px 22px;
    font-size: 14px;
    font-weight: 600;
}

QPushButton:hover {
    background-color: #dbeafe;
    border: 1.5px solid #3b82c4;
}

QPushButton#btnAccion {
    background-color: #2d6a4f;
    color: #ffffff;
    border: none;
    font-weight: bold;
}

QPushButton#btnAccion:hover  { background-color: #1b4332; }
QPushButton#btnAccion:pressed { background-color: #143727; }

QPushButton#btnVolver {
    background-color: #c9184a;
    color: #ffffff;
    border: none;
    font-weight: 600;
}

QPushButton#btnVolver:hover { background-color: #a4133c; }

QPushButton#btnSalir {
    background-color: #6b21a8;
    color: #ffffff;
    border: none;
    font-weight: 600;
}

QPushButton#btnSalir:hover { background-color: #581c87; }

/* ---- Tarjeta Libro ---- */
QFrame#tarjetaLibro {
    background-color: #ffffff;
    border: 1px solid #e0e7ef;
    border-radius: 16px;
}

QFrame#tarjetaLibro:hover {
    border: 1.5px solid #3b82c4;
    background-color: #f8fbff;
}

QLabel#portadaLibro {
    background-color: #e8eef5;
    border-radius: 10px;
    color: #8a9ab0;
    font-size: 12px;
}

QLabel#lblTituloLibro {
    font-size: 13px;
    font-weight: bold;
    color: #1a3a5c;
    line-height: 1.3;
}

QLabel#lblAutorLibro {
    font-size: 12px;
    color: #4a6a8a;
    font-style: italic;
}

QLabel#lblCategoriaLibro {
    font-size: 11px;
    color: #ffffff;
    background-color: #3b82c4;
    border-radius: 6px;
    padding: 2px 8px;
}

/* ---- Tablas ---- */
QTableWidget {
    background-color: #ffffff;
    gridline-color: #e2eaf2;
    border: 1px solid #dce3ec;
    selection-background-color: #dbeafe;
    selection-color: #1a202c;
    border-radius: 10px;
}

QHeaderView::section {
    background-color: #e8f0fa;
    color: #1a3a5c;
    padding: 10px 8px;
    border: none;
    font-weight: bold;
    font-size: 13px;
}

/* ---- Scrollbars ---- */
QScrollBar:vertical {
    background: #f0f4f8;
    width: 10px;
    border-radius: 5px;
}

QScrollBar::handle:vertical {
    background: #aab8c8;
    border-radius: 5px;
    min-height: 36px;
}

QScrollBar::handle:vertical:hover { background: #7a96b0; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

QScrollBar:horizontal {
    background: #f0f4f8;
    height: 10px;
    border-radius: 5px;
}

QScrollBar::handle:horizontal {
    background: #aab8c8;
    border-radius: 5px;
    min-width: 36px;
}

/* ---- Tooltip ---- */
QToolTip {
    background-color: #1a3a5c;
    color: #f0f4f8;
    border: 1px solid #3b82c4;
    padding: 7px 10px;
    border-radius: 7px;
    font-size: 13px;
}

    )");
}

//  PAGINA: LOGIN
QWidget* MainWindow::crearPaginaLogin()
{
    QWidget *w = new QWidget;
    w->setObjectName("paginaLoginRoot");

    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setAlignment(Qt::AlignCenter);

    QFrame *card = new QFrame;
    card->setObjectName("cardLogin");
    card->setFixedWidth(440);
    agregarSombra(card, 50, 0.40);

    QVBoxLayout *cardLay = new QVBoxLayout(card);
    cardLay->setContentsMargins(36, 36, 36, 36);
    cardLay->setSpacing(16);

    // Titulo
    QLabel *titulo = new QLabel("Enlace Escuela-Libreria");
    titulo->setObjectName("loginTitulo");
    titulo->setAlignment(Qt::AlignCenter);
    cardLay->addWidget(titulo);

    QLabel *sub = new QLabel("Sistema de Gestion");
    sub->setObjectName("loginSub");
    sub->setAlignment(Qt::AlignCenter);
    cardLay->addWidget(sub);

    cardLay->addSpacing(10);

    // Seleccion de rol
    QLabel *lblRol = new QLabel("Selecciona tu rol:");
    lblRol->setObjectName("loginEtiqueta");
    cardLay->addWidget(lblRol);

    QHBoxLayout *rowRoles = new QHBoxLayout;
    rowRoles->setSpacing(10);

    grupoRoles = new QButtonGroup(this);

    struct DatosRol { QString texto; Rol rol; };
    QList<DatosRol> roles = {
                             {"Proveedor",   Rol::Proveedor},
                             {"Escuela",     Rol::Escuela},
                             {"Estudiante",  Rol::Estudiante},
                             };

    int idRol = 0;
    for (const auto &d : roles)
    {
        QPushButton *btn = new QPushButton(d.texto);
        btn->setObjectName("btnRol");
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        grupoRoles->addButton(btn, idRol++);
        rowRoles->addWidget(btn);
    }
    grupoRoles->button(0)->setChecked(true);

    connect(grupoRoles, QOverload<int>::of(&QButtonGroup::idClicked),
            this, &MainWindow::seleccionarRolLogin);

    cardLay->addLayout(rowRoles);

    // Password
    QLabel *lblPass = new QLabel("Contrasena:");
    lblPass->setObjectName("loginEtiqueta");
    cardLay->addWidget(lblPass);

    inpPassword = new QLineEdit;
    inpPassword->setObjectName("inpPasswordLogin");
    inpPassword->setEchoMode(QLineEdit::Password);
    inpPassword->setPlaceholderText("Ingresa tu contrasena");
    cardLay->addWidget(inpPassword);

    cardLay->addSpacing(6);

    // Boton ingresar
    QPushButton *btnLogin = new QPushButton("Ingresar");
    btnLogin->setObjectName("btnLoginAccion");
    btnLogin->setCursor(Qt::PointingHandCursor);
    connect(btnLogin, &QPushButton::clicked, this, &MainWindow::accionLogin);
    cardLay->addWidget(btnLogin);

    lay->addWidget(card, 0, Qt::AlignCenter);

    return w;
}

//  PAGINA: MENU
QWidget* MainWindow::crearPaginaMenu()
{
    QWidget *w = new QWidget;
    w->setObjectName("paginaMenuRoot");

    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setAlignment(Qt::AlignCenter);
    lay->setSpacing(10);
    lay->setContentsMargins(40, 30, 40, 30);

    QLabel *lbl = new QLabel("Menu Principal");
    lbl->setObjectName("tituloPagina");
    lbl->setAlignment(Qt::AlignCenter);
    lay->addWidget(lbl);

    QLabel *sub = new QLabel("Selecciona una opcion");
    sub->setObjectName("subtituloMenu");
    sub->setAlignment(Qt::AlignCenter);
    lay->addWidget(sub);

    lay->addSpacing(10);

    // Los botones se reconstruyen en reconstruirMenuSegunRol()
    // Solo dejamos un marcador aqui; la logica real esta en reconstruirMenuSegunRol

    return w;
}

//  RECONSTRUIR MENU SEGUN ROL
void MainWindow::reconstruirMenuSegunRol()
{
    // Eliminar botones viejos (despues del titulo y subtitulo)
    QVBoxLayout *lay = qobject_cast<QVBoxLayout*>(paginaMenu->layout());
    while (lay->count() > 4)  // titulo + sub + spacing + spacing = 4
    {
        QLayoutItem *item = lay->takeAt(4);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    struct OpMenu { QString texto; void (MainWindow::*slot)(); bool separador; };

    QVector<OpMenu> opciones;

    if (rolActual == Rol::Proveedor)
    {
        opciones = {
                    {"Libros Disponibles",          &MainWindow::mostrarLibrosDisponibles, false},
                    {"Registrar Libro",             &MainWindow::mostrarRegistrarLibro,    false},
                    {"Registrar Escuela",           &MainWindow::mostrarRegistrarEscuela,  false},
                    {"Registrar Vale",              &MainWindow::mostrarRegistrarVale,     false},
                    {"Buscar Escuela por ID",       &MainWindow::mostrarBuscarEscuela,     false},
                    {"Mostrar Todas las Escuelas",  &MainWindow::mostrarTodasEscuelas,     false},
                    {"Mostrar Vales Pendientes",    &MainWindow::mostrarValesPendientes,   false},
                    {"Generar Reporte Semanal",     &MainWindow::mostrarReporteSemanal,    false},
                    {"Mostrar Resumen General",     &MainWindow::mostrarResumenGeneral,    false},
                    };
    }
    else if (rolActual == Rol::Escuela)
    {
        opciones = {
                    {"Libros Disponibles",       &MainWindow::mostrarLibrosDisponibles, false},
                    {"Pedir Libros al Proveedor",&MainWindow::mostrarPedirLibros,       false},
                    {"Mostrar Vales Pendientes", &MainWindow::mostrarValesPendientes,   false},
                    };
    }
    else if (rolActual == Rol::Estudiante)
    {
        opciones = {
                    {"Libros Disponibles",   &MainWindow::mostrarLibrosDisponibles, false},
                    {"Pedir Libros a Escuela",&MainWindow::mostrarPedirLibros,      false},
                    };
    }

    for (const auto &op : opciones)
    {
        QPushButton *btn = crearBotonMenu(op.texto);
        connect(btn, &QPushButton::clicked, this, op.slot);
        lay->addWidget(btn, 0, Qt::AlignCenter);
    }

    lay->addSpacing(16);
    QPushButton *btnSalir = crearBotonMenu("Guardar y Salir");
    btnSalir->setObjectName("btnSalir");
    connect(btnSalir, &QPushButton::clicked, this, &MainWindow::accionGuardarYSalir);
    lay->addWidget(btnSalir, 0, Qt::AlignCenter);
}

//  PAGINA: LIBROS DISPONIBLES (Galeria)
QWidget* MainWindow::crearPaginaLibros()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(30, 20, 30, 20);
    lay->setSpacing(12);

    QLabel *lbl = crearTitulo("Libros Disponibles");
    lay->addWidget(lbl);

    // Area de scroll con grid de tarjetas
    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    contenedorGrid = new QWidget;
    gridLibros = new QGridLayout(contenedorGrid);
    gridLibros->setSpacing(18);
    gridLibros->setContentsMargins(8, 8, 8, 8);

    scroll->setWidget(contenedorGrid);
    lay->addWidget(scroll, 1);

    QPushButton *btnVolver = new QPushButton("Volver al Menu");
    btnVolver->setObjectName("btnVolver");
    connect(btnVolver, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    lay->addWidget(btnVolver, 0, Qt::AlignLeft);

    return w;
}

//  Poblar galeria con datos del sistema
void MainWindow::poblarGaleria()
{
    // Limpiar tarjetas anteriores
    while (QLayoutItem *item = gridLibros->takeAt(0))
    {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    auto disponibles = sistema.librosDisponibles();
    if (disponibles.empty())
    {
        QLabel *vacio = new QLabel("No hay libros disponibles.");
        vacio->setAlignment(Qt::AlignCenter);
        gridLibros->addWidget(vacio, 0, 0);
        return;
    }

    int col = 0, fila = 0;
    const int COLS = 4;

    for (const auto &lib : disponibles)
    {
        // Adaptamos el acceso a los datos segun tu clase Libro
        // Ajusta los getters reales de tu clase Libro si son distintos
        QString titulo    = QString::fromStdString(lib.titulo);
        QString autor     = QString::fromStdString(lib.autor);
        QString categoria = QString::fromStdString(lib.categoria);
        QString imgUrl    = QString::fromStdString(lib.imagenUrl);

        TarjetaLibro *tarjeta = new TarjetaLibro(titulo, autor, categoria, imgUrl);
        gridLibros->addWidget(tarjeta, fila, col);

        col++;
        if (col >= COLS) { col = 0; fila++; }
    }
}

//  PAGINA: REGISTRAR LIBRO
QWidget* MainWindow::crearPaginaRegLibro()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(50, 20, 50, 20);

    lay->addWidget(crearTitulo("Registrar Libro"));

    QGroupBox *grupo = new QGroupBox("Datos del Libro");
    QFormLayout *form = new QFormLayout(grupo);
    form->setSpacing(12);

    inpTitulo    = new QLineEdit; form->addRow("Titulo:",    inpTitulo);
    inpAutor     = new QLineEdit; form->addRow("Autor:",     inpAutor);
    inpEditorial = new QLineEdit; form->addRow("Editorial:", inpEditorial);
    inpCategoria = new QLineEdit; form->addRow("Categoria:", inpCategoria);
    inpMateria   = new QLineEdit; form->addRow("Materia:",   inpMateria);
    inpImagenUrl = new QLineEdit; form->addRow("URL Imagen:",inpImagenUrl);

    inpISBN = new QSpinBox;
    inpISBN->setRange(0, 999999999);
    form->addRow("ISBN:", inpISBN);

    inpPrecio = new QDoubleSpinBox;
    inpPrecio->setRange(0, 99999.99);
    inpPrecio->setPrefix("$ ");
    form->addRow("Precio:", inpPrecio);

    inpCantidadLibro = new QSpinBox;
    inpCantidadLibro->setRange(1, 10000);
    form->addRow("Cantidad:", inpCantidadLibro);

    lay->addWidget(grupo);
    lay->addStretch();

    QHBoxLayout *bots = new QHBoxLayout;
    QPushButton *bv = new QPushButton("Volver");
    bv->setObjectName("btnVolver");
    QPushButton *bg = new QPushButton("Registrar");
    bg->setObjectName("btnAccion");
    connect(bv, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    connect(bg, &QPushButton::clicked, this, &MainWindow::accionRegistrarLibro);
    bots->addWidget(bv); bots->addStretch(); bots->addWidget(bg);
    lay->addLayout(bots);

    return w;
}

//  PAGINA: REGISTRAR ESCUELA
QWidget* MainWindow::crearPaginaRegEscuela()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(50, 20, 50, 20);

    lay->addWidget(crearTitulo("Registrar Escuela"));

    QGroupBox *grupo = new QGroupBox("Datos de la Escuela");
    QFormLayout *form = new QFormLayout(grupo);
    form->setSpacing(12);

    inpNombreEsc  = new QLineEdit; form->addRow("Nombre:",    inpNombreEsc);
    inpUbicacion  = new QLineEdit; form->addRow("Ubicacion:", inpUbicacion);
    inpTelefono   = new QSpinBox;  inpTelefono->setRange(0, 999999999);
    form->addRow("Telefono:", inpTelefono);
    inpCorreoEsc  = new QLineEdit; form->addRow("Correo:",    inpCorreoEsc);
    inpIdEscuela  = new QSpinBox;  inpIdEscuela->setRange(1, 99999);
    form->addRow("ID Escuela:", inpIdEscuela);

    lay->addWidget(grupo);
    lay->addStretch();

    QHBoxLayout *bots = new QHBoxLayout;
    QPushButton *bv = new QPushButton("Volver");
    bv->setObjectName("btnVolver");
    QPushButton *bg = new QPushButton("Registrar");
    bg->setObjectName("btnAccion");
    connect(bv, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    connect(bg, &QPushButton::clicked, this, &MainWindow::accionRegistrarEscuela);
    bots->addWidget(bv); bots->addStretch(); bots->addWidget(bg);
    lay->addLayout(bots);

    return w;
}

//  PAGINA: REGISTRAR VALE
QWidget* MainWindow::crearPaginaRegVale()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(50, 20, 50, 20);

    lay->addWidget(crearTitulo("Registrar Vale"));

    QGroupBox *grpVale = new QGroupBox("Datos del Vale");
    QFormLayout *fv = new QFormLayout(grpVale);
    fv->setSpacing(10);

    inpIdEscuelaVale = new QSpinBox; inpIdEscuelaVale->setRange(1, 99999);
    fv->addRow("ID Escuela:", inpIdEscuelaVale);
    inpCorreoVale = new QLineEdit;
    fv->addRow("Correo:", inpCorreoVale);
    inpFechaVale = new QLineEdit;
    inpFechaVale->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    fv->addRow("Fecha:", inpFechaVale);
    lay->addWidget(grpVale);

    QGroupBox *grpLibros = new QGroupBox("Agregar Libros al Vale");
    QVBoxLayout *lb = new QVBoxLayout(grpLibros);

    QHBoxLayout *row = new QHBoxLayout;
    inpISBNVale = new QSpinBox; inpISBNVale->setRange(0, 999999999);
    inpCantVale = new QSpinBox; inpCantVale->setRange(1, 1000);
    QPushButton *btnAg = new QPushButton("Agregar");
    btnAg->setObjectName("btnAccion");
    row->addWidget(new QLabel("ISBN:")); row->addWidget(inpISBNVale);
    row->addWidget(new QLabel("Cantidad:")); row->addWidget(inpCantVale);
    row->addWidget(btnAg);
    lb->addLayout(row);

    tablaLibrosVale = new QTableWidget(0, 2);
    tablaLibrosVale->setHorizontalHeaderLabels({"ISBN", "Cantidad"});
    tablaLibrosVale->horizontalHeader()->setStretchLastSection(true);
    tablaLibrosVale->setMaximumHeight(150);
    lb->addWidget(tablaLibrosVale);
    lay->addWidget(grpLibros);

    connect(btnAg, &QPushButton::clicked, this, &MainWindow::accionAgregarLibroAVale);

    QHBoxLayout *bots = new QHBoxLayout;
    QPushButton *bv = new QPushButton("Volver");
    bv->setObjectName("btnVolver");
    QPushButton *bg = new QPushButton("Registrar Vale");
    bg->setObjectName("btnAccion");
    connect(bv, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    connect(bg, &QPushButton::clicked, this, &MainWindow::accionRegistrarVale);
    bots->addWidget(bv); bots->addStretch(); bots->addWidget(bg);
    lay->addLayout(bots);

    return w;
}

//  PAGINA: BUSCAR ESCUELA
QWidget* MainWindow::crearPaginaBuscar()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(50, 20, 50, 20);

    lay->addWidget(crearTitulo("Buscar Escuela por ID"));

    QHBoxLayout *row = new QHBoxLayout;
    inpIdBuscar = new QSpinBox; inpIdBuscar->setRange(1, 99999);
    QPushButton *btnB = new QPushButton("Buscar");
    btnB->setObjectName("btnAccion");
    row->addWidget(new QLabel("ID Escuela:"));
    row->addWidget(inpIdBuscar);
    row->addWidget(btnB);
    row->addStretch();
    lay->addLayout(row);

    salidaBuscar = new QTextEdit;
    salidaBuscar->setReadOnly(true);
    lay->addWidget(salidaBuscar);

    connect(btnB, &QPushButton::clicked, this, &MainWindow::accionBuscarEscuela);

    QPushButton *bv = new QPushButton("Volver al Menu");
    bv->setObjectName("btnVolver");
    connect(bv, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    lay->addWidget(bv, 0, Qt::AlignLeft);

    return w;
}

//  PAGINAS: SALIDAS DE TEXTO (helpers comunes)
static QWidget* crearPaginaSalida(const QString &titulo,
                                  QTextEdit *&salida,
                                  QPushButton *&btnVolver)
{
    QWidget *w = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(50, 20, 50, 20);

    QLabel *lbl = new QLabel(titulo);
    lbl->setObjectName("tituloPagina");
    lay->addWidget(lbl);

    salida = new QTextEdit;
    salida->setReadOnly(true);
    lay->addWidget(salida);

    btnVolver = new QPushButton("Volver al Menu");
    btnVolver->setObjectName("btnVolver");
    lay->addWidget(btnVolver, 0, Qt::AlignLeft);

    return w;
}

QWidget* MainWindow::crearPaginaEscuelas()
{
    QPushButton *bv;
    QWidget *w = crearPaginaSalida("Todas las Escuelas", salidaEscuelas, bv);
    connect(bv, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    return w;
}

QWidget* MainWindow::crearPaginaVales()
{
    QPushButton *bv;
    QWidget *w = crearPaginaSalida("Vales Pendientes", salidaVales, bv);
    connect(bv, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    return w;
}

QWidget* MainWindow::crearPaginaReporte()
{
    QPushButton *bv;
    QWidget *w = crearPaginaSalida("Reporte Semanal", salidaReporte, bv);
    connect(bv, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    return w;
}

QWidget* MainWindow::crearPaginaResumen()
{
    QPushButton *bv;
    QWidget *w = crearPaginaSalida("Resumen General", salidaResumen, bv);
    connect(bv, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    return w;
}

//  PAGINA: PEDIR LIBROS
QWidget* MainWindow::crearPaginaPedir()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setContentsMargins(50, 20, 50, 20);

    lay->addWidget(crearTitulo("Pedir Libros"));

    QGroupBox *grp = new QGroupBox("Solicitud de Libros");
    QFormLayout *form = new QFormLayout(grp);
    form->setSpacing(12);

    inpISBNPedir = new QSpinBox; inpISBNPedir->setRange(0, 999999999);
    form->addRow("ISBN del Libro:", inpISBNPedir);
    inpCantPedir = new QSpinBox; inpCantPedir->setRange(1, 1000);
    form->addRow("Cantidad:", inpCantPedir);
    lay->addWidget(grp);

    salidaPedir = new QTextEdit;
    salidaPedir->setReadOnly(true);
    salidaPedir->setMaximumHeight(120);
    lay->addWidget(salidaPedir);

    lay->addStretch();

    QHBoxLayout *bots = new QHBoxLayout;
    QPushButton *bv = new QPushButton("Volver");
    bv->setObjectName("btnVolver");
    QPushButton *bp = new QPushButton("Enviar Pedido");
    bp->setObjectName("btnAccion");
    connect(bv, &QPushButton::clicked, this, &MainWindow::mostrarMenu);
    connect(bp, &QPushButton::clicked, this, &MainWindow::accionPedirLibros);
    bots->addWidget(bv); bots->addStretch(); bots->addWidget(bp);
    lay->addLayout(bots);

    return w;
}

//  NAVEGACION
void MainWindow::mostrarMenu()
{
    stackedWidget->setCurrentIndex(IDX_MENU);
}

void MainWindow::mostrarLibrosDisponibles()
{
    poblarGaleria();
    stackedWidget->setCurrentIndex(IDX_LIBROS);
}

void MainWindow::mostrarRegistrarLibro()
{
    stackedWidget->setCurrentIndex(IDX_REG_LIBRO);
}

void MainWindow::mostrarRegistrarEscuela()
{
    stackedWidget->setCurrentIndex(IDX_REG_ESCUELA);
}

void MainWindow::mostrarRegistrarVale()
{
    librosVale.clear();
    tablaLibrosVale->setRowCount(0);
    stackedWidget->setCurrentIndex(IDX_REG_VALE);
}

void MainWindow::mostrarBuscarEscuela()
{
    salidaBuscar->clear();
    stackedWidget->setCurrentIndex(IDX_BUSCAR);
}

void MainWindow::mostrarTodasEscuelas()
{
    salidaEscuelas->clear();
    sistema.mostrarEscuelas();
    salidaEscuelas->setText("(Datos impresos en consola)");
    stackedWidget->setCurrentIndex(IDX_ESCUELAS);
}

void MainWindow::mostrarValesPendientes()
{
    salidaVales->clear();
    sistema.mostrarValesPendientes();
    salidaVales->setText("(Datos impresos en consola)");
    stackedWidget->setCurrentIndex(IDX_VALES);
}

void MainWindow::mostrarReporteSemanal()
{
    salidaReporte->clear();
    sistema.generarReporteSemanal();
    salidaReporte->setText("(Datos impresos en consola)");
    stackedWidget->setCurrentIndex(IDX_REPORTE);
}

void MainWindow::mostrarResumenGeneral()
{
    salidaResumen->clear();
    sistema.mostrarResumen();
    salidaResumen->setText("(Datos impresos en consola)");
    stackedWidget->setCurrentIndex(IDX_RESUMEN);
}

void MainWindow::mostrarPedirLibros()
{
    salidaPedir->clear();
    stackedWidget->setCurrentIndex(IDX_PEDIR);
}

//  ACCIONES: LOGIN
void MainWindow::seleccionarRolLogin(int id)
{
    switch (id)
    {
    case 0: rolLogin = Rol::Proveedor;  break;
    case 1: rolLogin = Rol::Escuela;    break;
    case 2: rolLogin = Rol::Estudiante; break;
    default: break;
    }
}

void MainWindow::accionLogin()
{
    // Contrasenas de ejemplo — adapta a tu base de datos PostgreSQL
    // sistema.autenticar(rolLogin, inpPassword->text().toStdString())
    QString pass = inpPassword->text();

    if (pass.isEmpty())
    {
        QMessageBox::warning(this, "Campo vacio", "Ingresa una contrasena.");
        return;
    }

    // --- Aqui va tu validacion contra PostgreSQL ---
    // bool ok = sistema.autenticar(rolLogin, pass.toStdString());
    // Por ahora aceptamos cualquier contrasena no vacia:
    bool ok = true;

    if (!ok)
    {
        QMessageBox::critical(this, "Acceso denegado", "Contrasena incorrecta.");
        return;
    }

    rolActual = rolLogin;
    inpPassword->clear();

    reconstruirMenuSegunRol();
    stackedWidget->setCurrentIndex(IDX_MENU);
}

//  ACCIONES: CRUD
void MainWindow::accionRegistrarLibro()
{
    if (inpTitulo->text().trimmed().isEmpty() || inpAutor->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Campos vacios", "Titulo y Autor son obligatorios.");
        return;
    }

    sistema.registrarLibro(
        inpTitulo->text().trimmed().toStdString(),
        inpAutor->text().trimmed().toStdString(),
        inpISBN->value(),
        inpEditorial->text().trimmed().toStdString(),
        inpCategoria->text().trimmed().toStdString(),
        inpMateria->text().trimmed().toStdString(),
        (float)inpPrecio->value(),
        inpCantidadLibro->value()
        // Si tu sistema acepta URL de imagen, agregala aqui:
        // inpImagenUrl->text().trimmed().toStdString()
        );

    QMessageBox::information(this, "Exito", "Libro registrado correctamente.");
    inpTitulo->clear(); inpAutor->clear(); inpEditorial->clear();
    inpCategoria->clear(); inpMateria->clear(); inpImagenUrl->clear();
    inpISBN->setValue(0); inpPrecio->setValue(0); inpCantidadLibro->setValue(1);
    mostrarMenu();
}

void MainWindow::accionRegistrarEscuela()
{
    if (inpNombreEsc->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Campo vacio", "El nombre es obligatorio.");
        return;
    }

    sistema.registrarEscuela(
        inpNombreEsc->text().trimmed().toStdString(),
        inpUbicacion->text().trimmed().toStdString(),
        inpTelefono->value(),
        inpCorreoEsc->text().trimmed().toStdString(),
        inpIdEscuela->value()
        );

    QMessageBox::information(this, "Exito", "Escuela registrada correctamente.");
    inpNombreEsc->clear(); inpUbicacion->clear(); inpCorreoEsc->clear();
    inpTelefono->setValue(0); inpIdEscuela->setValue(1);
    mostrarMenu();
}

void MainWindow::accionAgregarLibroAVale()
{
    int isbn = inpISBNVale->value();
    int cant = inpCantVale->value();
    librosVale.append({isbn, cant});

    int fila = tablaLibrosVale->rowCount();
    tablaLibrosVale->insertRow(fila);
    tablaLibrosVale->setItem(fila, 0, new QTableWidgetItem(QString::number(isbn)));
    tablaLibrosVale->setItem(fila, 1, new QTableWidgetItem(QString::number(cant)));
}

void MainWindow::accionRegistrarVale()
{
    if (librosVale.isEmpty())
    {
        QMessageBox::warning(this, "Sin libros", "Agrega al menos un libro al vale.");
        return;
    }

    std::vector<std::pair<int,int>> libros;
    for (const auto &p : librosVale)
        libros.emplace_back(p.first, p.second);

    int idVale = sistema.generarIdVale();
    sistema.registrarVale(
        idVale,
        inpIdEscuelaVale->value(),
        libros,
        inpFechaVale->text().toStdString(),
        inpCorreoVale->text().toStdString()
        );

    QMessageBox::information(this, "Exito",
                             QString("Vale #%1 registrado correctamente.").arg(idVale));
    librosVale.clear();
    tablaLibrosVale->setRowCount(0);
    mostrarMenu();
}

void MainWindow::accionBuscarEscuela()
{
    int id = inpIdBuscar->value();
    Escuela *e = sistema.buscarEscuela(id);
    if (e)
    {
        salidaBuscar->setText(
            "Escuela encontrada:\n"
            "ID: " + QString::number(id) + "\n"
                                    "(Datos completos en consola)"
            );
        e->mostrarDatos();
    }
    else
    {
        salidaBuscar->setText(
            "No se encontro ninguna escuela con ID: " + QString::number(id));
    }
}

void MainWindow::accionPedirLibros()
{
    int isbn = inpISBNPedir->value();
    int cant = inpCantPedir->value();

    // Aqui va la logica real de pedido segun el rol
    // Por ejemplo: sistema.pedirLibro(isbn, cant, rolActual);
    salidaPedir->setText(
        QString("Pedido enviado:\n  ISBN: %1\n  Cantidad: %2\n\n"
                "En espera de confirmacion.")
            .arg(isbn).arg(cant)
        );
}

void MainWindow::accionGuardarYSalir()
{
    sistema.guardarInventario("data/inventario.csv");
    QMessageBox::information(this, "Guardado", "Inventario guardado en data/inventario.csv");
    QApplication::quit();
}
