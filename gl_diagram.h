/*
    DIAGRAM PAINTER
*/

#pragma once

#include <QFileDialog>
#include <QOpenGLWidget>
#include <QTableWidget>
#include <QPainter>
#include <QMouseEvent>

//Include the converter
#include "converter.h"

using namespace std;

//Declare the parameters used for drawing
extern int temp_min, temp_max, lum_min, lum_max, diagram_height, diagram_width, graph_line_h_step, graph_line_v_step, graph_lines_opacity, graph_pos_square_size;
extern float graph_point_size;
extern bool graph_show_names, graph_show_h_lines, graph_show_v_lines, graph_highlight_selected_star;

//Initialize the OpenGL widget class
class GL_Diagram : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GL_Diagram(QWidget *parent = nullptr);

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};


//Class containing the functions used to find the coordinates of a star on the diagram
class CoordsFunctions
{
public:
    //Calculates the x coordinates of a star from its temperature
    static int diagram_get_x(int temp_offset, int width, int range, int star_temp)
    {
        return static_cast<int>(static_cast<double>(width) /  static_cast<double>(range) * (range - (star_temp - temp_offset)));
    }

    //Calculates the y coordinates of a star from its luminosity
    static int  diagram_get_y(int y_offset, int height, int range, double star_lum)
    {
        return static_cast<int>(y_offset - (static_cast<double>(height) / static_cast<double>(range) * MathFunctions::log_base_10(star_lum)));
    }
};

//Class containing the functions used to draw the diagram
class DrawingFunctions
{
public:   
    //Draw the white frame around the diagram
    static void draw_frame(int offset)
    {
        //Set the viewport to match the OpenGL widget
        glViewport(0, 0, diagram_width, diagram_height);

        //Draw the four lines which compose the frame
        glLineWidth(2.0);
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);

        //Top line
        glVertex2i(offset, offset);
        glVertex2i(diagram_width - offset, offset);
        //Bottom line
        glVertex2i(offset, diagram_height - offset);
        glVertex2i(diagram_width - offset, diagram_height - offset);
        //Left line
        glVertex2i(offset, offset);
        glVertex2i(offset, diagram_height - offset);
        //Right line
        glVertex2i(diagram_width - offset, offset);
        glVertex2i(diagram_width - offset, diagram_height - offset);

        glEnd();
    }

    //Draw the scale information
    static void draw_scale_info(QPaintDevice *device)
    {
        //Set the viewport to match the OpenGL widget
        glViewport(0, 0, diagram_width, diagram_height);

        //Create the painter and set the viewport
        QPainter painter(device);
        painter.setViewport(0, 0, diagram_width, diagram_height);

        //Set the font to use
        QFont scale_font("XITS Math", 12);
        painter.setFont(scale_font);

        //Draw the text
        painter.drawText(2, 312, "logL");
        painter.drawText(8, 32, QString::number(lum_max));
        painter.drawText(8, 608, QString::number(lum_min));

        painter.drawText(288, 632, "Temperature");
        painter.drawText(8, 632, QString::number(temp_max) + " K");
        painter.drawText(576, 632, QString::number(temp_min) + " K");

        //Terminate the painter
        painter.end();
    }
    //Draw the reference lines
    static void draw_reference_lines(bool vertical, bool horizontal, int h_step,  int v_step, int temp_range, float brightness)
    {
        //Set the viewport to match the inner drawing area
        glViewport(32, 32, diagram_width - 64, diagram_height - 64);

        //Draws the reference lines
        glLineWidth(1.0);
        glColor3f(brightness, brightness, brightness);
        glBegin(GL_LINES);

        //If vertical reference lines are enabled, their position is calculated from the value of 'h_step'
        if(vertical)
        {
            for(int i = 0; i < 50; i += 1)
            {
                int line_x = CoordsFunctions::diagram_get_x(temp_min, diagram_width - 64, temp_range, h_step * i);

                glVertex2i(line_x, 0);
                glVertex2i(line_x, diagram_height);
            }
        }
        //If horizontal reference lines are enabled, their position is calculated two times (using opposite ranges) from the value of 'v_step'
        if(horizontal)
        {
            for(int i = 0; i < 8; i ++)
            {
                int line_y_positive = CoordsFunctions::diagram_get_y(diagram_height / 2, diagram_height - 64, lum_max, static_cast<double>(MathFunctions::power_of(v_step, i)));
                int line_y_negative = CoordsFunctions::diagram_get_y(diagram_height / 2, diagram_height - 64, lum_min , static_cast<double>(MathFunctions::power_of(v_step, i)));

                glVertex2i(0, line_y_positive);
                glVertex2i(diagram_width, line_y_positive);

                glVertex2i(0, line_y_negative);
                glVertex2i(diagram_width, line_y_negative);
            }
        }

        glEnd();
    }

    //Draw a single star on the diagram
    static void draw_star(int star_temperature, double star_luminosity, int luminosity_offset)
    {
        //Set the viewport to match the OpenGL widget
        glViewport(32, 32, diagram_width - 64, diagram_height - 64);

        //Calculate the coordinates of the star
        int star_x = CoordsFunctions::diagram_get_x(temp_min, diagram_width - 64, temp_max - temp_min, star_temperature);
        int star_y;
        if(star_luminosity < 1)
        {
            star_y = CoordsFunctions::diagram_get_y(luminosity_offset, diagram_height - 64, -lum_min, star_luminosity);
        }
        else
        {
            star_y = CoordsFunctions::diagram_get_y(luminosity_offset, diagram_height - 64, lum_max, star_luminosity);
        }

        //Assign a colour to the star
        double col = (1.0 / 13000) * (star_temperature - 3000);

        //The drawing colour is calculated from the value of 'col' using RGB
        glColor3f(static_cast<float>(1 - col), static_cast<float>((col / 2) + 0.5), static_cast<float>(col));

        //Draw the star to the diagram
        glPointSize(graph_point_size);
        glBegin(GL_POINTS);
        glVertex2i(star_x, star_y);
        glEnd();
    }

    //Draw the names of the stars
    static void draw_star_names(QPaintDevice *device, vector<vector<QString>> list)
    {
        //Set the viewport to match the OpenGL widget
        glViewport(32, 32, diagram_width - 64, diagram_height - 64);

        //Create the painter and sets the viewport
        QPainter painter(device);       
        painter.setViewport(32, 32, diagram_width - 64, diagram_height - 64);

        if(graph_show_names)
        {
            for(unsigned i = 0; i < list.size(); i++)
            {
                //Calculate the position of the label
                int star_x = static_cast<int>(CoordsFunctions::diagram_get_x(temp_min, diagram_width - 64, temp_max - temp_min, list[i][1].toInt())) + 8;
                int star_y;
                if(list[i][2].toDouble() < 1)
                {
                    star_y = static_cast<int>(CoordsFunctions::diagram_get_y(diagram_height / 2, diagram_height - 64, -lum_min, list[i][2].toDouble())) + 8;
                }
                else
                {
                    star_y = static_cast<int>(CoordsFunctions::diagram_get_y(diagram_height / 2, diagram_height - 64, lum_max, list[i][2].toDouble())) + 8;
                }
                //Set the font to use
                QFont graph_font("Verdana", 10);
                painter.setFont(graph_font);

                //Draw the text
                painter.drawText(star_x, star_y, list[i][0]);
            }
        }

        //Terminate the painter
        painter.end();
    }

    //Draw a square indicating the area in which the selected star is located
    static void draw_star_pos_square(vector<QString> star_params, QPaintDevice *device)
    {
        int center_x = static_cast<int>(CoordsFunctions::diagram_get_x(temp_min, diagram_width - 64, temp_max - temp_min, star_params[1].toInt()));
        int center_y;

        if(star_params[2].toDouble() < 1)
        {
            center_y = static_cast<int>(CoordsFunctions::diagram_get_y(diagram_height / 2, diagram_height - 64, -lum_min, star_params[2].toDouble()));
        }
        else
        {
            center_y = static_cast<int>(CoordsFunctions::diagram_get_y(diagram_height / 2, diagram_height - 64, lum_max, star_params[2].toDouble()));
        }

        //Set the viewport to match the OpenGL widget
        glViewport(32, 32, diagram_width - 64, diagram_height - 64);

        glColor3f(1.0, 0.0, 1.0);
        glBegin(GL_LINES);

        glVertex2i(center_x - graph_pos_square_size, center_y - graph_pos_square_size);
        glVertex2i(center_x + graph_pos_square_size, center_y - graph_pos_square_size);

        glVertex2i(center_x - graph_pos_square_size, center_y + graph_pos_square_size);
        glVertex2i(center_x + graph_pos_square_size, center_y + graph_pos_square_size);

        glVertex2i(center_x - graph_pos_square_size, center_y - graph_pos_square_size);
        glVertex2i(center_x - graph_pos_square_size, center_y + graph_pos_square_size);

        glVertex2i(center_x + graph_pos_square_size, center_y - graph_pos_square_size);
        glVertex2i(center_x + graph_pos_square_size, center_y + graph_pos_square_size);

        glEnd();

        QPainter painter(device);
        painter.setViewport(32, 32, diagram_width - 64, diagram_height - 64);

        //Set the font to use
        QFont graph_font("Verdana", 10);
        painter.setFont(graph_font);

        //Draw the text
        painter.drawText(center_x - graph_pos_square_size, center_y - (graph_pos_square_size + 8), star_params[0]);

        //Terminate the painter
        painter.end();
    }
};
