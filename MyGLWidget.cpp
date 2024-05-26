#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>
using namespace std;

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

MyGLWidget::MyGLWidget(QWidget *parent=0) : LL2GLWidget(parent) 
{
}

void MyGLWidget::initializeGL ()
{
  LL2GLWidget::initializeGL();
  connect(&timer, SIGNAL(timeout()), this, SLOT(animar()));
  
}

void MyGLWidget::iniEscena()
{
  radiEscena = sqrt(80);
  centreEscena = glm::vec3(0,0,0);

  headPos = glm::vec3(0,0,0);
  headAngle = 0;
  bodyPos.clear();	    
  bodyPos.push_back(glm::vec3(-1,0,0));    	
  tailPos = glm::vec3(-2,0,0);
  tailAngle = 0;

  marblePos = glm::vec3(10, 0, 0);

  direction = glm::vec3(1,0,0);
  beta = 45;
  alpha = 0;
  ortho = false;
  automatic = false;
}

void MyGLWidget::iniCamera(){

  obs = glm::vec3(0, 6, 0);
  vrp = glm::vec3(0, -2, 0);
  up = glm::vec3(0, 0, -1);
  fov = float(84.5*M_PI)/180;
  znear = 10.63;
  zfar  = 52.88;

  viewTransform();
}



void MyGLWidget::TerraTransform ()
{
  glm::mat4 TG(1.0f);
  TG = glm::scale(TG, glm::vec3(3, 3, 3));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::PipeTransform (glm::vec3 pos)
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, pos);
  TG = glm::scale(TG, glm::vec3(escalaModels[PIPE])*glm::vec3(1,3,1));
  TG = glm::translate(TG, -centreBaseModels[PIPE]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::SnakeHeadTransform ()
{
  glm::mat4 TG(1.0f);  
  TG = glm::translate(TG, headPos);
  TG = glm::rotate(TG, glm::radians(headAngle - 90), glm::vec3(0,1,0));
  TG = glm::scale(TG, glm::vec3(escalaModels[SNAKE_HEAD]));
  TG = glm::translate(TG, -centreBaseModels[SNAKE_HEAD]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::SnakeBodyTransform (glm::vec3 pos)
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, pos);
  TG = glm::scale(TG, glm::vec3(escalaModels[SNAKE_BODY]));
  TG = glm::translate(TG, -centreBaseModels[SNAKE_BODY]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::SnakeTailTransform ()
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, tailPos);
  TG = glm::rotate(TG, glm::radians(tailAngle), glm::vec3(0,1,0));
  TG = glm::scale(TG, glm::vec3(escalaModels[SNAKE_TAIL]));
  TG = glm::translate(TG, -centreBaseModels[SNAKE_TAIL]);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::MarbleTransform () 
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, marblePos);
  TG = glm::scale(TG, glm::vec3(escalaModels[MARBLE])*glm::vec3(0.75,0.75,0.75));
  TG = glm::translate(TG, -centreBaseModels[MARBLE]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::viewTransform ()
{
  // Matriu de posició i orientació de l'observador
  glm::mat4 View(1.0f);
  if (ortho) {
    View = glm::lookAt (obs, vrp, up);
  }
  else {
    View = glm::translate(View, glm::vec3(0,0,-31.62));
    View = glm::rotate(View, (beta + girbeta), glm::vec3(1,0,0));
    View = glm::rotate(View, (alpha + giralpha), glm::vec3(0,1,0));
    View = glm::translate(View, -vrp);
  }
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::projectTransform ()
{
  glm::mat4 Proj(1.0f);
  float l,r,b,t;
  if(ortho) {
    l = -15.0f;
    b = l;
    r = 15.0f;
    t = r;
    if (ra > 1) {
      l *= ra;
      r *= ra;
    }
    else if (ra < 1){
      b /= ra;
      t /= ra;
    }
    Proj = glm::ortho(l, r, b, t, 3.f, 9.f);
  }
  else {
    float ang = glm::radians(84.85/2);
    if(ra < 1) {
      fov = 2*atan(tan(ang)/ra);
    }
    else {
      fov = 2*ang;
    }
    Proj = glm::perspective (fov, ra, znear, zfar);
    LL2GLWidget::projectTransform();
  }
  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
  
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  makeCurrent();
  LL2GLWidget::mousePressEvent(e);
  update();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  if(e->x() > xClick) giralpha -= (e->x() - xClick)*factorAngleX;
  else if (e->x() < xClick) giralpha += (xClick - e->x())*factorAngleX;

  if(e->y() > yClick) girbeta -= (e->y() - yClick)*factorAngleY;
  else if (e->y() < yClick) girbeta += (yClick - e->y())*factorAngleY;
  xClick = e->x();
  yClick = e->y();
  viewTransform();
  update ();
}








void MyGLWidget::paintGL ()
{
  // descomentar per canviar paràmetres
  // glViewport (0, 0, ample, alt);

  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // SnakeHead
  glBindVertexArray (VAO_models[SNAKE_HEAD]);
  SnakeHeadTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[SNAKE_HEAD].faces().size()*3);
  
  // SnakeBody
  glBindVertexArray (VAO_models[SNAKE_BODY]);
  for (unsigned int i = 0; i < bodyPos.size(); i++)
  {  
	  SnakeBodyTransform(bodyPos[i]);
	  glDrawArrays(GL_TRIANGLES, 0, models[SNAKE_BODY].faces().size()*3);
  }

  // SnakeTail
  glBindVertexArray (VAO_models[SNAKE_TAIL]);
  SnakeTailTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[SNAKE_TAIL].faces().size()*3);
  
  // Marble
  glBindVertexArray (VAO_models[MARBLE]);
  MarbleTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[MARBLE].faces().size()*3);

  // Pipe
  glBindVertexArray (VAO_models[PIPE]);
  for (int i = 0; i < 30; ++i) {
    PipeTransform(glm::vec3(14.5-i,0,14.5));
    glDrawArrays(GL_TRIANGLES, 0, models[PIPE].faces().size()*3);
    PipeTransform(glm::vec3(14.5-i,0,-14.5));
    glDrawArrays(GL_TRIANGLES, 0, models[PIPE].faces().size()*3);
    PipeTransform(glm::vec3(14.5,0,14.5-i));
    glDrawArrays(GL_TRIANGLES, 0, models[PIPE].faces().size()*3);
    PipeTransform(glm::vec3(-14.5,0,14.5-i));
    glDrawArrays(GL_TRIANGLES, 0, models[PIPE].faces().size()*3);
  }
  
  
  
  // Terra
  glBindVertexArray (VAO_Terra);
  TerraTransform();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);
}






int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget()
{

}

bool MyGLWidget::checkPosition(glm::vec3 pos)
{
	bool valid = true;
    // Aquí cal que completeu el codi de manera adient
  if ((pos[0] >= 14 or pos[2] >= 14)) valid = false;
  if ((pos[0] <= -14 or pos[2] <= -14)) valid = false;
  for (unsigned int i = 0; i < bodyPos.size(); ++i) {
    if (pos == bodyPos[i]) valid = false;
  }
  
        

	return valid; 
}

void MyGLWidget::animar() {
  makeCurrent();
  LL2GLWidget::updateSnakeGame();
  update();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Up: { 
      direction = glm::vec3(0,0,-1);
      LL2GLWidget::updateSnakeGame();
      break;
    }
    case Qt::Key_Down: { 
      direction = glm::vec3(0,0,1);
      LL2GLWidget::updateSnakeGame();
      break;
        }
    case Qt::Key_Left: { 
      direction = glm::vec3(-1,0,0);
      LL2GLWidget::updateSnakeGame();
      break;
        }  
    case Qt::Key_Right: { 
      headAngle = 90;
      direction = glm::vec3(1,0,0);
      LL2GLWidget::updateSnakeGame();
      break;
        }
    case Qt::Key_C: { 
      if(ortho) ortho = false;
      else ortho = true;
      viewTransform();
      projectTransform();
      break;
        }           
    case Qt::Key_R: { 
      iniEscena();
      iniCamera();
      projectTransform();
      break;
        }  
    case Qt::Key_T: {
      if(automatic) {
        timer.stop();
        automatic = false;
      }
      else {
        timer.start(200);
        automatic = true;
      }
      break;
    }
    default: event->ignore(); break;
  }
  update();
}




