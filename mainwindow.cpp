#include <sstream>
#include <iomanip>
/*
    MAIN WINDOW
*/

#include "converter.h"
#include "file_manager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gl_diagram.h"

//Version
QString stargraph_version = "1.0.1a";

//Assigns the values to the extern variables
QTableWidget* entry_list = nullptr;

static QIntValidator* temp_validator;
static QDoubleValidator* lum_validator;
static QDoubleValidator* mag_validator;
static QString error_message = "";



//Starts the user interface
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Show version information
    this->setWindowTitle("StarGraph v" + stargraph_version);
    ui->label_credit->setText("StarGraph v" + stargraph_version + "(2018) by Filippo Dusi");

    //Sets the maximum string length for each lineEdit widget
    ui->lineEdit_input_name->setMaxLength(16);
    ui->lineEdit_input_temperature->setMaxLength(6);
    ui->lineEdit_input_luminosity->setMaxLength(8);

    //Initializes the input validators
    temp_validator = new QIntValidator(3000, 200000, this);
    lum_validator = new QDoubleValidator(0.000001, 1000000, 6, this);
    lum_validator->setNotation(QDoubleValidator::StandardNotation);
    mag_validator = new QDoubleValidator(-100, 100, 6, this);
    mag_validator->setNotation(QDoubleValidator::StandardNotation);

    ui->lineEdit_input_temperature->setValidator(temp_validator);
    ui->lineEdit_input_luminosity->setValidator(lum_validator);

    //Initializes the entry list
    entry_list = ui->table_entries;

    //Updates the OpenGL widget
    ui->openGLWidget_diagram->update();
}

//MainWindow deconstructor
MainWindow::~MainWindow()
{
    delete ui;
}

//Switches between teperature and spectral classs input modes
void MainWindow::on_checkBox_input_spectral_type_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->lineEdit_input_temperature->setMaxLength(2);
        ui->lineEdit_input_temperature->setValidator(nullptr);
        ui->label_input_temperature->setText("Star spectral class:");
        ui->label_kelvin->setText("");
    }
    else
    {
        ui->lineEdit_input_temperature->setMaxLength(6);
        ui->lineEdit_input_temperature->setValidator(temp_validator);
        ui->label_input_temperature->setText("Star temperature:");
        ui->label_kelvin->setText("K");
    }

    //Clears the lineEdit widget
    ui->lineEdit_input_temperature->clear();
}

//Switches between luminosity and absolute magnitude input modes
void MainWindow::on_checkBox_input_absolute_magnitude_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->lineEdit_input_luminosity->setMaxLength(5);
        ui->lineEdit_input_luminosity->setValidator(mag_validator);
        ui->label_input_luminosity->setText("Star absolute magnitude:");
    }
    else
    {
        ui->lineEdit_input_luminosity->setMaxLength(8);
        ui->lineEdit_input_luminosity->setValidator(lum_validator);
        ui->label_input_luminosity->setText("Star relative luminosity:");
    }

    //Clears the lineEdit widget
    ui->lineEdit_input_luminosity->clear();
}

//Adds a new entry to the 'entry_list' table
void MainWindow::on_pushButton_add_entry_clicked()
{
    QString name_value = ui->lineEdit_input_name->text();
    QString temperature_value;
    QString luminosity_value;
    QString spectral_class_value;
    QString absolute_magnitude_value;

    //Calculates the two parameters which the user didn't type in
    if(!ui->checkBox_input_spectral_type->checkState())
    {
        temperature_value = ui->lineEdit_input_temperature->text();

        //Calculates the spectral class
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

        spectral_class_value = str_spectral_class;
    }
    else
    {
        spectral_class_value = ui->lineEdit_input_temperature->text()[0].toUpper() + ui->lineEdit_input_temperature->text()[1];

        //Calculates the temperature
        QString num_spectral_class = spectral_class_value;
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

        temperature_value = QString::number(StarFunctions::get_temperature(num_spectral_class.toInt()));
    }
    if(!ui->checkBox_input_absolute_magnitude->checkState())
    {
        luminosity_value = ui->lineEdit_input_luminosity->text();

        //Calculates the absolute magnitude
        std::stringstream tempss;
        tempss << std::fixed;
        tempss << std::setprecision(10);
        tempss << StarFunctions::get_absolute_magnitude(luminosity_value.toDouble());
        absolute_magnitude_value = QString::fromStdString(tempss.str());
    }
    else
    {
        absolute_magnitude_value = ui->lineEdit_input_luminosity->text();

        //Calculates the relative luminosity
        std::stringstream tempss;
        tempss << std::fixed;
        tempss << std::setprecision(10);
        tempss << StarFunctions::get_relative_luminosity(absolute_magnitude_value.toDouble());
        luminosity_value = QString::fromStdString(tempss.str());

    }
    if(ui->lineEdit_input_name->text() == "" || ui->lineEdit_input_temperature->text() == "" || ui->lineEdit_input_luminosity->text() == "")
    {
        //Sets the error message
        error_message = "Error: all the fields must be filled";
    }
    else if(
            !(
            (spectral_class_value[0] == 'O' ||
            spectral_class_value[0] == 'B' ||
            spectral_class_value[0] == 'A' ||
            spectral_class_value[0] == 'F' ||
            spectral_class_value[0] == 'G' ||
            spectral_class_value[0] == 'K' ||
            spectral_class_value[0] == 'M'
            ) &&
            (isdigit(spectral_class_value.toStdString()[1]))
            )
        )
    {
        //Sets the error message
        error_message = "Error: invalid spectral type";
    }
    else
    {
        //Draws the values to the table
        ui->table_entries->setItem(ui->table_entries->rowCount() - 1, 0, new QTableWidgetItem(name_value));
        ui->table_entries->setItem(ui->table_entries->rowCount() - 1, 1, new QTableWidgetItem(temperature_value));
        ui->table_entries->setItem(ui->table_entries->rowCount() - 1, 2, new QTableWidgetItem(luminosity_value));
        ui->table_entries->setItem(ui->table_entries->rowCount() - 1, 3, new QTableWidgetItem(spectral_class_value));
        ui->table_entries->setItem(ui->table_entries->rowCount() - 1, 4, new QTableWidgetItem(absolute_magnitude_value));

        //Clears the lineEdit widgets
        ui->lineEdit_input_name->clear();
        ui->lineEdit_input_temperature->clear();
        ui->lineEdit_input_luminosity->clear();

        //Copies the table values into the 'entry_list' table
        entry_list = ui->table_entries;

        //Resets the error message
        error_message = "";

        //Updates the OpenGL widget
        ui->openGLWidget_diagram->update();
    }
    //If there was an error, the error message is displayed
    ui->label_input_error->setText("<html><head/><body><p><span style=\" color:#ff0000;\">" + error_message + "</span></p></body></html>");
}

//Changes the minimum temperature value
void MainWindow::on_spinBox_min_temperature_valueChanged(int arg1)
{
    temp_min = arg1;
    ui->openGLWidget_diagram->update();
}

//Changes the maximum temperature value
void MainWindow::on_spinBox_max_temperature_valueChanged(int arg1)
{
    temp_max = arg1;
    ui->openGLWidget_diagram->update();
}

//Changes the minimum luminosity value
void MainWindow::on_spinBox_min_luminosity_valueChanged(int arg1)
{
    lum_min = arg1;
    ui->openGLWidget_diagram->update();
}

//Changes the maximum luminosity value
void MainWindow::on_spinBox_max_luminosity_valueChanged(int arg1)
{
    lum_max = arg1;
    ui->openGLWidget_diagram->update();
}

//Changes the state of the 'show_names' bool
void MainWindow::on_checkBox_show_names_stateChanged(int arg1)
{
    graph_show_names = arg1;
    ui->openGLWidget_diagram->update();
}

//Changes the opacity of the reference lines
void MainWindow::on_spinBox_line_opacity_valueChanged(int arg1)
{
    graph_lines_opacity = arg1;
    ui->openGLWidget_diagram->update();
}

//Toggles the vertical reference lines
void MainWindow::on_actionVertical_toggled(bool arg1)
{
    graph_show_v_lines = arg1;
    ui->openGLWidget_diagram->update();
}

//Toggles the horizontal reference lines
void MainWindow::on_actionHorizontal_toggled(bool arg1)
{
    graph_show_h_lines = arg1;
    ui->openGLWidget_diagram->update();
}

//Saves the diagram as an image
void MainWindow::on_actionExport_as_image_triggered()
{
    FileIOFunctions::save_image(ui->openGLWidget_diagram);
}

//Saves the list as a '.sgl' file
void MainWindow::on_actionSave_list_triggered()
{
    FileIOFunctions::save_list_as_new(entry_list);
}

//Clears the current list and starts a new one
void MainWindow::on_actionNew_list_triggered()
{
    entry_list->clearContents();
    entry_list->setRowCount(0);
}

//Opens a '.sgl' file into a new list
void MainWindow::on_actionOpen_list_triggered()
{
    FileIOFunctions::open_list(entry_list);
}

//Opens a '.csv' file into a new list
void MainWindow::on_actionImport_list_triggered()
{
    FileIOFunctions::import_csv(entry_list);
}
