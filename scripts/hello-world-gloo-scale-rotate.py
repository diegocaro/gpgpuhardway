#! /usr/bin/env python
# -*- coding: utf-8 -*-
# Took from https://github.com/vispy/codecamp-esrf-2014/blob/master/nr/tutorial/scripts/hello-world-gloo-scale.py
# Now this code also rotate :)
# 
# -----------------------------------------------------------------------------
# Copyright (c) 2014, Nicolas P. Rougier. All rights reserved.
# Distributed under the terms of the new BSD License.
# -----------------------------------------------------------------------------
import sys
import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
from gloo import Program, VertexBuffer, IndexBuffer

vertex = """
    uniform float scale;
    attribute vec4 color;
    attribute vec2 position;
    uniform mat4 m_transform;
    varying vec4 v_color;
    void main()
    {
        gl_Position = m_transform * vec4(scale*position, 0.0, 1.0);
        v_color = color;
    } """

fragment = """
    varying vec4 v_color;
    void main()
    {
        gl_FragColor = v_color;
    } """

def display():
    gl.glClearColor(1,1,1,1)
    gl.glClear(gl.GL_COLOR_BUFFER_BIT)
    program.draw(gl.GL_TRIANGLE_STRIP)
    glut.glutSwapBuffers()

def reshape(width,height):
    gl.glViewport(0, 0, width, height)

def keyboard( key, x, y ):
    if key == '\033': sys.exit( )

def timer(fps):
    global clock
    clock += 0.005 * 1000.0/fps
    program['scale'] = (1+np.cos(clock))/2.0
    program['m_transform'] = [ (np.cos(clock),np.sin(clock),0,0), (-np.sin(clock),np.cos(clock),0,0), (0,0,1,0), (0,0,0,1) ]
    print program['m_transform']
    glut.glutTimerFunc(1000/fps, timer, fps)
    glut.glutPostRedisplay()

# Glut init
# --------------------------------------
glut.glutInit(sys.argv)
glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGBA)
glut.glutCreateWindow('Hello world!')
glut.glutReshapeWindow(512,512)
glut.glutReshapeFunc(reshape)
glut.glutKeyboardFunc(keyboard )
glut.glutDisplayFunc(display)
glut.glutTimerFunc(1000/60, timer, 60)

# Build program & data
# ----------------------------------------
program = Program(vertex, fragment, count=4)
program['color']    = [ (1,0,0,0), (0,1,0,1), (0,0,1,1), (1,1,0,1) ]
program['position'] = [ (-1,-1),   (-1,+1),   (+1,-1),   (+1,+1)   ]
program['m_transform'] = [ (1,0,0,0), (0,1,0,0), (0,0,1,0), (0,0,0,1) ]
clock = 0

# Enter mainloop
# --------------------------------------
glut.glutMainLoop()
