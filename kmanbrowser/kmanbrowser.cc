/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <kmanbrowser.h>

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface MainWindow
KManBrowser::MainWindow::~MainWindow()
{
}

void *KManBrowser::MainWindow::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KManBrowser/MainWindow:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = OpenParts::MainWindow::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KManBrowser::MainWindow::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KManBrowser/MainWindow:1.0" ) == 0) {
    return true;
  }
  for( vector<CORBA::Narrow_proto>::size_type _i = 0;
       _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

KManBrowser::MainWindow_ptr KManBrowser::MainWindow::_narrow( CORBA::Object_ptr _obj )
{
  KManBrowser::MainWindow_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KManBrowser/MainWindow:1.0" )))
      return _duplicate( (KManBrowser::MainWindow_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KManBrowser/MainWindow:1.0" ) ) ) {
      _o = new KManBrowser::MainWindow_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KManBrowser::MainWindow_ptr
KManBrowser::MainWindow::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KManBrowser::MainWindow_stub::~MainWindow_stub()
{
}

void KManBrowser::MainWindow_stub::open( const char* url, CORBA::Boolean reload, CORBA::Long xoffset, CORBA::Long yoffset )
{
  CORBA::StaticAny _url( CORBA::_stc_string, &url );
  CORBA::StaticAny _reload( CORBA::_stc_boolean, &reload );
  CORBA::StaticAny _xoffset( CORBA::_stc_long, &xoffset );
  CORBA::StaticAny _yoffset( CORBA::_stc_long, &yoffset );
  CORBA::StaticRequest __req( this, "open" );
  __req.add_in_arg( &_url );
  __req.add_in_arg( &_reload );
  __req.add_in_arg( &_xoffset );
  __req.add_in_arg( &_yoffset );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::zoomIn()
{
  CORBA::StaticRequest __req( this, "zoomIn" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::zoomOut()
{
  CORBA::StaticRequest __req( this, "zoomOut" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::print()
{
  CORBA::StaticRequest __req( this, "print" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::reload()
{
  CORBA::StaticRequest __req( this, "reload" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::openFile()
{
  CORBA::StaticRequest __req( this, "openFile" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::index()
{
  CORBA::StaticRequest __req( this, "index" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::forward()
{
  CORBA::StaticRequest __req( this, "forward" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::back()
{
  CORBA::StaticRequest __req( this, "back" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::bookmark()
{
  CORBA::StaticRequest __req( this, "bookmark" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::options()
{
  CORBA::StaticRequest __req( this, "options" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::openURL( const Browser::URLRequest& url )
{
  CORBA::StaticAny _url( _marshaller_Browser_URLRequest, &url );
  CORBA::StaticRequest __req( this, "openURL" );
  __req.add_in_arg( &_url );

  __req.oneway();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::slotURLStarted( OpenParts::Id id, const char* url )
{
  CORBA::StaticAny _id( CORBA::_stc_ulong, &id );
  CORBA::StaticAny _url( CORBA::_stc_string, &url );
  CORBA::StaticRequest __req( this, "slotURLStarted" );
  __req.add_in_arg( &_id );
  __req.add_in_arg( &_url );

  __req.oneway();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::slotURLCompleted( OpenParts::Id id )
{
  CORBA::StaticAny _id( CORBA::_stc_ulong, &id );
  CORBA::StaticRequest __req( this, "slotURLCompleted" );
  __req.add_in_arg( &_id );

  __req.oneway();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::setStatusBarText( const CORBA::WChar* text )
{
  CORBA::StaticAny _text( CORBA::_stc_wstring, &text );
  CORBA::StaticRequest __req( this, "setStatusBarText" );
  __req.add_in_arg( &_text );

  __req.oneway();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::setLocationBarURL( OpenParts::Id id, const char* url )
{
  CORBA::StaticAny _id( CORBA::_stc_ulong, &id );
  CORBA::StaticAny _url( CORBA::_stc_string, &url );
  CORBA::StaticRequest __req( this, "setLocationBarURL" );
  __req.add_in_arg( &_id );
  __req.add_in_arg( &_url );

  __req.oneway();

  mico_sii_throw( &__req, 
    0);
}


void KManBrowser::MainWindow_stub::createNewWindow( const char* url )
{
  CORBA::StaticAny _url( CORBA::_stc_string, &url );
  CORBA::StaticRequest __req( this, "createNewWindow" );
  __req.add_in_arg( &_url );

  __req.oneway();

  mico_sii_throw( &__req, 
    0);
}


struct _global_init_KManBrowser_MainWindow {
  _global_init_KManBrowser_MainWindow()
  {
    if( ::OpenParts::MainWindow::_narrow_helpers == NULL )
      ::OpenParts::MainWindow::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OpenParts::MainWindow::_narrow_helpers->push_back( KManBrowser::MainWindow::_narrow_helper2 );
  }
} __global_init_KManBrowser_MainWindow;

#ifdef HAVE_NAMESPACE
namespace KManBrowser { vector<CORBA::Narrow_proto> * MainWindow::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KManBrowser::MainWindow::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KManBrowser { CORBA::TypeCodeConst _tc_MainWindow; };
#else
CORBA::TypeCodeConst KManBrowser::_tc_MainWindow;
#endif

void
operator<<=( CORBA::Any &_a, const KManBrowser::MainWindow_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KManBrowser_MainWindow, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, KManBrowser::MainWindow_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_KManBrowser_MainWindow, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KManBrowser::MainWindow_ptr &_obj )
{
  KManBrowser::MainWindow_ptr *p;
  if (_a.to_static_any (_marshaller_KManBrowser_MainWindow, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}

class _Marshaller_KManBrowser_MainWindow : public CORBA::StaticTypeInfo {
    typedef KManBrowser::MainWindow_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KManBrowser_MainWindow::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KManBrowser_MainWindow::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KManBrowser::MainWindow::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KManBrowser_MainWindow::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KManBrowser_MainWindow::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KManBrowser::MainWindow::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_KManBrowser_MainWindow::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KManBrowser_MainWindow::typecode()
{
  return KManBrowser::_tc_MainWindow;
}

CORBA::StaticTypeInfo *_marshaller_KManBrowser_MainWindow;


// Stub interface View
KManBrowser::View::~View()
{
}

void *KManBrowser::View::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:KManBrowser/View:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = Browser::View::_narrow_helper( _repoid )))
      return _p;
  }
  {
    void *_p;
    if( (_p = Browser::PrintingExtension::_narrow_helper( _repoid )))
      return _p;
  }
  {
    void *_p;
    if( (_p = Browser::MagnifyingExtension::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool KManBrowser::View::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:KManBrowser/View:1.0" ) == 0) {
    return true;
  }
  for( vector<CORBA::Narrow_proto>::size_type _i = 0;
       _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

KManBrowser::View_ptr KManBrowser::View::_narrow( CORBA::Object_ptr _obj )
{
  KManBrowser::View_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:KManBrowser/View:1.0" )))
      return _duplicate( (KManBrowser::View_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:KManBrowser/View:1.0" ) ) ) {
      _o = new KManBrowser::View_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

KManBrowser::View_ptr
KManBrowser::View::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

KManBrowser::View_stub::~View_stub()
{
}

struct _global_init_KManBrowser_View {
  _global_init_KManBrowser_View()
  {
    if( ::Browser::View::_narrow_helpers == NULL )
      ::Browser::View::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::Browser::View::_narrow_helpers->push_back( KManBrowser::View::_narrow_helper2 );
    if( ::Browser::PrintingExtension::_narrow_helpers == NULL )
      ::Browser::PrintingExtension::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::Browser::PrintingExtension::_narrow_helpers->push_back( KManBrowser::View::_narrow_helper2 );
    if( ::Browser::MagnifyingExtension::_narrow_helpers == NULL )
      ::Browser::MagnifyingExtension::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::Browser::MagnifyingExtension::_narrow_helpers->push_back( KManBrowser::View::_narrow_helper2 );
  }
} __global_init_KManBrowser_View;

#ifdef HAVE_NAMESPACE
namespace KManBrowser { vector<CORBA::Narrow_proto> * View::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * KManBrowser::View::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace KManBrowser { CORBA::TypeCodeConst _tc_View; };
#else
CORBA::TypeCodeConst KManBrowser::_tc_View;
#endif

void
operator<<=( CORBA::Any &_a, const KManBrowser::View_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_KManBrowser_View, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, KManBrowser::View_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_KManBrowser_View, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, KManBrowser::View_ptr &_obj )
{
  KManBrowser::View_ptr *p;
  if (_a.to_static_any (_marshaller_KManBrowser_View, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}

class _Marshaller_KManBrowser_View : public CORBA::StaticTypeInfo {
    typedef KManBrowser::View_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_KManBrowser_View::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_KManBrowser_View::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::KManBrowser::View::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_KManBrowser_View::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_KManBrowser_View::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::KManBrowser::View::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_KManBrowser_View::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_KManBrowser_View::typecode()
{
  return KManBrowser::_tc_View;
}

CORBA::StaticTypeInfo *_marshaller_KManBrowser_View;

struct __tc_init_KMANBROWSER {
  __tc_init_KMANBROWSER()
  {
    KManBrowser::_tc_MainWindow = 
    "010000000e00000037000000010000001f00000049444c3a4b4d616e4272"
    "6f777365722f4d61696e57696e646f773a312e3000000b0000004d61696e"
    "57696e646f7700";
    _marshaller_KManBrowser_MainWindow = new _Marshaller_KManBrowser_MainWindow;
    KManBrowser::_tc_View = 
    "010000000e0000002d000000010000001900000049444c3a4b4d616e4272"
    "6f777365722f566965773a312e3000000000050000005669657700";
    _marshaller_KManBrowser_View = new _Marshaller_KManBrowser_View;
  }
};

static __tc_init_KMANBROWSER __init_KMANBROWSER;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

KManBrowser::MainWindow_skel::MainWindow_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KManBrowser/MainWindow:1.0", "MainWindow" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KManBrowser/MainWindow:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<MainWindow_skel>( this ) );
}

KManBrowser::MainWindow_skel::MainWindow_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KManBrowser/MainWindow:1.0", "MainWindow" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<MainWindow_skel>( this ) );
}

KManBrowser::MainWindow_skel::~MainWindow_skel()
{
}

bool KManBrowser::MainWindow_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    extern CORBA::ULong mico_string_hash (const char *, CORBA::ULong);
    switch (mico_string_hash (_req->op_name(), 29)) {
    case 0:
      if( strcmp( _req->op_name(), "open" ) == 0 ) {
        CORBA::String_var url;
        CORBA::StaticAny _url( CORBA::_stc_string, &url.inout() );
        CORBA::Boolean reload;
        CORBA::StaticAny _reload( CORBA::_stc_boolean, &reload );
        CORBA::Long xoffset;
        CORBA::StaticAny _xoffset( CORBA::_stc_long, &xoffset );
        CORBA::Long yoffset;
        CORBA::StaticAny _yoffset( CORBA::_stc_long, &yoffset );

        _req->add_in_arg( &_url );
        _req->add_in_arg( &_reload );
        _req->add_in_arg( &_xoffset );
        _req->add_in_arg( &_yoffset );

        if( !_req->read_args() )
          return true;

        open( url, reload, xoffset, yoffset );
        _req->write_results();
        return true;
      }
      break;
    case 2:
      if( strcmp( _req->op_name(), "slotURLStarted" ) == 0 ) {
        OpenParts::Id id;
        CORBA::StaticAny _id( CORBA::_stc_ulong, &id );
        CORBA::String_var url;
        CORBA::StaticAny _url( CORBA::_stc_string, &url.inout() );

        _req->add_in_arg( &_id );
        _req->add_in_arg( &_url );

        if( !_req->read_args() )
          return true;

        slotURLStarted( id, url );
        _req->write_results();
        return true;
      }
      break;
    case 3:
      if( strcmp( _req->op_name(), "openFile" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        openFile();
        _req->write_results();
        return true;
      }
      if( strcmp( _req->op_name(), "forward" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        forward();
        _req->write_results();
        return true;
      }
      break;
    case 6:
      if( strcmp( _req->op_name(), "openURL" ) == 0 ) {
        Browser::URLRequest url;
        CORBA::StaticAny _url( _marshaller_Browser_URLRequest, &url );

        _req->add_in_arg( &_url );

        if( !_req->read_args() )
          return true;

        openURL( url );
        _req->write_results();
        return true;
      }
      break;
    case 7:
      if( strcmp( _req->op_name(), "back" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        back();
        _req->write_results();
        return true;
      }
      break;
    case 9:
      if( strcmp( _req->op_name(), "setLocationBarURL" ) == 0 ) {
        OpenParts::Id id;
        CORBA::StaticAny _id( CORBA::_stc_ulong, &id );
        CORBA::String_var url;
        CORBA::StaticAny _url( CORBA::_stc_string, &url.inout() );

        _req->add_in_arg( &_id );
        _req->add_in_arg( &_url );

        if( !_req->read_args() )
          return true;

        setLocationBarURL( id, url );
        _req->write_results();
        return true;
      }
      break;
    case 10:
      if( strcmp( _req->op_name(), "options" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        options();
        _req->write_results();
        return true;
      }
      break;
    case 11:
      if( strcmp( _req->op_name(), "reload" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        reload();
        _req->write_results();
        return true;
      }
      break;
    case 12:
      if( strcmp( _req->op_name(), "zoomOut" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        zoomOut();
        _req->write_results();
        return true;
      }
      if( strcmp( _req->op_name(), "setStatusBarText" ) == 0 ) {
        CORBA::WString_var text;
        CORBA::StaticAny _text( CORBA::_stc_wstring, &text.inout() );

        _req->add_in_arg( &_text );

        if( !_req->read_args() )
          return true;

        setStatusBarText( text );
        _req->write_results();
        return true;
      }
      break;
    case 15:
      if( strcmp( _req->op_name(), "zoomIn" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        zoomIn();
        _req->write_results();
        return true;
      }
      break;
    case 16:
      if( strcmp( _req->op_name(), "createNewWindow" ) == 0 ) {
        CORBA::String_var url;
        CORBA::StaticAny _url( CORBA::_stc_string, &url.inout() );

        _req->add_in_arg( &_url );

        if( !_req->read_args() )
          return true;

        createNewWindow( url );
        _req->write_results();
        return true;
      }
      break;
    case 19:
      if( strcmp( _req->op_name(), "print" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        print();
        _req->write_results();
        return true;
      }
      break;
    case 20:
      if( strcmp( _req->op_name(), "index" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        index();
        _req->write_results();
        return true;
      }
      break;
    case 25:
      if( strcmp( _req->op_name(), "bookmark" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        bookmark();
        _req->write_results();
        return true;
      }
      break;
    case 28:
      if( strcmp( _req->op_name(), "slotURLCompleted" ) == 0 ) {
        OpenParts::Id id;
        CORBA::StaticAny _id( CORBA::_stc_ulong, &id );

        _req->add_in_arg( &_id );

        if( !_req->read_args() )
          return true;

        slotURLCompleted( id );
        _req->write_results();
        return true;
      }
      break;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_catch &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

KManBrowser::MainWindow_ptr KManBrowser::MainWindow_skel::_this()
{
  return KManBrowser::MainWindow::_duplicate( this );
}


KManBrowser::View_skel::View_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KManBrowser/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:KManBrowser/View:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<View_skel>( this ) );
}

KManBrowser::View_skel::View_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:KManBrowser/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<View_skel>( this ) );
}

KManBrowser::View_skel::~View_skel()
{
}

bool KManBrowser::View_skel::dispatch( CORBA::StaticServerRequest_ptr /*_req*/, CORBA::Environment & /*_env*/ )
{
  return false;
}

KManBrowser::View_ptr KManBrowser::View_skel::_this()
{
  return KManBrowser::View::_duplicate( this );
}

