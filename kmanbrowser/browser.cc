/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <browser.h>

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------
#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst _tc_URLRequest; };
#else
CORBA::TypeCodeConst Browser::_tc_URLRequest;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
Browser::URLRequest::URLRequest()
{
}

Browser::URLRequest::URLRequest( const URLRequest& _s )
{
  url = ((URLRequest&)_s).url;
  reload = ((URLRequest&)_s).reload;
  xOffset = ((URLRequest&)_s).xOffset;
  yOffset = ((URLRequest&)_s).yOffset;
}

Browser::URLRequest::~URLRequest()
{
}

Browser::URLRequest&
Browser::URLRequest::operator=( const URLRequest& _s )
{
  url = ((URLRequest&)_s).url;
  reload = ((URLRequest&)_s).reload;
  xOffset = ((URLRequest&)_s).xOffset;
  yOffset = ((URLRequest&)_s).yOffset;
  return *this;
}
#endif

void operator<<=( CORBA::Any &_a, const Browser::URLRequest &_s )
{
  CORBA::StaticAny _sa (_marshaller_Browser_URLRequest, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, Browser::URLRequest *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Browser::URLRequest &_s )
{
  CORBA::StaticAny _sa (_marshaller_Browser_URLRequest, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Browser::URLRequest *&_s )
{
  return _a.to_static_any (_marshaller_Browser_URLRequest, (void *&)_s);
}

class _Marshaller_Browser_URLRequest : public CORBA::StaticTypeInfo {
    typedef Browser::URLRequest _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Browser_URLRequest::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_Browser_URLRequest::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_Browser_URLRequest::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Browser_URLRequest::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->url.inout() ) &&
    CORBA::_stc_boolean->demarshal( dc, &((_MICO_T*)v)->reload ) &&
    CORBA::_stc_long->demarshal( dc, &((_MICO_T*)v)->xOffset ) &&
    CORBA::_stc_long->demarshal( dc, &((_MICO_T*)v)->yOffset ) &&
    dc.struct_end();
}

void _Marshaller_Browser_URLRequest::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->url.inout() );
  CORBA::_stc_boolean->marshal( ec, &((_MICO_T*)v)->reload );
  CORBA::_stc_long->marshal( ec, &((_MICO_T*)v)->xOffset );
  CORBA::_stc_long->marshal( ec, &((_MICO_T*)v)->yOffset );
  ec.struct_end();
}

CORBA::TypeCode_ptr _Marshaller_Browser_URLRequest::typecode()
{
  return Browser::_tc_URLRequest;
}

CORBA::StaticTypeInfo *_marshaller_Browser_URLRequest;

#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst _tc_Transfer; };
#else
CORBA::TypeCodeConst Browser::_tc_Transfer;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
Browser::Transfer::Transfer()
{
}

Browser::Transfer::Transfer( const Transfer& _s )
{
  source = ((Transfer&)_s).source;
  destination = ((Transfer&)_s).destination;
}

Browser::Transfer::~Transfer()
{
}

Browser::Transfer&
Browser::Transfer::operator=( const Transfer& _s )
{
  source = ((Transfer&)_s).source;
  destination = ((Transfer&)_s).destination;
  return *this;
}
#endif

void operator<<=( CORBA::Any &_a, const Browser::Transfer &_s )
{
  CORBA::StaticAny _sa (_marshaller_Browser_Transfer, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, Browser::Transfer *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Browser::Transfer &_s )
{
  CORBA::StaticAny _sa (_marshaller_Browser_Transfer, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Browser::Transfer *&_s )
{
  return _a.to_static_any (_marshaller_Browser_Transfer, (void *&)_s);
}

class _Marshaller_Browser_Transfer : public CORBA::StaticTypeInfo {
    typedef Browser::Transfer _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Browser_Transfer::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_Browser_Transfer::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_Browser_Transfer::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Browser_Transfer::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->source.inout() ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->destination.inout() ) &&
    dc.struct_end();
}

void _Marshaller_Browser_Transfer::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->source.inout() );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->destination.inout() );
  ec.struct_end();
}

CORBA::TypeCode_ptr _Marshaller_Browser_Transfer::typecode()
{
  return Browser::_tc_Transfer;
}

CORBA::StaticTypeInfo *_marshaller_Browser_Transfer;

#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst _tc_EventNewTransfer; };
#else
CORBA::TypeCodeConst Browser::_tc_EventNewTransfer;
#endif

#ifdef HAVE_NAMESPACE
namespace Browser { const char* eventNewTransfer = "Browser/NewTransfer"; };
#else
const char* Browser::eventNewTransfer = "Browser/NewTransfer";
#endif
#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst _tc_EventOpenURL; };
#else
CORBA::TypeCodeConst Browser::_tc_EventOpenURL;
#endif

#ifdef HAVE_NAMESPACE
namespace Browser { const char* eventOpenURL = "Browser/OpenURL"; };
#else
const char* Browser::eventOpenURL = "Browser/OpenURL";
#endif
#ifdef HAVE_NAMESPACE
namespace Browser { const CORBA::Long View::TOOLBAR_ITEM_ID_BEGIN = 1000; };
#else
const CORBA::Long Browser::View::TOOLBAR_ITEM_ID_BEGIN = 1000;
#endif
#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst View::_tc_EventFillMenu; };
#else
CORBA::TypeCodeConst Browser::View::_tc_EventFillMenu;
#endif

#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst View::_tc_EventFillToolBar; };
#else
CORBA::TypeCodeConst Browser::View::_tc_EventFillToolBar;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
Browser::View::EventFillToolBar::EventFillToolBar()
{
}

Browser::View::EventFillToolBar::EventFillToolBar( const EventFillToolBar& _s )
{
  create = ((EventFillToolBar&)_s).create;
  startIndex = ((EventFillToolBar&)_s).startIndex;
  toolBar = ((EventFillToolBar&)_s).toolBar;
}

Browser::View::EventFillToolBar::~EventFillToolBar()
{
}

Browser::View::EventFillToolBar&
Browser::View::EventFillToolBar::operator=( const EventFillToolBar& _s )
{
  create = ((EventFillToolBar&)_s).create;
  startIndex = ((EventFillToolBar&)_s).startIndex;
  toolBar = ((EventFillToolBar&)_s).toolBar;
  return *this;
}
#endif

void operator<<=( CORBA::Any &_a, const Browser::View::EventFillToolBar &_s )
{
  CORBA::StaticAny _sa (_marshaller_Browser_View_EventFillToolBar, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, Browser::View::EventFillToolBar *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Browser::View::EventFillToolBar &_s )
{
  CORBA::StaticAny _sa (_marshaller_Browser_View_EventFillToolBar, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Browser::View::EventFillToolBar *&_s )
{
  return _a.to_static_any (_marshaller_Browser_View_EventFillToolBar, (void *&)_s);
}

class _Marshaller_Browser_View_EventFillToolBar : public CORBA::StaticTypeInfo {
    typedef Browser::View::EventFillToolBar _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Browser_View_EventFillToolBar::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_Browser_View_EventFillToolBar::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_Browser_View_EventFillToolBar::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Browser_View_EventFillToolBar::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_boolean->demarshal( dc, &((_MICO_T*)v)->create ) &&
    CORBA::_stc_long->demarshal( dc, &((_MICO_T*)v)->startIndex ) &&
    _marshaller_OpenPartsUI_ToolBar->demarshal( dc, &((_MICO_T*)v)->toolBar.inout() ) &&
    dc.struct_end();
}

void _Marshaller_Browser_View_EventFillToolBar::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_boolean->marshal( ec, &((_MICO_T*)v)->create );
  CORBA::_stc_long->marshal( ec, &((_MICO_T*)v)->startIndex );
  _marshaller_OpenPartsUI_ToolBar->marshal( ec, &((_MICO_T*)v)->toolBar.inout() );
  ec.struct_end();
}

CORBA::TypeCode_ptr _Marshaller_Browser_View_EventFillToolBar::typecode()
{
  return Browser::View::_tc_EventFillToolBar;
}

CORBA::StaticTypeInfo *_marshaller_Browser_View_EventFillToolBar;

#ifdef HAVE_NAMESPACE
namespace Browser { const char* View::eventFillMenuEdit = "Browser/GUI/fillMenu/Edit"; };
#else
const char* Browser::View::eventFillMenuEdit = "Browser/GUI/fillMenu/Edit";
#endif
#ifdef HAVE_NAMESPACE
namespace Browser { const char* View::eventFillMenuView = "Browser/GUI/fillMenu/View"; };
#else
const char* Browser::View::eventFillMenuView = "Browser/GUI/fillMenu/View";
#endif
#ifdef HAVE_NAMESPACE
namespace Browser { const char* View::eventFillToolBar = "Browser/GUI/fillToolBar"; };
#else
const char* Browser::View::eventFillToolBar = "Browser/GUI/fillToolBar";
#endif

// Stub interface View
Browser::View::~View()
{
}

void *Browser::View::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:Browser/View:1.0" ) == 0 )
    return (void *)this;
  {
    void *_p;
    if( (_p = OpenParts::Part::_narrow_helper( _repoid )))
      return _p;
  }
  return NULL;
}

bool Browser::View::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:Browser/View:1.0" ) == 0) {
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

Browser::View_ptr Browser::View::_narrow( CORBA::Object_ptr _obj )
{
  Browser::View_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:Browser/View:1.0" )))
      return _duplicate( (Browser::View_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:Browser/View:1.0" ) ) ) {
      _o = new Browser::View_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

Browser::View_ptr
Browser::View::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

Browser::View_stub::~View_stub()
{
}

char* Browser::View_stub::url()
{
  char* _res;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "url" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Long Browser::View_stub::xOffset()
{
  CORBA::Long _res;
  CORBA::StaticAny __res( CORBA::_stc_long, &_res );

  CORBA::StaticRequest __req( this, "xOffset" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Long Browser::View_stub::yOffset()
{
  CORBA::Long _res;
  CORBA::StaticAny __res( CORBA::_stc_long, &_res );

  CORBA::StaticRequest __req( this, "yOffset" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


void Browser::View_stub::stop()
{
  CORBA::StaticRequest __req( this, "stop" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


char* Browser::View_stub::viewName()
{
  char* _res;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "viewName" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


struct _global_init_Browser_View {
  _global_init_Browser_View()
  {
    if( ::OpenParts::Part::_narrow_helpers == NULL )
      ::OpenParts::Part::_narrow_helpers = new vector<CORBA::Narrow_proto>;
    ::OpenParts::Part::_narrow_helpers->push_back( Browser::View::_narrow_helper2 );
  }
} __global_init_Browser_View;

#ifdef HAVE_NAMESPACE
namespace Browser { vector<CORBA::Narrow_proto> * View::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * Browser::View::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst _tc_View; };
#else
CORBA::TypeCodeConst Browser::_tc_View;
#endif

void
operator<<=( CORBA::Any &_a, const Browser::View_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_Browser_View, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, Browser::View_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_Browser_View, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, Browser::View_ptr &_obj )
{
  Browser::View_ptr *p;
  if (_a.to_static_any (_marshaller_Browser_View, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}

class _Marshaller_Browser_View : public CORBA::StaticTypeInfo {
    typedef Browser::View_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Browser_View::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_Browser_View::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::Browser::View::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_Browser_View::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Browser_View::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::Browser::View::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_Browser_View::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_Browser_View::typecode()
{
  return Browser::_tc_View;
}

CORBA::StaticTypeInfo *_marshaller_Browser_View;

#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst _tc_ViewList; };
#else
CORBA::TypeCodeConst Browser::_tc_ViewList;
#endif


// Stub interface PrintingExtension
Browser::PrintingExtension::~PrintingExtension()
{
}

void *Browser::PrintingExtension::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:Browser/PrintingExtension:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool Browser::PrintingExtension::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:Browser/PrintingExtension:1.0" ) == 0) {
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

Browser::PrintingExtension_ptr Browser::PrintingExtension::_narrow( CORBA::Object_ptr _obj )
{
  Browser::PrintingExtension_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:Browser/PrintingExtension:1.0" )))
      return _duplicate( (Browser::PrintingExtension_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:Browser/PrintingExtension:1.0" ) ) ) {
      _o = new Browser::PrintingExtension_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

Browser::PrintingExtension_ptr
Browser::PrintingExtension::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

Browser::PrintingExtension_stub::~PrintingExtension_stub()
{
}

void Browser::PrintingExtension_stub::print()
{
  CORBA::StaticRequest __req( this, "print" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


#ifdef HAVE_NAMESPACE
namespace Browser { vector<CORBA::Narrow_proto> * PrintingExtension::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * Browser::PrintingExtension::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst _tc_PrintingExtension; };
#else
CORBA::TypeCodeConst Browser::_tc_PrintingExtension;
#endif

void
operator<<=( CORBA::Any &_a, const Browser::PrintingExtension_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_Browser_PrintingExtension, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, Browser::PrintingExtension_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_Browser_PrintingExtension, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, Browser::PrintingExtension_ptr &_obj )
{
  Browser::PrintingExtension_ptr *p;
  if (_a.to_static_any (_marshaller_Browser_PrintingExtension, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}

class _Marshaller_Browser_PrintingExtension : public CORBA::StaticTypeInfo {
    typedef Browser::PrintingExtension_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Browser_PrintingExtension::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_Browser_PrintingExtension::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::Browser::PrintingExtension::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_Browser_PrintingExtension::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Browser_PrintingExtension::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::Browser::PrintingExtension::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_Browser_PrintingExtension::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_Browser_PrintingExtension::typecode()
{
  return Browser::_tc_PrintingExtension;
}

CORBA::StaticTypeInfo *_marshaller_Browser_PrintingExtension;


// Stub interface MagnifyingExtension
Browser::MagnifyingExtension::~MagnifyingExtension()
{
}

void *Browser::MagnifyingExtension::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:Browser/MagnifyingExtension:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool Browser::MagnifyingExtension::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:Browser/MagnifyingExtension:1.0" ) == 0) {
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

Browser::MagnifyingExtension_ptr Browser::MagnifyingExtension::_narrow( CORBA::Object_ptr _obj )
{
  Browser::MagnifyingExtension_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:Browser/MagnifyingExtension:1.0" )))
      return _duplicate( (Browser::MagnifyingExtension_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:Browser/MagnifyingExtension:1.0" ) ) ) {
      _o = new Browser::MagnifyingExtension_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

Browser::MagnifyingExtension_ptr
Browser::MagnifyingExtension::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

Browser::MagnifyingExtension_stub::~MagnifyingExtension_stub()
{
}

void Browser::MagnifyingExtension_stub::zoomIn()
{
  CORBA::StaticRequest __req( this, "zoomIn" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void Browser::MagnifyingExtension_stub::zoomOut()
{
  CORBA::StaticRequest __req( this, "zoomOut" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


CORBA::Boolean Browser::MagnifyingExtension_stub::canZoomIn()
{
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "canZoomIn" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


CORBA::Boolean Browser::MagnifyingExtension_stub::canZoomOut()
{
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "canZoomOut" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifdef HAVE_NAMESPACE
namespace Browser { vector<CORBA::Narrow_proto> * MagnifyingExtension::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * Browser::MagnifyingExtension::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst _tc_MagnifyingExtension; };
#else
CORBA::TypeCodeConst Browser::_tc_MagnifyingExtension;
#endif

void
operator<<=( CORBA::Any &_a, const Browser::MagnifyingExtension_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_Browser_MagnifyingExtension, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, Browser::MagnifyingExtension_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_Browser_MagnifyingExtension, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, Browser::MagnifyingExtension_ptr &_obj )
{
  Browser::MagnifyingExtension_ptr *p;
  if (_a.to_static_any (_marshaller_Browser_MagnifyingExtension, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}

class _Marshaller_Browser_MagnifyingExtension : public CORBA::StaticTypeInfo {
    typedef Browser::MagnifyingExtension_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Browser_MagnifyingExtension::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_Browser_MagnifyingExtension::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::Browser::MagnifyingExtension::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_Browser_MagnifyingExtension::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Browser_MagnifyingExtension::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::Browser::MagnifyingExtension::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_Browser_MagnifyingExtension::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_Browser_MagnifyingExtension::typecode()
{
  return Browser::_tc_MagnifyingExtension;
}

CORBA::StaticTypeInfo *_marshaller_Browser_MagnifyingExtension;


// Stub interface ViewFactory
Browser::ViewFactory::~ViewFactory()
{
}

void *Browser::ViewFactory::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:Browser/ViewFactory:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool Browser::ViewFactory::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:Browser/ViewFactory:1.0" ) == 0) {
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

Browser::ViewFactory_ptr Browser::ViewFactory::_narrow( CORBA::Object_ptr _obj )
{
  Browser::ViewFactory_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:Browser/ViewFactory:1.0" )))
      return _duplicate( (Browser::ViewFactory_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:Browser/ViewFactory:1.0" ) ) ) {
      _o = new Browser::ViewFactory_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

Browser::ViewFactory_ptr
Browser::ViewFactory::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

Browser::ViewFactory_stub::~ViewFactory_stub()
{
}

Browser::View_ptr Browser::ViewFactory_stub::create()
{
  Browser::View_ptr _res;
  CORBA::StaticAny __res( _marshaller_Browser_View, &_res );

  CORBA::StaticRequest __req( this, "create" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifdef HAVE_NAMESPACE
namespace Browser { vector<CORBA::Narrow_proto> * ViewFactory::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * Browser::ViewFactory::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace Browser { CORBA::TypeCodeConst _tc_ViewFactory; };
#else
CORBA::TypeCodeConst Browser::_tc_ViewFactory;
#endif

void
operator<<=( CORBA::Any &_a, const Browser::ViewFactory_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_Browser_ViewFactory, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, Browser::ViewFactory_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_Browser_ViewFactory, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, Browser::ViewFactory_ptr &_obj )
{
  Browser::ViewFactory_ptr *p;
  if (_a.to_static_any (_marshaller_Browser_ViewFactory, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}

class _Marshaller_Browser_ViewFactory : public CORBA::StaticTypeInfo {
    typedef Browser::ViewFactory_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Browser_ViewFactory::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_Browser_ViewFactory::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::Browser::ViewFactory::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_Browser_ViewFactory::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Browser_ViewFactory::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::Browser::ViewFactory::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_Browser_ViewFactory::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_Browser_ViewFactory::typecode()
{
  return Browser::_tc_ViewFactory;
}

CORBA::StaticTypeInfo *_marshaller_Browser_ViewFactory;

class _Marshaller__seq_Browser_View : public CORBA::StaticTypeInfo {
    typedef IfaceSequenceTmpl<Browser::View_var,Browser::View_ptr> _MICO_T;
    static CORBA::TypeCode_ptr _tc;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller__seq_Browser_View::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller__seq_Browser_View::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller__seq_Browser_View::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller__seq_Browser_View::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::ULong len;
  if( !dc.seq_begin( len ) )
    return FALSE;
  ((_MICO_T *) v)->length( len );
  for( CORBA::ULong i = 0; i < len; i++ ) {
    if( !_marshaller_Browser_View->demarshal( dc, &(*(_MICO_T*)v)[i].inout() ) )
      return FALSE;
  }
  return dc.seq_end();
}

void _Marshaller__seq_Browser_View::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::ULong len = ((_MICO_T *) v)->length();
  ec.seq_begin( len );
  for( CORBA::ULong i = 0; i < len; i++ )
    _marshaller_Browser_View->marshal( ec, &(*(_MICO_T*)v)[i].inout() );
  ec.seq_end();
}

CORBA::TypeCode_ptr _Marshaller__seq_Browser_View::typecode()
{
  if (!_tc)
    _tc = (new CORBA::TypeCode (
    "01000000130000003c000000010000000e00000029000000010000001500"
    "000049444c3a42726f777365722f566965773a312e300000000005000000"
    "566965770000000000000000"))->mk_constant();
  return _tc;
}

CORBA::TypeCode_ptr _Marshaller__seq_Browser_View::_tc = 0;
CORBA::StaticTypeInfo *_marshaller__seq_Browser_View;

void operator<<=( CORBA::Any &_a, const IfaceSequenceTmpl<Browser::View_var,Browser::View_ptr> &_s )
{
  CORBA::StaticAny _sa (_marshaller__seq_Browser_View, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, IfaceSequenceTmpl<Browser::View_var,Browser::View_ptr> *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, IfaceSequenceTmpl<Browser::View_var,Browser::View_ptr> &_s )
{
  CORBA::StaticAny _sa (_marshaller__seq_Browser_View, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, IfaceSequenceTmpl<Browser::View_var,Browser::View_ptr> *&_s )
{
  return _a.to_static_any (_marshaller__seq_Browser_View, (void *&)_s);
}


struct __tc_init_BROWSER {
  __tc_init_BROWSER()
  {
    Browser::_tc_URLRequest = 
    "010000000f00000078000000010000001b00000049444c3a42726f777365"
    "722f55524c526571756573743a312e3000000b00000055524c5265717565"
    "73740000040000000400000075726c001200000000000000070000007265"
    "6c6f616400000800000008000000784f6666736574000300000008000000"
    "794f66667365740003000000";
    _marshaller_Browser_URLRequest = new _Marshaller_Browser_URLRequest;
    Browser::_tc_Transfer = 
    "010000000f00000064000000010000001900000049444c3a42726f777365"
    "722f5472616e736665723a312e3000000000090000005472616e73666572"
    "000000000200000007000000736f75726365000012000000000000000c00"
    "000064657374696e6174696f6e001200000000000000";
    _marshaller_Browser_Transfer = new _Marshaller_Browser_Transfer;
    Browser::_tc_EventNewTransfer = 
    "0100000015000000b0000000010000002100000049444c3a42726f777365"
    "722f4576656e744e65775472616e736665723a312e300000000011000000"
    "4576656e744e65775472616e73666572000000000f000000640000000100"
    "00001900000049444c3a42726f777365722f5472616e736665723a312e30"
    "00000000090000005472616e73666572000000000200000007000000736f"
    "75726365000012000000000000000c00000064657374696e6174696f6e00"
    "1200000000000000";
    Browser::_tc_EventOpenURL = 
    "0100000015000000bc000000010000001d00000049444c3a42726f777365"
    "722f4576656e744f70656e55524c3a312e30000000000d0000004576656e"
    "744f70656e55524c000000000f00000078000000010000001b0000004944"
    "4c3a42726f777365722f55524c526571756573743a312e3000000b000000"
    "55524c526571756573740000040000000400000075726c00120000000000"
    "00000700000072656c6f616400000800000008000000784f666673657400"
    "0300000008000000794f66667365740003000000";
    Browser::View::_tc_EventFillMenu = 
    "010000001500000075000000010000002300000049444c3a42726f777365"
    "722f566965772f4576656e7446696c6c4d656e753a312e3000000e000000"
    "4576656e7446696c6c4d656e750000000e0000002d000000010000001900"
    "000049444c3a4f70656e506172747355492f4d656e753a312e3000000000"
    "050000004d656e7500";
    Browser::View::_tc_EventFillToolBar = 
    "010000000f000000b4000000010000002600000049444c3a42726f777365"
    "722f566965772f4576656e7446696c6c546f6f6c4261723a312e30000000"
    "110000004576656e7446696c6c546f6f6c42617200000000030000000700"
    "00006372656174650000080000000b0000007374617274496e6465780000"
    "0300000008000000746f6f6c426172000e00000030000000010000001c00"
    "000049444c3a4f70656e506172747355492f546f6f6c4261723a312e3000"
    "08000000546f6f6c42617200";
    _marshaller_Browser_View_EventFillToolBar = new _Marshaller_Browser_View_EventFillToolBar;
    Browser::_tc_View = 
    "010000000e00000029000000010000001500000049444c3a42726f777365"
    "722f566965773a312e3000000000050000005669657700";
    _marshaller_Browser_View = new _Marshaller_Browser_View;
    Browser::_tc_ViewList = 
    "010000001500000078000000010000001900000049444c3a42726f777365"
    "722f566965774c6973743a312e300000000009000000566965774c697374"
    "00000000130000003c000000010000000e00000029000000010000001500"
    "000049444c3a42726f777365722f566965773a312e300000000005000000"
    "566965770000000000000000";
    Browser::_tc_PrintingExtension = 
    "010000000e00000042000000010000002200000049444c3a42726f777365"
    "722f5072696e74696e67457874656e73696f6e3a312e3000000012000000"
    "5072696e74696e67457874656e73696f6e00";
    _marshaller_Browser_PrintingExtension = new _Marshaller_Browser_PrintingExtension;
    Browser::_tc_MagnifyingExtension = 
    "010000000e00000044000000010000002400000049444c3a42726f777365"
    "722f4d61676e696679696e67457874656e73696f6e3a312e300014000000"
    "4d61676e696679696e67457874656e73696f6e00";
    _marshaller_Browser_MagnifyingExtension = new _Marshaller_Browser_MagnifyingExtension;
    Browser::_tc_ViewFactory = 
    "010000000e00000034000000010000001c00000049444c3a42726f777365"
    "722f56696577466163746f72793a312e30000c0000005669657746616374"
    "6f727900";
    _marshaller_Browser_ViewFactory = new _Marshaller_Browser_ViewFactory;
    _marshaller__seq_Browser_View = new _Marshaller__seq_Browser_View;
  }
};

static __tc_init_BROWSER __init_BROWSER;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

Browser::View_skel::View_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Browser/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:Browser/View:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<View_skel>( this ) );
}

Browser::View_skel::View_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Browser/View:1.0", "View" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<View_skel>( this ) );
}

Browser::View_skel::~View_skel()
{
}

bool Browser::View_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    extern CORBA::ULong mico_string_hash (const char *, CORBA::ULong);
    switch (mico_string_hash (_req->op_name(), 7)) {
    case 0:
      if( strcmp( _req->op_name(), "yOffset" ) == 0 ) {
        CORBA::Long _res;
        CORBA::StaticAny __res( CORBA::_stc_long, &_res );
        _req->set_result( &__res );

        if( !_req->read_args() )
          return true;

        _res = yOffset();
        _req->write_results();
        return true;
      }
      break;
    case 3:
      if( strcmp( _req->op_name(), "stop" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        stop();
        _req->write_results();
        return true;
      }
      break;
    case 4:
      if( strcmp( _req->op_name(), "viewName" ) == 0 ) {
        char* _res;
        CORBA::StaticAny __res( CORBA::_stc_string, &_res );
        _req->set_result( &__res );

        if( !_req->read_args() )
          return true;

        _res = viewName();
        _req->write_results();
        CORBA::string_free( _res );
        return true;
      }
      break;
    case 6:
      if( strcmp( _req->op_name(), "url" ) == 0 ) {
        char* _res;
        CORBA::StaticAny __res( CORBA::_stc_string, &_res );
        _req->set_result( &__res );

        if( !_req->read_args() )
          return true;

        _res = url();
        _req->write_results();
        CORBA::string_free( _res );
        return true;
      }
      if( strcmp( _req->op_name(), "xOffset" ) == 0 ) {
        CORBA::Long _res;
        CORBA::StaticAny __res( CORBA::_stc_long, &_res );
        _req->set_result( &__res );

        if( !_req->read_args() )
          return true;

        _res = xOffset();
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

Browser::View_ptr Browser::View_skel::_this()
{
  return Browser::View::_duplicate( this );
}


Browser::PrintingExtension_skel::PrintingExtension_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Browser/PrintingExtension:1.0", "PrintingExtension" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:Browser/PrintingExtension:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<PrintingExtension_skel>( this ) );
}

Browser::PrintingExtension_skel::PrintingExtension_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Browser/PrintingExtension:1.0", "PrintingExtension" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<PrintingExtension_skel>( this ) );
}

Browser::PrintingExtension_skel::~PrintingExtension_skel()
{
}

bool Browser::PrintingExtension_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "print" ) == 0 ) {

      if( !_req->read_args() )
        return true;

      print();
      _req->write_results();
      return true;
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

Browser::PrintingExtension_ptr Browser::PrintingExtension_skel::_this()
{
  return Browser::PrintingExtension::_duplicate( this );
}


Browser::MagnifyingExtension_skel::MagnifyingExtension_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Browser/MagnifyingExtension:1.0", "MagnifyingExtension" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:Browser/MagnifyingExtension:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<MagnifyingExtension_skel>( this ) );
}

Browser::MagnifyingExtension_skel::MagnifyingExtension_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Browser/MagnifyingExtension:1.0", "MagnifyingExtension" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<MagnifyingExtension_skel>( this ) );
}

Browser::MagnifyingExtension_skel::~MagnifyingExtension_skel()
{
}

bool Browser::MagnifyingExtension_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    extern CORBA::ULong mico_string_hash (const char *, CORBA::ULong);
    switch (mico_string_hash (_req->op_name(), 7)) {
    case 0:
      if( strcmp( _req->op_name(), "canZoomIn" ) == 0 ) {
        CORBA::Boolean _res;
        CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
        _req->set_result( &__res );

        if( !_req->read_args() )
          return true;

        _res = canZoomIn();
        _req->write_results();
        return true;
      }
      break;
    case 1:
      if( strcmp( _req->op_name(), "zoomIn" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        zoomIn();
        _req->write_results();
        return true;
      }
      break;
    case 5:
      if( strcmp( _req->op_name(), "zoomOut" ) == 0 ) {

        if( !_req->read_args() )
          return true;

        zoomOut();
        _req->write_results();
        return true;
      }
      break;
    case 6:
      if( strcmp( _req->op_name(), "canZoomOut" ) == 0 ) {
        CORBA::Boolean _res;
        CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
        _req->set_result( &__res );

        if( !_req->read_args() )
          return true;

        _res = canZoomOut();
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

Browser::MagnifyingExtension_ptr Browser::MagnifyingExtension_skel::_this()
{
  return Browser::MagnifyingExtension::_duplicate( this );
}


Browser::ViewFactory_skel::ViewFactory_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Browser/ViewFactory:1.0", "ViewFactory" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:Browser/ViewFactory:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<ViewFactory_skel>( this ) );
}

Browser::ViewFactory_skel::ViewFactory_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Browser/ViewFactory:1.0", "ViewFactory" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<ViewFactory_skel>( this ) );
}

Browser::ViewFactory_skel::~ViewFactory_skel()
{
}

bool Browser::ViewFactory_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "create" ) == 0 ) {
      View_ptr _res;
      CORBA::StaticAny __res( _marshaller_Browser_View, &_res );
      _req->set_result( &__res );

      if( !_req->read_args() )
        return true;

      _res = create();
      _req->write_results();
      CORBA::release( _res );
      return true;
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

Browser::ViewFactory_ptr Browser::ViewFactory_skel::_this()
{
  return Browser::ViewFactory::_duplicate( this );
}

