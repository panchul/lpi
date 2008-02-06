/*
Copyright (c) 2005-2008 Lode Vandevenne
All rights reserved.

This file is part of Lode's Programming Interface.

Lode's Programming Interface is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lode's Programming Interface is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lode's Programming Interface.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LPI_GL_H_INCLUDED
#define LPI_GL_H_INCLUDED

#include <GL/gl.h>

namespace lpi
{

void initGL();
void set2DScreen();
void set3DScreen(double near, double far);

int screenWidth();
int screenHeight();
bool onScreen(int x, int y);

void setScissor(int left, int top, int right, int bottom);
void setSmallestScissor(int left, int top, int right, int bottom); //same as setScissor, but will new scissor area will be inside the old scissor area, all parts outside are removed
void setOpenGLScissor();
void resetScissor();

void enableOneSided();
void enableTwoSided();
void enableZBuffer();
void disableZBuffer();
void enableSmoothing();
void disableSmoothing();
bool enabledSmoothing();

} //namespace lpi

#endif