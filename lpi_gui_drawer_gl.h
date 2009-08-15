/*
Copyright (c) 2005-2009 Lode Vandevenne
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


#pragma once

#include "lpi_gui_drawer.h"

#include "lpi_gui_base.h"
#include "lpi_color.h"
#include "lpi_texture.h"
#include "lpi_text.h"

namespace lpi
{
namespace gui
{

void initBuiltInGui(const std::string& xmlgui);
void initBuiltInGuiTexturesSmall64(const std::string& png_base64);
void initBuiltInGuiIconsSmall64(const std::string& png_base64);
void initBuiltInGuiTexturesSmall(const std::vector<unsigned char>& png);
void initBuiltInGuiIconsSmall(const std::vector<unsigned char>& png);
void initBuiltInGuiTexturesSmall(const std::string& png_file_name);
void initBuiltInGuiIconsSmall(const std::string& png_file_name);

struct TextureSet //contains the actual texture data, not just pointers
{
  private:
  Texture texture[128];
  
  public:
  Texture& operator[](int index);
};

extern Texture builtInTexture[128];
extern const std::string builtInGuiData;

/*
a BackPanel is a collection of 9 textures that should be tileable (except the corners)
so that you can make a variable size rectangle with it
*/
class BackPanel
{
  public:
  /*
  enable side and/or center texture
  if center is disabled, only sides are drawn (so it can be used to draw a border around something) 
  if sides are disabled, the center texture is used for the whole thing
  */
  bool enableSides;
  bool enableCenter;
  
  //pointers to the 9 different textures making up a panel that can have any size
  const Texture* t00; //top left corner
  const Texture* t01; //top side
  const Texture* t02; //top right corner
  const Texture* t10; //left side
  const Texture* t11; //center
  const Texture* t12; //right side
  const Texture* t20; //bottom left corner
  const Texture* t21; //bottom side
  const Texture* t22; //bottom right corner

  //draw the panel at x, y position, with given width and height
  void draw(int x, int y, int width, int height, const ColorRGB& colorMod=RGB_White) const;
  
  //constructors
  BackPanel();
  BackPanel(int style); //used to quickly generate working BackPanel prototypes in parameters
  
  //make panel with flat color (no textures but fill color)
  void makeUntextured();
  //give 9 separate textures
  void makeTextured9(const Texture* t00=&builtInTexture[0], const Texture* t01=&builtInTexture[1], const Texture* t02=&builtInTexture[2], 
            const Texture* t10=&builtInTexture[3], const Texture* t11=&builtInTexture[4], const Texture* t12=&builtInTexture[5], 
            const Texture* t20=&builtInTexture[6], const Texture* t21=&builtInTexture[7], const Texture* t22=&builtInTexture[8]);
  //give 1 texture, the other 8 are assumed to have successive memory locations
  void makeTextured(const Texture* t00=&builtInTexture[0]);
};

#define DEFAULTPANEL BackPanel(1)

/*
a BackRule is a collection of 3 textures (the center one tileable) that
can form a horizontal or vertical line
*/
class BackRule
{
  public:
  ColorRGB colorMod; //used for modifying the tetures
  
  /*
  if sides are disabled, the center texture is used for the whole thing
  */
  bool enableSides;
  
  Direction direction;
   
  //the 3 different textures making up the line
  const Texture* t0; //left or top corner
  const Texture* t1; //tilable center
  const Texture* t2; //right or bottom corner

  //draw the line at x, y position, with given length
  void draw(int x, int y, int length) const;
  
  //constructors
  BackRule();
  BackRule(int style); //for prototypes
  
  //give 3 separate textures
  void makeHorizontal(const Texture* t0=&builtInTexture[41], Texture* t1=&builtInTexture[42], Texture* t2=&builtInTexture[43], const ColorRGB& colorMod=RGB_White);
  //give 1 texture, the other 2 are assumed to have successive memory locations
  void makeHorizontal1(const Texture* t0=&builtInTexture[41], const ColorRGB& colorMod=RGB_White);
       
  //give 3 separate textures
  void makeVertical(const Texture* t0=&builtInTexture[44], Texture* t1=&builtInTexture[45], Texture* t2=&builtInTexture[46], const ColorRGB& colorMod=RGB_White);
  //give 1 texture, the other 2 are assumed to have successive memory locations
  void makeVertical1(const Texture* t0=&builtInTexture[44], const ColorRGB& colorMod=RGB_White);
};

#define DEFAULTHRULE BackRule(1)
#define DEFAULTVRULE BackRule(2)

extern BackPanel builtInPanel[6];
extern BackRule builtInRule[2];

struct GuiSet
{
  const Texture* windowTextures[9];
  const Texture* buttonTextures[9];
  const Texture* buttonOverTextures[9];
  const Texture* buttonDownTextures[9];
  const Texture* arrowN; //for scrollbars, arrow of droplist, ...
  const Texture* arrowE;
  const Texture* arrowS;
  const Texture* arrowW;
  const Texture* scroller; //for scrollbars
  const Texture* emptyButton;
  const Texture* roundButton;
  const Texture* slider; //the button of a slider, the simplified scrollbar (the slider is what the scroller is to scrollbars)
  const Texture* scrollbarBackground;
  const Texture* checkBox[2];
  const Texture* smallCheckBox[2];
  const Texture* bullet[2];
  const Texture* hline[3];
  const Texture* vline[3];
  const Texture* smiley;
  const Texture* windowTop[3];
  const Texture* closeButton;
  const Texture* resizer; //the resizer at bottom right of a window
  const Texture* whiteButton; //can easily be given any color with color mods
  const Texture* whiteRoundButton;
  const Texture* scrollBarPairCorner; //the cornerpiece of a scrollbarpair
  const Texture* crossHair; //for example to indicate a 2D location on a map, color picker, ...
  const Texture* tabUnSelected[9];
  const Texture* tabSelected[9];
  
  
  const BackPanel* windowPanel;
  const BackPanel* buttonPanel;
  const BackPanel* buttonOverPanel;
  const BackPanel* buttonDownPanel;
  const BackPanel* tabUnSelectedPanel;
  const BackPanel* tabSelectedPanel;
  
  const BackRule* sliderHRule;
  const BackRule* sliderVRule;
  
  ColorRGB mainColor; //if the mouse is not over or down a button
  ColorRGB mouseOverColor; //this isn't for panel buttons, but for image buttons like the arrows of a scrollbar, ...
  ColorRGB mouseDownColor; //this isn't for panel buttons, but for image buttons like the arrows of a scrollbar, ...
  
  Markup panelButtonMarkup[3];
  Markup textButtonMarkup[3];
};

extern GuiSet builtInGuiSet;

class GUIDrawerGL : public IGUIDrawer
{
  private:
    GuiSet* guiset;
    
  public:
    GUIDrawerGL(GuiSet* set);
    
    static void init(); //must be called after screen is created
    
    virtual void drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color);
    virtual void drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled);
    virtual void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled);
    virtual void drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled);
    
    virtual void drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3);
    
    virtual void drawText(const std::string& text, int x = 0, int y = 0, const Markup& markup = TS_W);
    virtual void drawTextCentered(const std::string& text, int x = 0, int y = 0, const Markup& markup = TS_W);
    
    virtual void drawTexture(int x, int y, const Texture* texture, const ColorRGB& colorMod = RGB_White);
    
    virtual void setSmallestScissor(int x0, int y0, int x1, int y1);
    virtual void resetScissor();
    
    //not all GUI parts use all input parameters! only x0 and y0 are always used.
    virtual void drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, bool inactive, const ColorRGB& color);
    
    //input
    virtual IGUIInput& getInput();
};


} //namespace gui
} //namespace lpi


namespace lpi
{
extern gui::GUIDrawerGL gGUIDrawer; //global gui drawer usig GL. Outside of namespace gui on purpose (too long name otherwise)
} //namespace lpi
