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

vertex_code = """
    uniform float scale;
    attribute vec4 color;
    attribute vec2 position;
    uniform mat4 transform;
    varying vec4 v_color;
    void main()
    {
        gl_Position = transform * vec4(scale*position, 0.0, 1.0);
        v_color = color;
    } """

fragment_code = """
    varying vec4 v_color;
    void main()
    {
        gl_FragColor = v_color;
    } """

def display():
    gl.glClear(gl.GL_COLOR_BUFFER_BIT)
    gl.glDrawArrays(gl.GL_TRIANGLE_STRIP, 0, 4)
    glut.glutSwapBuffers()

def reshape(width,height):
    gl.glViewport(0, 0, width, height)

def keyboard( key, x, y ):
    if key == '\033':
        sys.exit( )

def timer(fps):
    global clock
    global data
    clock += 0.005 * 1000.0/fps
    
    loc = gl.glGetUniformLocation(program, "scale")
    gl.glUniform1f(loc, (1+np.cos(clock))/2.0)
    
    loc = gl.glGetUniformLocation(program, "transform")
    data['transform'] = [ (np.cos(clock),np.sin(clock),0,0), (-np.sin(clock),np.cos(clock),0,0), (0,0,1,0), (0,0,0,1) ]
    gl.glUniformMatrix4fv(loc, 1, False, data['transform'])
    
    glut.glutTimerFunc(1000/fps, timer, fps)
    glut.glutPostRedisplay()

# GLUT init
# --------------------------------------
glut.glutInit()
glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGBA)
glut.glutCreateWindow('Hello world!')
glut.glutReshapeWindow(512,512)
glut.glutReshapeFunc(reshape)
glut.glutKeyboardFunc(keyboard)
glut.glutDisplayFunc(display)
glut.glutTimerFunc(1000/60, timer, 60)

# Build data
# --------------------------------------
data = np.zeros(4, [("position", np.float32, 2),
                    ("color",    np.float32, 4),
                    ("transform", np.float32, 4)])

data['position'] = [ (-1,-1),   (-1,+1),   (+1,-1),   (+1,+1)   ]
data['color']    = [ (1,0,0,1), (0,1,0,1), (0,0,1,1), (1,1,0,1) ]
data['transform'] = [ (1,0,0,0), (0,1,0,0), (0,0,1,0), (0,0,0,1) ]

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
buffer = gl.glGenBuffers(1)

# Make this buffer the default one
gl.glBindBuffer(gl.GL_ARRAY_BUFFER, buffer)

# Upload data
gl.glBufferData(gl.GL_ARRAY_BUFFER, data.nbytes, data, gl.GL_DYNAMIC_DRAW)


# Bind attributes
# --------------------------------------

# vertices positions
stride = data.strides[0]
offset = ctypes.c_void_p(0)
loc = gl.glGetAttribLocation(program, "position")

gl.glEnableVertexAttribArray(loc)
gl.glBindBuffer(gl.GL_ARRAY_BUFFER, buffer)
gl.glVertexAttribPointer(loc, 3, gl.GL_FLOAT, False, stride, offset)

# adding color
offset = ctypes.c_void_p(data.dtype["position"].itemsize)
loc = gl.glGetAttribLocation(program, "color")

gl.glEnableVertexAttribArray(loc)
gl.glBindBuffer(gl.GL_ARRAY_BUFFER, buffer)
gl.glVertexAttribPointer(loc, 4, gl.GL_FLOAT, False, stride, offset)

# transform matrix
offset = ctypes.c_void_p(data.dtype["position"].itemsize+data.dtype["color"].itemsize)
loc = gl.glGetUniformLocation(program, "transform")

#gl.glEnableVertexAttribArray(loc)
#gl.glBindBuffer(gl.GL_ARRAY_BUFFER, buffer)
gl.glUniformMatrix4fv(loc, 1, False, data['transform'])

# Bind uniforms
# --------------------------------------
loc = gl.glGetUniformLocation(program, "scale")
gl.glUniform1f(loc, 1.0)

# For rendering
clock = 0


# Enter mainloop
# --------------------------------------
glut.glutMainLoop()
