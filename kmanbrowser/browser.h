/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__BROWSER_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __BROWSER_H__

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

#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL Browser {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_Browser)


struct URLRequest;
typedef TVarVar<URLRequest> URLRequest_var;
typedef URLRequest_var URLRequest_out;

struct URLRequest {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef URLRequest_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  URLRequest();
  ~URLRequest();
  URLRequest( const URLRequest& s );
  URLRequest& operator=( const URLRequest& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  CORBA::String_var url;
  CORBA::Boolean reload;
  CORBA::Long xOffset;
  CORBA::Long yOffset;
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_URLRequest;

struct Transfer;
typedef TVarVar<Transfer> Transfer_var;
typedef Transfer_var Transfer_out;

struct Transfer {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef Transfer_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  Transfer();
  ~Transfer();
  Transfer( const Transfer& s );
  Transfer& operator=( const Transfer& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  CORBA::String_var source;
  CORBA::String_var destination;
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_Transfer;

typedef Transfer EventNewTransfer;
#ifdef _WINDOWS
static EventNewTransfer _dummy_EventNewTransfer;
#endif
typedef Transfer_var EventNewTransfer_var;
typedef EventNewTransfer_var EventNewTransfer_out;

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_EventNewTransfer;

MICO_EXPORT_CONST_DECL const char* eventNewTransfer;
typedef URLRequest EventOpenURL;
#ifdef _WINDOWS
static EventOpenURL _dummy_EventOpenURL;
#endif
typedef URLRequest_var EventOpenURL_var;
typedef EventOpenURL_var EventOpenURL_out;

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_EventOpenURL;

MICO_EXPORT_CONST_DECL const char* eventOpenURL;
class View;
typedef View *View_ptr;
typedef View_ptr ViewRef;
typedef ObjVar<View> View_var;
typedef View_var View_out;


// Common definitions for interface View
class View : 
  virtual public ::OpenParts::Part
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

    static const CORBA::Long TOOLBAR_ITEM_ID_BEGIN;
    typedef ::OpenPartsUI::Menu EventFillMenu;
    typedef ::OpenPartsUI::Menu_ptr EventFillMenu_ptr;
    typedef ::OpenPartsUI::MenuRef EventFillMenuRef;
    typedef ::OpenPartsUI::Menu_var EventFillMenu_var;
    typedef EventFillMenu_var EventFillMenu_out;

    static CORBA::TypeCodeConst _tc_EventFillMenu;

    struct EventFillToolBar;
    typedef TVarVar<EventFillToolBar> EventFillToolBar_var;
    typedef EventFillToolBar_var EventFillToolBar_out;

    struct EventFillToolBar {
      #ifdef HAVE_TYPEDEF_OVERLOAD
      typedef EventFillToolBar_var _var_type;
      #endif
      #ifdef HAVE_EXPLICIT_STRUCT_OPS
      EventFillToolBar();
      ~EventFillToolBar();
      EventFillToolBar( const EventFillToolBar& s );
      EventFillToolBar& operator=( const EventFillToolBar& s );
      #endif //HAVE_EXPLICIT_STRUCT_OPS

      CORBA::Boolean create;
      CORBA::Long startIndex;
      OpenPartsUI::ToolBar_var toolBar;
    };

    static CORBA::TypeCodeConst _tc_EventFillToolBar;

    static const char* eventFillMenuEdit;
    static const char* eventFillMenuView;
    static const char* eventFillToolBar;
    virtual char* url() = 0;
    virtual CORBA::Long xOffset() = 0;
    virtual CORBA::Long yOffset() = 0;
    virtual void stop() = 0;
    virtual char* viewName() = 0;

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
  virtual public ::OpenParts::Part_stub
{
  public:
    virtual ~View_stub();
    char* url();
    CORBA::Long xOffset();
    CORBA::Long yOffset();
    void stop();
    char* viewName();

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

typedef IfaceSequenceTmpl<View_var,View_ptr> ViewList;
#ifdef _WINDOWS
static ViewList _dummy_ViewList;
#endif
typedef TSeqVar<IfaceSequenceTmpl<View_var,View_ptr> > ViewList_var;
typedef ViewList_var ViewList_out;

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_ViewList;

class PrintingExtension;
typedef PrintingExtension *PrintingExtension_ptr;
typedef PrintingExtension_ptr PrintingExtensionRef;
typedef ObjVar<PrintingExtension> PrintingExtension_var;
typedef PrintingExtension_var PrintingExtension_out;


// Common definitions for interface PrintingExtension
class PrintingExtension : 
  virtual public CORBA::Object
{
  public:
    virtual ~PrintingExtension();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef PrintingExtension_ptr _ptr_type;
    typedef PrintingExtension_var _var_type;
    #endif

    static PrintingExtension_ptr _narrow( CORBA::Object_ptr obj );
    static PrintingExtension_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static PrintingExtension_ptr _duplicate( PrintingExtension_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static PrintingExtension_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void print() = 0;

  protected:
    PrintingExtension() {};
  private:
    PrintingExtension( const PrintingExtension& );
    void operator=( const PrintingExtension& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_PrintingExtension;

// Stub for interface PrintingExtension
class PrintingExtension_stub:
  virtual public PrintingExtension
{
  public:
    virtual ~PrintingExtension_stub();
    void print();

  private:
    void operator=( const PrintingExtension_stub& );
};

class PrintingExtension_skel :
  virtual public StaticMethodDispatcher,
  virtual public PrintingExtension
{
  public:
    PrintingExtension_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~PrintingExtension_skel();
    PrintingExtension_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    PrintingExtension_ptr _this();

};

class MagnifyingExtension;
typedef MagnifyingExtension *MagnifyingExtension_ptr;
typedef MagnifyingExtension_ptr MagnifyingExtensionRef;
typedef ObjVar<MagnifyingExtension> MagnifyingExtension_var;
typedef MagnifyingExtension_var MagnifyingExtension_out;


// Common definitions for interface MagnifyingExtension
class MagnifyingExtension : 
  virtual public CORBA::Object
{
  public:
    virtual ~MagnifyingExtension();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef MagnifyingExtension_ptr _ptr_type;
    typedef MagnifyingExtension_var _var_type;
    #endif

    static MagnifyingExtension_ptr _narrow( CORBA::Object_ptr obj );
    static MagnifyingExtension_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static MagnifyingExtension_ptr _duplicate( MagnifyingExtension_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static MagnifyingExtension_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;
    virtual CORBA::Boolean canZoomIn() = 0;
    virtual CORBA::Boolean canZoomOut() = 0;

  protected:
    MagnifyingExtension() {};
  private:
    MagnifyingExtension( const MagnifyingExtension& );
    void operator=( const MagnifyingExtension& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_MagnifyingExtension;

// Stub for interface MagnifyingExtension
class MagnifyingExtension_stub:
  virtual public MagnifyingExtension
{
  public:
    virtual ~MagnifyingExtension_stub();
    void zoomIn();
    void zoomOut();
    CORBA::Boolean canZoomIn();
    CORBA::Boolean canZoomOut();

  private:
    void operator=( const MagnifyingExtension_stub& );
};

class MagnifyingExtension_skel :
  virtual public StaticMethodDispatcher,
  virtual public MagnifyingExtension
{
  public:
    MagnifyingExtension_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~MagnifyingExtension_skel();
    MagnifyingExtension_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    MagnifyingExtension_ptr _this();

};

class ViewFactory;
typedef ViewFactory *ViewFactory_ptr;
typedef ViewFactory_ptr ViewFactoryRef;
typedef ObjVar<ViewFactory> ViewFactory_var;
typedef ViewFactory_var ViewFactory_out;


// Common definitions for interface ViewFactory
class ViewFactory : 
  virtual public CORBA::Object
{
  public:
    virtual ~ViewFactory();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef ViewFactory_ptr _ptr_type;
    typedef ViewFactory_var _var_type;
    #endif

    static ViewFactory_ptr _narrow( CORBA::Object_ptr obj );
    static ViewFactory_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static ViewFactory_ptr _duplicate( ViewFactory_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static ViewFactory_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual View_ptr create() = 0;

  protected:
    ViewFactory() {};
  private:
    ViewFactory( const ViewFactory& );
    void operator=( const ViewFactory& );
};

MICO_EXPORT_TYPEVAR_DECL CORBA::TypeCodeConst _tc_ViewFactory;

// Stub for interface ViewFactory
class ViewFactory_stub:
  virtual public ViewFactory
{
  public:
    virtual ~ViewFactory_stub();
    View_ptr create();

  private:
    void operator=( const ViewFactory_stub& );
};

class ViewFactory_skel :
  virtual public StaticMethodDispatcher,
  virtual public ViewFactory
{
  public:
    ViewFactory_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~ViewFactory_skel();
    ViewFactory_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment &_env );
    ViewFactory_ptr _this();

};

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_Browser)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif



#ifndef MICO_CONF_NO_POA

#endif // MICO_CONF_NO_POA

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

void operator<<=( CORBA::Any &_a, const ::Browser::URLRequest &_s );
void operator<<=( CORBA::Any &_a, ::Browser::URLRequest *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Browser::URLRequest &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Browser::URLRequest *&_s );

extern CORBA::StaticTypeInfo *_marshaller_Browser_URLRequest;

void operator<<=( CORBA::Any &_a, const ::Browser::Transfer &_s );
void operator<<=( CORBA::Any &_a, ::Browser::Transfer *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Browser::Transfer &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Browser::Transfer *&_s );

extern CORBA::StaticTypeInfo *_marshaller_Browser_Transfer;

void operator<<=( CORBA::Any &_a, const ::Browser::View::EventFillToolBar &_s );
void operator<<=( CORBA::Any &_a, ::Browser::View::EventFillToolBar *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Browser::View::EventFillToolBar &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Browser::View::EventFillToolBar *&_s );

extern CORBA::StaticTypeInfo *_marshaller_Browser_View_EventFillToolBar;

void operator<<=( CORBA::Any &a, const Browser::View_ptr obj );
void operator<<=( CORBA::Any &a, Browser::View_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, Browser::View_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_Browser_View;

void operator<<=( CORBA::Any &a, const Browser::PrintingExtension_ptr obj );
void operator<<=( CORBA::Any &a, Browser::PrintingExtension_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, Browser::PrintingExtension_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_Browser_PrintingExtension;

void operator<<=( CORBA::Any &a, const Browser::MagnifyingExtension_ptr obj );
void operator<<=( CORBA::Any &a, Browser::MagnifyingExtension_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, Browser::MagnifyingExtension_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_Browser_MagnifyingExtension;

void operator<<=( CORBA::Any &a, const Browser::ViewFactory_ptr obj );
void operator<<=( CORBA::Any &a, Browser::ViewFactory_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, Browser::ViewFactory_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_Browser_ViewFactory;

void operator<<=( CORBA::Any &_a, const IfaceSequenceTmpl<Browser::View_var,Browser::View_ptr> &_s );
void operator<<=( CORBA::Any &_a, IfaceSequenceTmpl<Browser::View_var,Browser::View_ptr> *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, IfaceSequenceTmpl<Browser::View_var,Browser::View_ptr> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, IfaceSequenceTmpl<Browser::View_var,Browser::View_ptr> *&_s );

extern CORBA::StaticTypeInfo *_marshaller__seq_Browser_View;

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#if !defined(MICO_NO_TOPLEVEL_MODULES) && !defined(MICO_IN_GENERATED_CODE)
#include <mico/template_impl.h>
#endif

#endif
