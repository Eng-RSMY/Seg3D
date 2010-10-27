/*
 For more information, please see: http://software.sci.utah.edu

 The MIT License

 Copyright (c) 2009 Scientific Computing and Imaging Institute,
 University of Utah.


 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */

#ifndef APPLICATION_TOOLS_POLYLINETOOL_H
#define APPLICATION_TOOLS_POLYLINETOOL_H

#include <Application/Tool/SingleTargetTool.h>

namespace Seg3D
{

class PolylineToolPrivate;
typedef boost::shared_ptr< PolylineToolPrivate > PolylineToolPrivateHandle;

class PolylineTool : public SingleTargetTool
{

SEG3D_TOOL
(
  SEG3D_TOOL_NAME( "PolylineTool", "Tool for drawing with polylines" )
  SEG3D_TOOL_MENULABEL( "Polyline" )
  SEG3D_TOOL_MENU( "Tools" )
  SEG3D_TOOL_SHORTCUT_KEY( "Alt+Y" )
  SEG3D_TOOL_URL( "http://seg3d.org/" )
  SEG3D_TOOL_HOTKEYS( "Add Point=Left Mouse Button|Move Point=Right Mouse Button"
    "Add Point in Segment=ALT Left Mouse Button|Delete Point=ALT Right Mouse Button"
    "|Paint Floodfill=F|Erase Floodfill=E" )  
)

  // -- constructor/destructor --
public:
  PolylineTool( const std::string& toolid );
  virtual ~PolylineTool();

public:

  // HANDLE_MOUSE_PRESS:
  // Called when a mouse button has been pressed.
  virtual bool handle_mouse_press( ViewerHandle viewer, 
    const Core::MouseHistory& mouse_history, 
    int button, int buttons, int modifiers );
    
  // HANDLE_MOUSE_RELEASE:
  // Called when a mouse button has been released.
  virtual bool handle_mouse_release( ViewerHandle viewer, 
    const Core::MouseHistory& mouse_history, 
    int button, int buttons, int modifiers );
    
  // HANDLE_MOUSE_MOVE:
  // Called when the mouse moves in a viewer.
  virtual bool handle_mouse_move( ViewerHandle viewer, 
    const Core::MouseHistory& mouse_history, 
    int button, int buttons, int modifiers );

  // HANDLE_KEY_PRESS:
  // Called when a key is pressed
  virtual bool handle_key_press( ViewerHandle viewer, int key, int modifiers ); 
  
  // REDRAW:
  // Draw seed points in the specified viewer.
  // The function should only be called by the renderer, which has a valid GL context.
  virtual void redraw( size_t viewer_id, const Core::Matrix& proj_mat );

  // HAS_2D_VISUAL:
  // Returns true if the tool draws itself in the 2D view, otherwise false.
  // The default implementation returns false.
  virtual bool has_2d_visual();

  // -- dispatch functions --
public:
  void fill( Core::ActionContextHandle context );
  void erase( Core::ActionContextHandle context );
  void reset( Core::ActionContextHandle context );

  // -- State Variables --
public:
  Core::StatePointVectorHandle vertices_state_;

private:
  PolylineToolPrivateHandle private_;
};

} // end namespace

#endif
