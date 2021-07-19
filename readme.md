This project includes the assignments of mit 6.837, computer graphics on my own efforts.

Bugs in assignment 4 has been solved and it works as expected.

Assignment 5 seems to have some problem in glCanvas.cpp and in boundingbox.cpp, and I made some changes to them by commenting the code line "glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);" before initializing callback functions and by adding a new line "glEnable(GL_DEPTH_TEST);" to the beginning of BoundingBox::paint(). The result is that it works perfectly well, but I really can't understand why this happens.

Assignment 6

the N function of perlin noise is quite confusing, I don't know how to fit it to the range of [0,1]; what's most surprising is that the example on the assignment webpage shows that simply shift the value by 0.5 , i.e. N(x,y,z)+0.5, is enough. I can't understand it at all. Anyway, I got almost the same visual results. 

Note that the last test case of the logo of mit 6.837 requires a lot of time to render(it took mu up to 2 hours to render ti). The python script runall.py could help simplify calling programs by commandline parameters. 



Assignment 7

the test cases of scene7_01_sphere_triangle.txt given on the assignment webpage are apparently not correct, since the pictures can never be 12*12 resolution; but the results are quite similar, and alleviate the phenomenon of aliasing. The 6.837 logo test case requires over 8 hours to render based my algorithm and machine. And the tent filter is not the one mentioned in Tiger book(separable 2d tent filter, but just a linear function rotate around to form a cone). 



Assignment 8

The assignment is relatively easier than previous raytracing algorithm. Nothing to emphasize. But the last test case seems to require time to complete the raytracing.





