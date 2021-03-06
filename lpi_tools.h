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


/*
lpi_tools:
Tools for lpi itself, e.g. functions that act as programs to convert PNG images
to/from base64 files, to create built-in textures for GUI and font or extract them.
*/

#pragma once

#include <string>

namespace lpi
{
  namespace gui { class IGUIDrawer; }

  void binaryFileToBase64File(const std::string& outfilename, const std::string& infilename, bool cppnewlines);
  void base64FileToBinaryFile(const std::string& outfilename, const std::string& infilename);
  void base64StringToBinaryFile(const std::string& outfilename, const std::string& in);
  
  void graphicalKeyBoardNumberTest(gui::IGUIDrawer& drawer);

} //namespace lpi

