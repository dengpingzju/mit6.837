This project includes the assignments of mit 6.837, computer graphics on my own efforts.

Bugs in assignment 4 has been solved and it works as expected.

Assignment 5 seems to have some problem in glCanvas.cpp and in boundingbox.cpp, and I made some changes to them by commenting the code line "glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);" before initializing callback functions and by adding a new line "glEnable(GL_DEPTH_TEST);" to the beginning of BoundingBox::paint(). The result is that it works perfectly well, but I really can't understand why this happens.

