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

#ifndef APPLICATION_TOOL_TOOL_H
#define APPLICATION_TOOL_TOOL_H

// STL includes
#include <string>

// Boost includes
#include <boost/signals2/signal.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

// Core includes
#include <Core/Utils/EnumClass.h>
#include <Core/Utils/Log.h>
#include <Core/Utils/Exception.h>
#include <Core/State/State.h>
#include <Core/State/StateEngine.h>

#include <Application/Layer/Layer.h>

namespace Seg3D
{

// CLASS TOOL:
// Base class of a tool

// Forward declaration
class Tool;
typedef boost::shared_ptr< Tool > ToolHandle;

// Tool groups help organize the tools in different categories
SCI_ENUM_CLASS
(
  ToolGroupType,
  TOOL_E = 0x0001,
  FILTER_E = 0x0002,
  DATATODATA_E = 0x0010,
  DATATOMASK_E = 0x0020,
  MASKTOMASK_E = 0x0040
)

// Class definition
class Tool : public Core::StateHandler
{

  // -- constructor/destructor --
public:
  Tool( const std::string& tool_type );
  virtual ~Tool();

  // -- query properties of tool --
public:
  virtual std::string type() const = 0;
  virtual std::string menu_name() const = 0;
  virtual std::string shortcut_key() const = 0;
  virtual int properties() const = 0;
  virtual std::string url() const = 0;

  inline std::string toolid() const 
  {
    return stateid();
  }

protected:
  friend class ToolFactory;

  // -- close tool --
public:
  // CLOSE:
  // This function is called when the application closes the tool. It is meant
  // to disconnect all connections. Since close tool is called synchronously
  // on the application thread, it can clean out most of the underlying
  // connections safely.
  // NOTE: since the tool handle is given to the user interface, the user
  // interface thread may issue the final destruction of the class. Hence all
  // thread critical pieces should be done by this function.
  virtual void close();

  // -- activate/deactivate --
public:
  // ACTIVATE:
  // Activate a tool: this tool is set as the active tool and hence it should
  // setup the right mouse tools in the viewers.
  virtual void activate();

  // DEACTIVATE:
  // Deactivate a tool. A tool is always deactivate before the next one is
  // activated.
  virtual void deactivate();

protected:

  void CreateLayerNameList( std::vector< std::string >& layer_names, Core::VolumeType type );
  void CreateLayerNameList( std::vector< std::string >& layer_names, 
    Core::VolumeType type, LayerGroupHandle layer_group );
  void CreateLayerNameList( std::vector< std::string >& layer_names,
    Core::VolumeType type, LayerGroupHandle layer_group, std::string exclude );

public:

  const static std::string NONE_OPTION_C;

};

// SCI_TOOL_TYPE:
// Tool type should be defined at the top of each action. It renders code that
// allows both the class as well as the Tool object to determine what its
// properties are. By defining class specific static functions the class 
// properties can be queried without instantiating the action. On the other
// hand you want to query these properties from the object as well, even when
// we only have a pointer to the base object. Hence we need virtual functions
// as well. 

#define SCI_TOOL_TYPE(type_string,menu_name_string,shortcut_key_string,properties_mask,help_url) \
  public: \
    static std::string Type() { return Core::StringToLower(type_string); } \
    static std::string MenuName() { return menu_name_string; } \
    static std::string ShortcutKey() { return shortcut_key_string; } \
    static int         Properties() { return properties_mask; } \
    static std::string Url() { return help_url; } \
    \
    virtual std::string type() const { return Type(); } \
    virtual std::string menu_name() const { return MenuName(); } \
    virtual std::string shortcut_key() const { return ShortcutKey(); } \
    virtual int         properties() const { return Properties(); } \
    virtual std::string url() const { return Url(); }

} // end namespace Seg3D

#endif
