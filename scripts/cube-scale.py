#! /usr/bin/env python
# -*- coding: utf-8 -*-
# Took from https://github.com/vispy/codecamp-esrf-2014/blob/master/nr/tutorial/scripts/hello-world-gl.py
# Now this code scale and rotate a squared colored box :)
# -----------------------------------------------------------------------------
# Copyright (c) 2014, Nicolas P. Rougier. All rights reserved.
# Distributed under the terms of the new BSD License.
# -----------------------------------------------------------------------------
import sys
import ctypes
import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
import math
import random

# http://www.webglacademy.com/courses.php?courses=0|1|20|2|3|4|23|5|6|7|10#3

vertex_code = """
    uniform float scale;
    attribute vec4 color;
    attribute vec3 position;
    uniform mat4 u_view;
    uniform mat4 transform;
    varying vec4 v_color;
    void main()
    {
        gl_Position = transform * vec4(0.5*position, 1.0);
        v_color = color;
    } """

fragment_code = """
    varying vec4 v_color;
    void main()
    {
        gl_FragColor = v_color;
    } """

def display():
    gl.glClear(gl.GL_COLOR_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT)
    #GL.glDepthFunc(gl.GL_LEQUAL)
    #gl.glDepthMask(gl.GL_TRUE)
    
        
    #gl.glDrawArrays(gl.GL_TRIANGLE_STRIP, 0, 5)
    #gl.glDrawElements(gl.GL_TRIANGLES, len(Indices), gl.GL_UNSIGNED_INT, None)
    gl.glDrawElements(gl.GL_TRIANGLES, len(Indices), gl.GL_UNSIGNED_INT, ctypes.c_void_p(0))
    glut.glutSwapBuffers()

def reshape(width,height):
    gl.glViewport(0, 0, width, height)

def keyboard( key, x, y ):
    if key == '\033':
        sys.exit( )

def timer(fps):
    global clock
    global data
    global phi, theta
    
    clock += 0.005 * 1000.0/fps

    # for scaling
    #loc = gl.glGetUniformLocation(program, "scale")
    #gl.glUniform1f(loc, (1+np.cos(clock))/2.0)
    
    loc = gl.glGetUniformLocation(program, "transform")
#    transform['transform'] = [ 
#        (np.cos(clock),np.sin(clock),0,0), 
#        (-np.sin(clock),np.cos(clock),0,0), 
#        (0,0,1,0), (0,0,0,1) ]
    # gl.glUniformMatrix4fv(loc, 1, False, transform['transform'])
    
    # Rotate cube
    theta += 0.5 # degrees
    phi   += 0.5 # degrees
    model = np.eye(4, dtype=np.float32)
    rotate(model, theta, 0, 0, 1)
    rotate(model, phi, 0, 1, 0)
    gl.glUniformMatrix4fv(loc, 1, False, model)
    
    glut.glutTimerFunc(1000/fps, timer, fps)
    glut.glutPostRedisplay()



def rotate(M, angle, x, y, z, point=None):
    angle = math.pi * angle / 180
    c, s = math.cos(angle), math.sin(angle)
    n = math.sqrt(x * x + y * y + z * z)
    x, y, z = x/n, y/n, z/n
    cx, cy, cz = (1 - c) * x, (1 - c) * y, (1 - c) * z
    R = np.array([[cx * x + c, cy * x - z * s, cz * x + y * s, 0],
                  [cx * y + z * s, cy * y + c, cz * y - x * s, 0],
                  [cx * z - y * s, cy * z + x * s, cz * z + c, 0],
                  [0, 0, 0, 1]], dtype=M.dtype).T
    M[...] = np.dot(M, R)
    return M


def translate(M, x, y=None, z=None):
    y = x if y is None else y
    z = x if z is None else z
    T = np.array([[1.0, 0.0, 0.0, x],
                  [0.0, 1.0, 0.0, y],
                  [0.0, 0.0, 1.0, z],
                  [0.0, 0.0, 0.0, 1.0]], dtype=M.dtype).T
    M[...] = np.dot(M, T)
    return M
# GLUT init
# --------------------------------------
glut.glutInit()
glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGBA | glut.GLUT_DEPTH)
glut.glutCreateWindow('Hello world!')
glut.glutReshapeWindow(512,512)
glut.glutReshapeFunc(reshape)
glut.glutKeyboardFunc(keyboard)
glut.glutDisplayFunc(display)
glut.glutTimerFunc(1000/60, timer, 60)

# Build data
# --------------------------------------

Vertices = np.zeros(8, [("position", np.float32, 3)] )
Vertices["position"] = [( 1, 1, 1), (-1, 1, 1), (-1,-1, 1), ( 1,-1, 1),
                 ( 1,-1,-1), ( 1, 1,-1), (-1, 1,-1), (-1,-1,-1)]
                 
Color = np.zeros(32, [("color", np.float32,4)] )
for i in range(8):
    Color['color'][i*3] = [1, 0, 1, 1]
    Color['color'][i*3+1] = [1, 0, 1, 1]
    Color['color'][i*3+2] = [1, 0, 1, 1]

Indices = np.array([0,1,2, 0,2,3,  0,3,4, 0,4,5,  0,5,6, 0,6,1,
                      1,6,7, 1,7,2,  7,4,3, 7,3,2,  4,7,6, 4,6,5], dtype=np.uint32)

transform = np.zeros(4, [ ("transform", np.float32, 4) ])
transform['transform'] = [ (1,0,0,0), (0,1,0,0), (0,0,1,0), (0,0,0,1) ]

# Build & activate program
# --------------------------------------

# Request a program and shader slots from GPU
program  = gl.glCreateProgram()
vertex   = gl.glCreateShader(gl.GL_VERTEX_SHADER)
fragment = gl.glCreateShader(gl.GL_FRAGMENT_SHADER)

# Set shaders source
gl.glShaderSource(vertex, vertex_code)
gl.glShaderSource(fragment, fragment_code)

# Compile shaders
gl.glCompileShader(vertex)
gl.glCompileShader(fragment)

# Attach shader objects to the program
gl.glAttachShader(program, vertex)
gl.glAttachShader(program, fragment)

# Build program
gl.glLinkProgram(program)

# Get rid of shaders (no more needed)
gl.glDetachShader(program, vertex)
gl.glDetachShader(program, fragment)

# Make program the default program
gl.glUseProgram(program)


# Build buffer
# --------------------------------------

# Request a buffer slot from GPU
bufferV = gl.glGenBuffers(1)
# Make this buffer the default one
gl.glBindBuffer(gl.GL_ARRAY_BUFFER, bufferV)
# Upload data
gl.glBufferData(gl.GL_ARRAY_BUFFER, Vertices.nbytes, Vertices, gl.GL_DYNAMIC_DRAW)


# Request a buffer slot from GPU
bufferC = gl.glGenBuffers(1)
# Make this buffer the default one
gl.glBindBuffer(gl.GL_ARRAY_BUFFER, bufferC)
# Upload data
gl.glBufferData(gl.GL_ARRAY_BUFFER, Color.nbytes, Color, gl.GL_DYNAMIC_DRAW)


bufferI = gl.glGenBuffers(1)
# Make this buffer the default one
gl.glBindBuffer(gl.GL_ELEMENT_ARRAY_BUFFER, bufferI)
# Upload data
gl.glBufferData(gl.GL_ELEMENT_ARRAY_BUFFER, Indices.nbytes, Indices, gl.GL_STATIC_DRAW)



# Bind attributes
# --------------------------------------

# vertices positions
stride = Vertices.strides[0]
offset = ctypes.c_void_p(0)
loc = gl.glGetAttribLocation(program, "position")
gl.glEnableVertexAttribArray(loc)
gl.glBindBuffer(gl.GL_ARRAY_BUFFER, bufferV)
gl.glVertexAttribPointer(loc, 3, gl.GL_FLOAT, False, stride, offset)

# adding color
stride = Color.strides[0]
offset = ctypes.c_void_p(0)
loc = gl.glGetAttribLocation(program, "color")
gl.glEnableVertexAttribArray(loc)
gl.glBindBuffer(gl.GL_ARRAY_BUFFER, bufferC)
gl.glVertexAttribPointer(loc, 4, gl.GL_FLOAT, False, stride, offset)

# transform matrix
offset = ctypes.c_void_p(0)
loc = gl.glGetUniformLocation(program, "transform")
gl.glUniformMatrix4fv(loc, 1, False, transform['transform'])

# Bind uniforms
# --------------------------------------
loc = gl.glGetUniformLocation(program, "scale")
gl.glUniform1f(loc, 1.0)


#loc = gl.glGetUniformLocation(program, "u_view")
#view = translate(np.eye(4), 0, 0, -5)
#gl.glUniformMatrix4fv(loc, 1, False, view)

# For rendering
clock = 0

phi, theta = 40, 30


# config
gl.glEnable(gl.GL_DEPTH_TEST)
#gl.glDepthFunc(gl.GL_LEQUAL)

#gl.glDisable(gl.GL_BLEND)
#gl.glEnable(gl.GL_POLYGON_OFFSET_FILL)

# Enter mainloop
# --------------------------------------
glut.glutMainLoop()
