/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__KMANBROWSER_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __KMANBROWSER_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <kom.h>
#else
#define MICO_IN_GENERATED_CODE
#include <kom.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <openparts_ui.h>
#else
#define MICO_IN_GENERATED_CODE
#include <openparts_ui.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <openparts.h>
#else
#define MICO_IN_GENERATED_CODE
#include <openparts.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <browser.h>
#else
#define MICO_IN_GENERATED_CODE
#include <browser.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL KManBrowser {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KManBrowser)


class MainWindow;
typedef MainWindow *MainWindow_ptr;
typedef MainWindow_ptr MainWindowRef;
typedef ObjVar<MainWindow> MainWindow_var;
typedef MainWindow_var MainWindow_out;


// Common definitions for interface MainWindow
class MainWindow : 
  virtual public ::OpenParts::MainWindow
{
  public:
    virtual ~MainWindow();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef MainWindow_ptr _ptr_type;
    typedef MainWindow_var _var_type;
    #endif

    static MainWindow_ptr _narrow( CORBA::Object_ptr obj );
    static MainWindow_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static MainWindow_ptr _duplicate( MainWindow_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static MainWindow_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void open( const char* url, CORBA::Boolean reload, CORBA::Long xoffset, CORBA::Long yoffset ) = 0;
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;
    virtual void print() = 0;
    virtual void reload() = 0;
    virtual void openFile() = 0;
    virtual void index() = 0;
    virtual void forward() = 0;
    virtual void back() = 0;
    virtual void bookmark() = 0;
    virtual void options() = 0;
    virtual void openURL( const Browser::URLRequest& url ) = 0;
    virtual void slotURLStarted( OpenParts::Id id, const char* url ) = 0;
    virtual void slotURLCompleted( OpenParts::Id id ) = 0;
    virtual void setStatusBarText( const CORBA::WChar* text ) = 0;
    virtual void setLocationBarURL( OpenParts::Id id, const char* url ) = 0;
    virtual void createNewWindow( const char* url ) = 0;

  protected:
    MainWindow() {};
  private:
    MainWindow( const MainWindow& );
    void operator=( const MainWindow& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_MainWindow;

// Stub for interface MainWindow
class MainWindow_stub:
  virtual public MainWindow,
  virtual public ::OpenParts::MainWindow_stub
{
  public:
    virtual ~MainWindow_stub();
    void open( const char* url, CORBA::Boolean reload, CORBA::Long xoffset, CORBA::Long yoffset );
    void zoomIn();
    void zoomOut();
    void print();
    void reload();
    void openFile();
    void index();
    void forward();
    void back();
    void bookmark();
    void options();
    void openURL( const Browser::URLRequest& url );
    void slotURLStarted( OpenParts::Id id, const char* url );
    void slotURLCompleted( OpenParts::Id id );
    void setStatusBarText( const CORBA::WChar* text );
    void setLocationBarURL( OpenParts::Id id, const char* url );
    void createNewWindow( const char* url );

  private:
    void operator=( const MainWindow_stub& );
};

class MainWindow_skel :
  virtual public StaticMethodDispatcher,
  virtual public MainWindow
{
  public:
    MainWindow_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~MainWindow_skel();
    MainWindow_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    MainWindow_ptr _this();

};

class View;
typedef View *View_ptr;
typedef View_ptr ViewRef;
typedef ObjVar<View> View_var;
typedef View_var View_out;


// Common definitions for interface View
class View : 
  virtual public ::Browser::View,
  virtual public ::Browser::PrintingExtension,
  virtual public ::Browser::MagnifyingExtension
{
  public:
    virtual ~View();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef View_ptr _ptr_type;
    typedef View_var _var_type;
    #endif

    static View_ptr _narrow( CORBA::Object_ptr obj );
    static View_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static View_ptr _duplicate( View_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static View_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

  protected:
    View() {};
  private:
    View( const View& );
    void operator=( const View& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_View;

// Stub for interface View
class View_stub:
  virtual public View,
  virtual public ::Browser::View_stub,
  virtual public ::Browser::PrintingExtension_stub,
  virtual public ::Browser::MagnifyingExtension_stub
{
  public:
    virtual ~View_stub();
  private:
    void operator=( const View_stub& );
};

class View_skel :
  virtual public StaticMethodDispatcher,
  virtual public View
{
  public:
    View_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~View_skel();
    View_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    View_ptr _this();

};

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_KManBrowser)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif



#ifndef MICO_CONF_NO_POA

#endif // MICO_CONF_NO_POA

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

void operator<<=( CORBA::Any &a, const KManBrowser::MainWindow_ptr obj );
void operator<<=( CORBA::Any &a, KManBrowser::MainWindow_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KManBrowser::MainWindow_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KManBrowser_MainWindow;

void operator<<=( CORBA::Any &a, const KManBrowser::View_ptr obj );
void operator<<=( CORBA::Any &a, KManBrowser::View_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, KManBrowser::View_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_KManBrowser_View;

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#if !defined(MICO_NO_TOPLEVEL_MODULES) && !defined(MICO_IN_GENERATED_CODE)
#include <mico/template_impl.h>
#endif

#endif
