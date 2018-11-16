/*
    DIAGRAM HANDLER
*/

#include "gl_diagram.h"
#include "mainwindow.h"

//Assigns the values to the extern variables
int temp_min = 3000;
int temp_max = 25000;
int lum_min = -6;
int lum_max = 6;
int diagram_width = 640;
int diagram_height = 640;
int graph_line_h_step = 2500;
int graph_line_v_step = 10;
int graph_lines_opacity = 25;

bool graph_show_names = false;
bool graph_show_h_lines = false;
bool graph_show_v_lines = true;

//Initializes the widget's promotion to 'GL_Diagram'
GL_Diagram::GL_Diagram(QWidget *parent) : QOpenGLWidget(parent)
{

}

//Initializes the OpenGL widget
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

//Draws to the OpenGL widget
void GL_Diagram::paintGL()
{
    //Clears the buffer before drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //If enabled, draws the reference lines
    DrawingFunctions::draw_reference_lines(graph_show_v_lines, graph_show_h_lines, graph_line_h_step, graph_line_v_step, temp_max - temp_min, lum_max - lum_min, static_cast<float>(static_cast<double>(graph_lines_opacity) / 100.0));

    //Draws a star for every row in the 'entry_list' table
    for(int i = 0; i < entry_list->rowCount(); i++)
    {
        DrawingFunctions::draw_star(entry_list->item(i, 1)->text().toInt(), entry_list->item(i, 2)->text().toDouble(), diagram_height/ 2);
    }

    //Draws the white frame around the diagram
    DrawingFunctions::draw_frame(32);

    //Draws the scale information
    DrawingFunctions::draw_scale_info(this);

    //If enabled, draws the names of the stars
    DrawingFunctions::draw_star_names(this, entry_list);
}

//Resizes the OpenGL widget
void GL_Diagram::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}
