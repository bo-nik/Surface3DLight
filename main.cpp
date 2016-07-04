#include <QApplication> 
#include "scene3D.h"

int main(int argc, char** argv) 
{ 
   QApplication app(argc, argv);

   Scene3D scene;
   scene.setWindowTitle("Surface");
//   scene.resize(500, 500);
   scene.showMaximized();
   scene.setMinimumSize(400, 400);
   scene.show();
 
   return app.exec();
} 
