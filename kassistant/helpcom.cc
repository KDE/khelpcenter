/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <helpcom.h>

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------

// Stub interface helpcom
helpcom::~helpcom()
{
}

helpcom_ptr helpcom::_duplicate( helpcom_ptr _obj )
{
  if( !CORBA::is_nil( _obj ) )
    _obj->_ref();
  return _obj;
}

void *helpcom::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:helpcom:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool helpcom::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:helpcom:1.0" ) == 0) {
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

helpcom_ptr helpcom::_narrow( CORBA::Object_ptr _obj )
{
  helpcom_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:helpcom:1.0" )))
      return _duplicate( (helpcom_ptr) _p );
    if( _narrow_helper2( _obj ) ) {
      _o = new helpcom_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
    if( _obj->_is_a_remote( "IDL:helpcom:1.0" ) ) {
      _o = new helpcom_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

helpcom_ptr helpcom::_nil()
{
  return NULL;
}

helpcom_stub::~helpcom_stub()
{
}

CORBA::ULong helpcom_stub::registerApp( const char* name )
{
  CORBA::Request_var _req = this->_request( "registerApp" );
  _req->add_in_arg( "name" ) <<= CORBA::Any::from_string( (char *) name, 0 );
  _req->result()->value()->type( CORBA::_tc_ulong );
  _req->invoke();
  #ifdef HAVE_EXCEPTIONS
  if( _req->env()->exception() ) {
    CORBA::Exception *_ex = _req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_narrow( _ex );
    if( _uuex ) {
      mico_throw( CORBA::UNKNOWN() );
    } else {
      mico_throw( *_ex );
    }
  }
  #else
  {
    CORBA::Exception *_ex;
    if( (_ex = _req->env()->exception()) )
      CORBA::Exception::_throw_failed( _ex );
  }
  #endif
  CORBA::ULong _res;
  *_req->result()->value() >>= _res;
  return _res;
}


void helpcom_stub::removeApp( CORBA::ULong id )
{
  CORBA::Request_var _req = this->_request( "removeApp" );
  _req->add_in_arg( "id" ) <<= id;
  _req->result()->value()->type( CORBA::_tc_void );
  _req->invoke();
  #ifdef HAVE_EXCEPTIONS
  if( _req->env()->exception() ) {
    CORBA::Exception *_ex = _req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_narrow( _ex );
    if( _uuex ) {
      mico_throw( CORBA::UNKNOWN() );
    } else {
      mico_throw( *_ex );
    }
  }
  #else
  {
    CORBA::Exception *_ex;
    if( (_ex = _req->env()->exception()) )
      CORBA::Exception::_throw_failed( _ex );
  }
  #endif
}


void helpcom_stub::showDocument( CORBA::ULong id, const char* name )
{
  CORBA::Request_var _req = this->_request( "showDocument" );
  _req->add_in_arg( "id" ) <<= id;
  _req->add_in_arg( "name" ) <<= CORBA::Any::from_string( (char *) name, 0 );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->invoke();
  #ifdef HAVE_EXCEPTIONS
  if( _req->env()->exception() ) {
    CORBA::Exception *_ex = _req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_narrow( _ex );
    if( _uuex ) {
      mico_throw( CORBA::UNKNOWN() );
    } else {
      mico_throw( *_ex );
    }
  }
  #else
  {
    CORBA::Exception *_ex;
    if( (_ex = _req->env()->exception()) )
      CORBA::Exception::_throw_failed( _ex );
  }
  #endif
}


void helpcom_stub::documentHint( CORBA::ULong id, const char* name )
{
  CORBA::Request_var _req = this->_request( "documentHint" );
  _req->add_in_arg( "id" ) <<= id;
  _req->add_in_arg( "name" ) <<= CORBA::Any::from_string( (char *) name, 0 );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->invoke();
  #ifdef HAVE_EXCEPTIONS
  if( _req->env()->exception() ) {
    CORBA::Exception *_ex = _req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_narrow( _ex );
    if( _uuex ) {
      mico_throw( CORBA::UNKNOWN() );
    } else {
      mico_throw( *_ex );
    }
  }
  #else
  {
    CORBA::Exception *_ex;
    if( (_ex = _req->env()->exception()) )
      CORBA::Exception::_throw_failed( _ex );
  }
  #endif
}


void helpcom_stub::activate( CORBA::ULong id )
{
  CORBA::Request_var _req = this->_request( "activate" );
  _req->add_in_arg( "id" ) <<= id;
  _req->result()->value()->type( CORBA::_tc_void );
  _req->invoke();
  #ifdef HAVE_EXCEPTIONS
  if( _req->env()->exception() ) {
    CORBA::Exception *_ex = _req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_narrow( _ex );
    if( _uuex ) {
      mico_throw( CORBA::UNKNOWN() );
    } else {
      mico_throw( *_ex );
    }
  }
  #else
  {
    CORBA::Exception *_ex;
    if( (_ex = _req->env()->exception()) )
      CORBA::Exception::_throw_failed( _ex );
  }
  #endif
}


void helpcom_stub::configure()
{
  CORBA::Request_var _req = this->_request( "configure" );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->invoke();
  #ifdef HAVE_EXCEPTIONS
  if( _req->env()->exception() ) {
    CORBA::Exception *_ex = _req->env()->exception();
    CORBA::UnknownUserException *_uuex = CORBA::UnknownUserException::_narrow( _ex );
    if( _uuex ) {
      mico_throw( CORBA::UNKNOWN() );
    } else {
      mico_throw( *_ex );
    }
  }
  #else
  {
    CORBA::Exception *_ex;
    if( (_ex = _req->env()->exception()) )
      CORBA::Exception::_throw_failed( _ex );
  }
  #endif
}


vector<CORBA::Narrow_proto> * helpcom::_narrow_helpers;
CORBA::TypeCodeConst _tc_helpcom;

CORBA::Boolean
operator<<=( CORBA::Any &_a, const helpcom_ptr _obj )
{
  return (_a <<= CORBA::Any::from_object( _obj, "helpcom" ));
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, helpcom_ptr &_obj )
{
  CORBA::Object_ptr _o;
  if( !(_a >>= CORBA::Any::to_object( _o )) )
    return FALSE;
  if( CORBA::is_nil( _o ) ) {
    _obj = ::helpcom::_nil();
    return TRUE;
  }
  _obj = ::helpcom::_narrow( _o );
  CORBA::release( _o );
  return TRUE;
}

struct __tc_init_HELPCOM {
  __tc_init_HELPCOM()
  {
    _tc_helpcom = "010000000e00000024000000010000001000000049444c3a68656c70636f"
    "6d3a312e30000800000068656c70636f6d00";
  }
};

static __tc_init_HELPCOM __init_HELPCOM;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// Dynamic Implementation Routine for interface helpcom
helpcom_skel::helpcom_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:helpcom:1.0", "helpcom" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:helpcom:1.0" );
  register_dispatcher( new InterfaceDispatcherWrapper<helpcom_skel>( this ) );
}

helpcom_skel::helpcom_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:helpcom:1.0", "helpcom" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new InterfaceDispatcherWrapper<helpcom_skel>( this ) );
}

helpcom_skel::~helpcom_skel()
{
}

bool helpcom_skel::dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  if( strcmp( _req->op_name(), "registerApp" ) == 0 ) {
    CORBA::String_var name;

    CORBA::NVList_ptr _args = new CORBA::NVList (1);
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_string );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= CORBA::Any::to_string( name, 0 );
    CORBA::ULong _res;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      _res = registerApp( name );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    CORBA::Any *_any_res = new CORBA::Any;
    *_any_res <<= _res;
    _req->result( _any_res );
    return true;
  }
  if( strcmp( _req->op_name(), "removeApp" ) == 0 ) {
    CORBA::ULong id;

    CORBA::NVList_ptr _args = new CORBA::NVList (1);
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_ulong );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= id;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      removeApp( id );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    return true;
  }
  if( strcmp( _req->op_name(), "showDocument" ) == 0 ) {
    CORBA::ULong id;
    CORBA::String_var name;

    CORBA::NVList_ptr _args = new CORBA::NVList (2);
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_ulong );
    _args->add( CORBA::ARG_IN );
    _args->item( 1 )->value()->type( CORBA::_tc_string );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= id;
    *_args->item( 1 )->value() >>= CORBA::Any::to_string( name, 0 );
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      showDocument( id, name );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    return true;
  }
  if( strcmp( _req->op_name(), "documentHint" ) == 0 ) {
    CORBA::ULong id;
    CORBA::String_var name;

    CORBA::NVList_ptr _args = new CORBA::NVList (2);
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_ulong );
    _args->add( CORBA::ARG_IN );
    _args->item( 1 )->value()->type( CORBA::_tc_string );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= id;
    *_args->item( 1 )->value() >>= CORBA::Any::to_string( name, 0 );
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      documentHint( id, name );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    return true;
  }
  if( strcmp( _req->op_name(), "activate" ) == 0 ) {
    CORBA::ULong id;

    CORBA::NVList_ptr _args = new CORBA::NVList (1);
    _args->add( CORBA::ARG_IN );
    _args->item( 0 )->value()->type( CORBA::_tc_ulong );

    if (!_req->params( _args ))
      return true;

    *_args->item( 0 )->value() >>= id;
    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      activate( id );
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    return true;
  }
  if( strcmp( _req->op_name(), "configure" ) == 0 ) {
    CORBA::NVList_ptr _args = new CORBA::NVList (0);

    if (!_req->params( _args ))
      return true;

    #ifdef HAVE_EXCEPTIONS
    try {
    #endif
      configure();
    #ifdef HAVE_EXCEPTIONS
    } catch( CORBA::SystemException_var &_ex ) {
      _req->exception( _ex->_clone() );
      return true;
    } catch( ... ) {
      assert( 0 );
      return true;
    }
    #endif
    return true;
  }
  return false;
}

helpcom_ptr helpcom_skel::_this()
{
  return helpcom::_duplicate( this );
}

