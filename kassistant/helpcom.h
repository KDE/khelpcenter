/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__HELPCOM_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __HELPCOM_H__

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

class helpcom;
typedef helpcom *helpcom_ptr;
typedef helpcom_ptr helpcomRef;
typedef ObjVar<helpcom> helpcom_var;
typedef helpcom_var helpcom_out;


// Common definitions for interface helpcom
class helpcom : virtual public CORBA::Object
{
  public:
    virtual ~helpcom();
    static helpcom_ptr _duplicate( helpcom_ptr obj );
    static helpcom_ptr _narrow( CORBA::Object_ptr obj );
    static helpcom_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual CORBA::ULong registerApp( const char* name ) = 0;
    virtual void removeApp( CORBA::ULong id ) = 0;
    virtual void showDocument( CORBA::ULong id, const char* name ) = 0;
    virtual void documentHint( CORBA::ULong id, const char* name ) = 0;
    virtual void activate( CORBA::ULong id ) = 0;
    virtual void configure() = 0;
  protected:
    helpcom() {};
  private:
    helpcom( const helpcom& );
    void operator=( const helpcom& );
};

extern CORBA::TypeCodeConst _tc_helpcom;

// Stub for interface helpcom
class helpcom_stub : virtual public helpcom
{
  public:
    virtual ~helpcom_stub();
    CORBA::ULong registerApp( const char* name );
    void removeApp( CORBA::ULong id );
    void showDocument( CORBA::ULong id, const char* name );
    void documentHint( CORBA::ULong id, const char* name );
    void activate( CORBA::ULong id );
    void configure();
  private:
    void operator=( const helpcom_stub& );
};

class helpcom_skel :
  virtual public MethodDispatcher,
  virtual public helpcom
{
  public:
    helpcom_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~helpcom_skel();
    helpcom_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    helpcom_ptr _this();

};

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &a, const helpcom_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, helpcom_ptr &obj );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#if !defined(MICO_NO_TOPLEVEL_MODULES) && !defined(MICO_IN_GENERATED_CODE)
#include <mico/template_impl.h>
#endif

#endif
