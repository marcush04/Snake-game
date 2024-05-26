#include "LL2GLWidget.h"

#include <vector>

#include <QTimer>

class MyGLWidget : public LL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent);
    ~MyGLWidget();

  protected:
  
    virtual void keyPressEvent (QKeyEvent *event);
    virtual void initializeGL ( );
    virtual void iniEscena ();
    virtual void iniCamera (); 
    virtual void viewTransform ();
    virtual void projectTransform ();
    
    virtual void TerraTransform ();
    virtual void PipeTransform (glm::vec3 pos);  
    virtual void SnakeHeadTransform();
    virtual void SnakeBodyTransform(glm::vec3 pos);
    virtual void SnakeTailTransform();
    virtual void MarbleTransform();
    virtual void paintGL();
    virtual void mousePressEvent (QMouseEvent *event);
    //virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);

    // funció per comprovar si una nova posició de la serp és vàlida
    virtual bool checkPosition(glm::vec3 pos);
    
    
  public slots:
    void animar();

  private:
    float alpha, beta, giralpha, girbeta;
    bool ortho;
    bool automatic;
    QTimer timer;
    int printOglError(const char file[], int line, const char func[]);
   
};
