HW1 ReadMe

examlpe1.cpp - main program
vshader1.glsl - simple vertex shader that applies a projection matrix
fshade1.glsl - sets color to black
vshader2.glsl - vertex shader that applies a projection matrix and passes through a color attribute
fshader2.glsl - fragment shader that applies the input color

Most of the geometry is created in generateGeometry. There is a function ,loadPolyline, which loads polyline files and puts the data in a struct for later use. generateHilbertCurve creates a hilbert curve at the current iteration and creates GPU buffers for it.The coloring is done in this function, by creating a quad around each point and following the color pattern. The display function routes to another function to display content based on the current state of the app (either the triangle, a polyline figure, or hilbert curve). 