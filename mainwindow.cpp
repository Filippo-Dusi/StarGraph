/*
    MAIN WINDOW
*/
#include "file_manager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gl_diagram.h"

using namespace std;

//Version
QString stargraph_version = "1.0.3";

//Assign the values to the extern variables
QTableWidget* entry_table = nullptr;

vector<vector<QString>> entry_list;

static QIntValidator* temp_validator;
static QDoubleValidator* lum_validator;
static QDoubleValidator* mag_validator;
static QString error_message = "";

int selected_star = -1;

bool manual_input = true;

//Set up the user interface
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Show version information
    this->setWindowTitle("StarGraph v" + stargraph_version);
    ui->label_credit->setText("StarGraph v" + stargraph_version + " © 2018 Filippo Dusi");

    //Set the maximum string length for each lineEdit widget
    ui->lineEdit_input_name->setMaxLength(16);
    ui->lineEdit_input_temperature->setMaxLength(6);
    ui->lineEdit_input_luminosity->setMaxLength(8);

    //Initialize the input validators
    temp_validator = new QIntValidator(3000, 200000, this);
    lum_validator = new QDoubleValidator(0.000001, 1000000, 6, this);
    lum_validator->setNotation(QDoubleValidator::StandardNotation);
    mag_validator = new QDoubleValidator(-100, 100, 6, this);
    mag_validator->setNotation(QDoubleValidator::StandardNotation);

    ui->lineEdit_input_temperature->setValidator(temp_validator);
    ui->lineEdit_input_luminosity->setValidator(lum_validator);

    //Initialize the entry list
    entry_table = ui->table_entries;

    //Update the OpenGL widget
    ui->openGLWidget_diagram->update();
}

//MainWindow deconstructor
MainWindow::~MainWindow()
{
    delete ui;
}

//Switch between teperature and spectral classs input modes
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

    //Clear the lineEdit widget
    ui->lineEdit_input_temperature->clear();
}

//Switch between luminosity and absolute magnitude input modes
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

    //Clear the lineEdit widget
    ui->lineEdit_input_luminosity->clear();
}

//Add a new entry
void MainWindow::on_pushButton_add_entry_clicked()
{
    manual_input = false;

    QString name_value = ui->lineEdit_input_name->text();
    QString temperature_value;
    QString luminosity_value;
    QString spectral_class_value;
    QString absolute_magnitude_value;

    //Calculate the two parameters the user didn't type in
    if(!ui->checkBox_input_spectral_type->checkState())
    {
        temperature_value = ui->lineEdit_input_temperature->text();
        spectral_class_value = ParameterCalculation::get_spectral_class_str(ui->lineEdit_input_temperature->text());
    }
    else
    {
        spectral_class_value = ui->lineEdit_input_temperature->text();
        temperature_value = ParameterCalculation::get_temperature_str(ui->lineEdit_input_temperature->text()[0].toUpper() + ui->lineEdit_input_temperature->text()[1]);
    }
    if(!ui->checkBox_input_absolute_magnitude->checkState())
    {
        luminosity_value = ui->lineEdit_input_luminosity->text();
        absolute_magnitude_value = ParameterCalculation::get_absolute_magnitude_str(ui->lineEdit_input_luminosity->text());
    }
    else
    {
        absolute_magnitude_value = ui->lineEdit_input_luminosity->text();
        luminosity_value = ParameterCalculation::get_relative_luminosity_str(ui->lineEdit_input_luminosity->text());
    }
    if(ui->lineEdit_input_name->text() == "" || ui->lineEdit_input_temperature->text() == "" || ui->lineEdit_input_luminosity->text() == "")
    {
        //Set the error message
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
        //Set the error message
        error_message = "Error: invalid spectral type";
    }
    else
    {
        //Update the entry list
        vector<QString> entry;

        entry.push_back(name_value);
        entry.push_back(temperature_value);
        entry.push_back(luminosity_value);
        entry.push_back(spectral_class_value);
        entry.push_back(absolute_magnitude_value);

        entry_list.push_back(entry);

        //Draw the values to the table
        update_table(ui->table_entries);

        //Clear the lineEdit widgets
        ui->lineEdit_input_name->clear();
        ui->lineEdit_input_temperature->clear();
        ui->lineEdit_input_luminosity->clear();

        //Reset the error message
        error_message = "";

        //Update the OpenGL widget
        ui->openGLWidget_diagram->update();

        manual_input = true;
    }
    //If there was an error, the error message is displayed
    ui->label_input_error->setText("<html><head/><body><p><span style=\" color:#ff0000;\">" + error_message + "</span></p></body></html>");
}

//Change the minimum temperature value
void MainWindow::on_spinBox_min_temperature_valueChanged(int arg1)
{
    temp_min = arg1;
    ui->openGLWidget_diagram->update();
}

//Change the maximum temperature value
void MainWindow::on_spinBox_max_temperature_valueChanged(int arg1)
{
    temp_max = arg1;
    ui->openGLWidget_diagram->update();
}

//Change the minimum luminosity value
void MainWindow::on_spinBox_min_luminosity_valueChanged(int arg1)
{
    lum_min = arg1;
    ui->openGLWidget_diagram->update();
}

//Change the maximum luminosity value
void MainWindow::on_spinBox_max_luminosity_valueChanged(int arg1)
{
    lum_max = arg1;
    ui->openGLWidget_diagram->update();
}

//Change the state of the 'show_names' bool
void MainWindow::on_checkBox_show_names_stateChanged(int arg1)
{
    graph_show_names = arg1;
    ui->openGLWidget_diagram->update();
}

//Change the point size on the diagram
void MainWindow::on_doubleSpinBox_point_size_valueChanged(double arg1)
{
    graph_point_size = static_cast<float>(arg1);
    ui->openGLWidget_diagram->update();
}

//Change the opacity of the reference lines
void MainWindow::on_spinBox_line_opacity_valueChanged(int arg1)
{
    graph_lines_opacity = arg1;
    ui->openGLWidget_diagram->update();
}

//Toggle the vertical reference lines
void MainWindow::on_actionVertical_toggled(bool arg1)
{
    graph_show_v_lines = arg1;
    ui->openGLWidget_diagram->update();
}

//Toggle the horizontal reference lines
void MainWindow::on_actionHorizontal_toggled(bool arg1)
{
    graph_show_h_lines = arg1;
    ui->openGLWidget_diagram->update();
}

//Save the diagram as an image
void MainWindow::on_actionExport_as_image_triggered()
{
    FileIOFunctions::save_image(ui->openGLWidget_diagram);
}

//Save the list as a '.sgl' file
void MainWindow::on_actionSave_list_triggered()
{
    FileIOFunctions::save_list_as_new(entry_list);
}

//Clear the current list and starts a new one
void MainWindow::on_actionNew_list_triggered()
{
    entry_list.clear();
    manual_input = false;
    update_table(ui->table_entries); 
    ui->openGLWidget_diagram->update();
    manual_input = true;
}

//Open a '.sgl' file into a new list
void MainWindow::on_actionOpen_list_triggered()
{
    manual_input = false;
    entry_list = FileIOFunctions::open_list();
    update_table(ui->table_entries);
    ui->openGLWidget_diagram->update();
    manual_input = true;
}

//Open a '.csv' file into a new list
void MainWindow::on_actionImport_list_triggered()
{
    manual_input = false;
    entry_list = FileIOFunctions::import_csv();
    update_table(ui->table_entries);
    ui->openGLWidget_diagram->update();
    manual_input = true;
}

void MainWindow::on_table_entries_cellChanged(int row, int column)
{
    QString new_value = ui->table_entries->item(row, column)->text();
    if(manual_input)
    {
        manual_input = 0;
        switch(column)
        {
            case 1:
                entry_list[static_cast<unsigned>(row)][3] = ParameterCalculation::get_spectral_class_str(new_value);
                ui->table_entries->setItem(row, 3, new QTableWidgetItem(ParameterCalculation::get_spectral_class_str(new_value)));
                break;

            case 2:
                entry_list[static_cast<unsigned>(row)][4] = ParameterCalculation::get_absolute_magnitude_str(new_value);
                ui->table_entries->setItem(row, 4, new QTableWidgetItem(ParameterCalculation::get_absolute_magnitude_str(new_value)));
                break;

            case 3:
                ui->table_entries->setItem(row, 3, new QTableWidgetItem(new_value.toUpper()));
                entry_list[static_cast<unsigned>(row)][1] = ParameterCalculation::get_temperature_str(new_value);
                ui->table_entries->setItem(row, 1, new QTableWidgetItem(ParameterCalculation::get_temperature_str(new_value)));
                break;

            case 4:
                entry_list[static_cast<unsigned>(row)][2] = ParameterCalculation::get_relative_luminosity_str(new_value);
                ui->table_entries->setItem(row, 2, new QTableWidgetItem(ParameterCalculation::get_relative_luminosity_str(new_value)));
                break;
        }

        entry_list[static_cast<unsigned>(row)][static_cast<unsigned>(column)] = new_value;
        entry_table = ui->table_entries;
        ui->openGLWidget_diagram->update();
        manual_input = 1;
    }
}

void MainWindow::on_table_entries_cellPressed(int row, int column)
{
    selected_star = row;
    ui->openGLWidget_diagram->update();
}

void MainWindow::on_doubleSpinBox_highlighted_area_valueChanged(double arg1)
{
    graph_pos_square_size = static_cast<int>(arg1 / 2);
    ui->openGLWidget_diagram->update();
}

void MainWindow::on_checkBox_highlight_selected_star_toggled(bool checked)
{
    graph_highlight_selected_star = checked;
    selected_star = -1;

    ui->openGLWidget_diagram->update();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox message_box_about;
    message_box_about.setTextFormat(Qt::RichText);
    message_box_about.setWindowTitle("About StarGraph v" + stargraph_version);
    message_box_about.setText("StarGraph is an open source software developed by Filippo Dusi and licensed under GPL. The official SourceForge repository and the credits for the used assets can be found <a href='https://sourceforge.net/projects/stargraph'>here</a>.");
    message_box_about.setIcon(QMessageBox::Icon::Information);
    message_box_about.setButtonText(1, "Close");
    message_box_about.exec();
}
