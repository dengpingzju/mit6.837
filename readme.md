This project includes the assignments of mit 6.837, computer graphics on my own efforts.

Bugs in assignment 4 has been solved and it works as expected.

Assignment 5 seems to have some problem in glCanvas.cpp and in boundingbox.cpp, and I made some changes to them by commenting the code line "glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);" before initializing callback functions and by adding a new line "glEnable(GL_DEPTH_TEST);" to the beginning of BoundingBox::paint(). The result is that it works perfectly well, but I really can't understand why this happens.

Assignment 6

the N function of perlin noise is quite confusing, I don't know how to fit it to the range of [0,1]; what's most surprising is that the example on the assignment webpage shows that simply shift the value by 0.5 , i.e. N(x,y,z)+0.5, is enough. I can't understand it at all. Anyway, I got almost the same visual results. 

The python script runall.py could help simplify calling programs with commandline parameters, and the code itself shows its usage.

Update:

I fix a serious bug for casting shadowing rays, that intersect all the triangles to check whether a triangle occludes the light source, making it degenerate to the original intersecting all scheme and unbelievable slow. The right way is to just go through the gird and announce occlusion once a triangle is hit, no matter what the t is (anyway, the intersection point is in the boundingbox), or announce no hit until all the cells along the way don't trigger a hit, under the assumption that the light source is outside of the boundingbox. This assumption  normally conforms to fact so I reckon this solution a sufficiently good one. In fact, the condition can be loosened to that the light source is outside of the convex hull containing all the vertices of triangles and the view point is on the different side from the light source.

More information in Update section.



Assignment 7

The tent filter is not that mentioned in Tiger book(separable 2d tent filter, in assignment it's just a linear function rotate around to form a cone). 

Note: The browser will blur images of small resolution, so download the image from the assignment webpage and browse it.



Assignment 8

The assignment is relatively easier than previous raytracing algorithm. Nothing to emphasize.



Assignment 9

Very easy, and you can have fun, except that the last optional test case of wind is somewhat difficult since there is no references for it and I have to try many ways randomly. Anyway, the other parts are quite easy.



Important:

I found a very serious problem of visual studio. I compile all above code under DEBUG configuration and this is really a stupid decision, with assert() consuming lots of CPU time. I get rid of some assert and replace fabs function calls with my own version, then compile using RELEASE configuration again, the executive will become much, much more efficient! Be very careful about that!

Some output are not updated, I fix bugs and test with some test cases, and then ignore those wrong; so if you think some image is not correct, you should regenerate it by running the program with corresponding commandline parameters.











