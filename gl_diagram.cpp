/*
    DIAGRAM HANDLER
*/

#include "gl_diagram.h"
#include "mainwindow.h"

#include <qdebug.h>

//Assign the default values to the extern variables
int temp_min = 3000;
int temp_max = 15000;
int lum_min = -8;
int lum_max = 8;
int diagram_width = 640;
int diagram_height = 640;
int graph_line_h_step = 2500;
int graph_line_v_step = 10;
int graph_lines_opacity = 25;
int graph_pos_square_size = 32;

float graph_point_size = 3.0;

bool graph_show_names = false;
bool graph_show_h_lines = false;
bool graph_show_v_lines = true;
bool graph_highlight_selected_star = false;

//Initialize the widget's promotion to 'GL_Diagram'
GL_Diagram::GL_Diagram(QWidget *parent) : QOpenGLWidget(parent)
{
}

//Initialize the OpenGL widget
void GL_Diagram::initializeGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, diagram_width, diagram_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, diagram_width, diagram_height, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//Draw to the OpenGL widget
void GL_Diagram::paintGL()
{
    //Clear the buffer before drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //If enabled, draw the reference lines
    DrawingFunctions::draw_reference_lines(graph_show_v_lines, graph_show_h_lines, graph_line_h_step, graph_line_v_step, temp_max - temp_min, static_cast<float>(static_cast<double>(graph_lines_opacity) / 100.0));

    //Draw a star for each row in the 'entry_table' table
    for(unsigned i = 0; i < entry_list.size(); i++)
    {
        DrawingFunctions::draw_star(entry_list[i][1].toInt(), entry_list[i][2].toDouble(), diagram_height/ 2);
    }

    //Draw the white frame around the diagram
    DrawingFunctions::draw_frame(32);

    //Draw the scale information
    DrawingFunctions::draw_scale_info(this);

    //If enabled, draw the names of the stars
    DrawingFunctions::draw_star_names(this, entry_list);

    //Draw a square around the selected star on the diagram
    if(selected_star != -1 && graph_highlight_selected_star)
    {
        DrawingFunctions::draw_star_pos_square(entry_list[static_cast<unsigned>(selected_star)], this);
    }
}

//Resize the OpenGL widget
void GL_Diagram::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}
