/*
    MAIN WINDOW INITIALIZER
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QWidget>

//Version
extern QString stargraph_version;

//Declares the 'entry_list' table
extern QTableWidget* entry_list;

//Initializes the user interface
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_checkBox_input_spectral_type_stateChanged(int arg1);

    void on_checkBox_input_absolute_magnitude_stateChanged(int arg1);

    void on_pushButton_add_entry_clicked();

    void on_spinBox_min_temperature_valueChanged(int arg1);

    void on_spinBox_max_temperature_valueChanged(int arg1);

    void on_spinBox_min_luminosity_valueChanged(int arg1);

    void on_spinBox_max_luminosity_valueChanged(int arg1);

    void on_checkBox_show_names_stateChanged(int arg1);

    void on_spinBox_line_opacity_valueChanged(int arg1);

    void on_actionVertical_toggled(bool arg1);

    void on_actionHorizontal_toggled(bool arg1);

    void on_actionExport_as_image_triggered();

    void on_actionSave_list_triggered();

    void on_actionNew_list_triggered();

    void on_actionOpen_list_triggered();

    void on_actionImport_list_triggered();

    void on_doubleSpinBox_point_size_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
