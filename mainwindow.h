/*
    MAIN WINDOW INITIALIZER
*/
#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QWidget>
#include <sstream>
#include <iomanip>

#include "converter.h"

using namespace std;

//Version
extern QString stargraph_version;

//Declare the 'entry_table' variable
extern QTableWidget* entry_table;

extern vector<vector<QString>> entry_list;

extern int selected_star;

extern bool manual_input;

//Initialize the user interface
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

    void on_table_entries_cellChanged(int row, int column);

    void on_table_entries_cellPressed(int row, int column);

    void on_doubleSpinBox_highlighted_area_valueChanged(double arg1);

    void on_checkBox_highlight_selected_star_toggled(bool checked);

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;

public:
    static void update_table(QTableWidget* table)
    {
        while(table->rowCount() > 0)
        {
            table->removeRow(table->rowCount() - 1);
        }
        for(int i = 0; i < static_cast<int>(entry_list.size()); i ++)
        {
            table->insertRow(static_cast<int>(i));
            for(int j = 0; j < 5; j ++)
            {
                table->setItem(i, j, new QTableWidgetItem(entry_list[static_cast<unsigned>(i)][static_cast<unsigned>(j)]));
            }
        }

        entry_table = table;
    }

};

class ParameterCalculation
{
public:
    static QString get_spectral_class_str(QString temperature_value)
    {
        //Calculate the spectral class
        QString str_spectral_class = QString::number(StarFunctions::get_spectral_type(temperature_value.toInt()));
        switch (str_spectral_class.toStdString()[0])
        {
            case '1':
                str_spectral_class[0] = 'O';
                break;
            case '2':
                str_spectral_class[0] = 'B';
                break;
            case '3':
                str_spectral_class[0] = 'A';
                break;
            case '4':
                str_spectral_class[0] = 'F';
                break;
            case '5':
                str_spectral_class[0] = 'G';
                break;
            case '6':
                str_spectral_class[0] = 'K';
                break;
            case '7':
                str_spectral_class[0] = 'M';
                break;
        }

        return str_spectral_class;
    }

    static QString get_temperature_str(QString spectral_class_value)
    {
        //Calculate the temperature
        QString num_spectral_class = spectral_class_value.toUpper();
        switch (num_spectral_class.toStdString()[0])
        {
            case 'O':
                num_spectral_class[0] = '1';
                break;
            case 'B':
                num_spectral_class[0] = '2';
                break;
            case 'A':
                num_spectral_class[0] = '3';
                break;
            case 'F':
                num_spectral_class[0] = '4';
                break;
            case 'G':
                num_spectral_class[0] = '5';
                break;
            case 'K':
                num_spectral_class[0] = '6';
                break;
            case 'M':
                num_spectral_class[0] = '7';
                break;
        }

        return QString::number(StarFunctions::get_temperature(num_spectral_class.toInt()));
    }

    static QString get_absolute_magnitude_str(QString luminosity_value)
    {
        //Calculate the absolute magnitude
        stringstream tempss;
        tempss << fixed;
        tempss << setprecision(10);
        tempss << StarFunctions::get_absolute_magnitude(luminosity_value.toDouble());
        QString absolute_magnitude_value = QString::fromStdString(tempss.str());

        while(absolute_magnitude_value[absolute_magnitude_value.size() - 1] == '0' && absolute_magnitude_value.size() > 3)
        {
            absolute_magnitude_value.truncate(absolute_magnitude_value.size() - 1);
        }
        return absolute_magnitude_value;
    }

    static QString get_relative_luminosity_str(QString absolute_magnitude_value)
    {
        //Calculate the relative luminosity
        stringstream tempss;
        tempss << fixed;
        tempss << setprecision(10);
        tempss << StarFunctions::get_relative_luminosity(absolute_magnitude_value.toDouble());
        QString luminosity_value = QString::fromStdString(tempss.str());

        while(luminosity_value[luminosity_value.size() - 1] == '0' && luminosity_value.size() > 3)
        {
            luminosity_value.truncate(luminosity_value.size() - 1);
        }

        return luminosity_value;
    }

};

