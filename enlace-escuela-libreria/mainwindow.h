#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QMessageBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QVector>
#include <QPair>
#include <QFrame>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QButtonGroup>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>

#include "SistemaGestion.h"
#include "Escuela.h"

//  Rol de usuario
enum class Rol { Ninguno, Proveedor, Escuela, Estudiante };

//  TarjetaLibro — widget visual para la galeria
class TarjetaLibro : public QFrame
{
    Q_OBJECT
public:
    explicit TarjetaLibro(const QString &titulo,
                          const QString &autor,
                          const QString &categoria,
                          const QString &imagenUrl,
                          QWidget *parent = nullptr);

private slots:
    void onImagenDescargada(QNetworkReply *reply);

private:
    QLabel *lblPortada;
    QLabel *lblTitulo;
    QLabel *lblAutor;
    QLabel *lblCategoria;
    QNetworkAccessManager *nam;

    void cargarImagen(const QString &url);
};

//  MainWindow
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Login
    void accionLogin();
    void seleccionarRolLogin(int id);

    // Navegacion
    void mostrarMenu();
    void mostrarLibrosDisponibles();
    void mostrarRegistrarLibro();
    void mostrarRegistrarEscuela();
    void mostrarRegistrarVale();
    void mostrarBuscarEscuela();
    void mostrarTodasEscuelas();
    void mostrarValesPendientes();
    void mostrarReporteSemanal();
    void mostrarResumenGeneral();
    void mostrarPedirLibros();

    // Acciones
    void accionRegistrarLibro();
    void accionRegistrarEscuela();
    void accionRegistrarVale();
    void accionBuscarEscuela();
    void accionAgregarLibroAVale();
    void accionGuardarYSalir();
    void accionPedirLibros();

protected:
    QStackedWidget *stackedWidget;
    SistemaGestion  sistema;
    Rol             rolActual = Rol::Ninguno;

    // --- Indices de paginas ---
    enum Idx {
        IDX_LOGIN = 0,
        IDX_MENU,
        IDX_LIBROS,
        IDX_REG_LIBRO,
        IDX_REG_ESCUELA,
        IDX_REG_VALE,
        IDX_BUSCAR,
        IDX_ESCUELAS,
        IDX_VALES,
        IDX_REPORTE,
        IDX_RESUMEN,
        IDX_PEDIR,
        IDX_COUNT
    };

    // --- Paginas ---
    QWidget *paginaLogin;
    QWidget *paginaMenu;
    QWidget *paginaLibros;
    QWidget *paginaRegLibro;
    QWidget *paginaRegEscuela;
    QWidget *paginaRegVale;
    QWidget *paginaBuscar;
    QWidget *paginaEscuelas;
    QWidget *paginaVales;
    QWidget *paginaReporte;
    QWidget *paginaResumen;
    QWidget *paginaPedir;

    // --- Login ---
    QLineEdit    *inpPassword;
    QButtonGroup *grupoRoles;
    Rol           rolLogin = Rol::Proveedor;

    // --- Galeria ---
    QGridLayout  *gridLibros;
    QWidget      *contenedorGrid;

    // --- Registro Libro ---
    QLineEdit      *inpTitulo, *inpAutor, *inpEditorial;
    QLineEdit      *inpCategoria, *inpMateria, *inpImagenUrl;
    QSpinBox       *inpISBN, *inpCantidadLibro;
    QDoubleSpinBox *inpPrecio;

    // --- Registro Escuela ---
    QLineEdit *inpNombreEsc, *inpUbicacion, *inpCorreoEsc;
    QSpinBox  *inpTelefono, *inpIdEscuela;

    // --- Registro Vale ---
    QSpinBox     *inpIdEscuelaVale, *inpISBNVale, *inpCantVale;
    QLineEdit    *inpCorreoVale, *inpFechaVale;
    QTableWidget *tablaLibrosVale;
    QVector<QPair<int,int>> librosVale;

    // --- Buscar Escuela ---
    QSpinBox  *inpIdBuscar;
    QTextEdit *salidaBuscar;

    // --- Pedir libros ---
    QSpinBox  *inpISBNPedir, *inpCantPedir;
    QTextEdit *salidaPedir;

    // --- Salidas texto ---
    QTextEdit *salidaEscuelas, *salidaVales;
    QTextEdit *salidaReporte,  *salidaResumen;

    // --- Helpers ---
    QWidget*     crearPaginaLogin();
    QWidget*     crearPaginaMenu();
    QWidget*     crearPaginaLibros();
    QWidget*     crearPaginaRegLibro();
    QWidget*     crearPaginaRegEscuela();
    QWidget*     crearPaginaRegVale();
    QWidget*     crearPaginaBuscar();
    QWidget*     crearPaginaEscuelas();
    QWidget*     crearPaginaVales();
    QWidget*     crearPaginaReporte();
    QWidget*     crearPaginaResumen();
    QWidget*     crearPaginaPedir();

    QPushButton* crearBotonMenu(const QString &texto);
    QLabel*      crearTitulo(const QString &texto);
    void         aplicarEstilos();
    void         poblarGaleria();
    void         reconstruirMenuSegunRol();
    void         agregarSombra(QWidget *w, int radio = 20, qreal op = 0.14);
};

#endif // MAINWINDOW_H
