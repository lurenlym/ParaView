/*=========================================================================

  Program:   ParaView
  Module:    vtkSMTrace.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMTrace
// .SECTION Description
//

#ifndef __vtkSMTrace_h
#define __vtkSMTrace_h

#include "vtkPVServerManagerCoreModule.h"
#include "vtkSmartPointer.h"
#include "vtkSMObject.h"
#include "vtkStdString.h"
#include "vtkWeakPointer.h"

class vtkSMProxy;
class SmartPyObject;

class VTKPVSERVERMANAGERCORE_EXPORT vtkSMTrace : public vtkSMObject
{
public:
  static vtkSMTrace* New();
  vtkTypeMacro(vtkSMTrace, vtkSMObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Provides access to the "active" tracer. There can only be one active tracer
  // in the application currently.
  static vtkSMTrace* GetActiveTracer()
    { return vtkSMTrace::ActiveTracer.GetPointer(); }

  // Description:
  // Methods to start/stop tracing. This will create a new instance of
  // vtkSMTrace and set that up as the active tracer. If an active tracer is
  // already present, then this will simply return the current active tracer.
  static vtkSMTrace* StartTrace();

  // Description:
  // Stop trace and return the generated trace script.
  // This will also destroy the active tracer.
  static vtkStdString StopTrace();

  // Description:
  // Get/Set whether all properties should be saved for a proxy,
  // including the default values. If false, only the properties
  // that have been modified from the XML-defaults will be logged.
  vtkSetMacro(TraceXMLDefaults, bool);
  vtkGetMacro(TraceXMLDefaults, bool);

  // Description:
  // Log generated trace to stdout as the trace is being generated
  // (useful for debugging).
  vtkSetMacro(LogTraceToStdout, bool);
  vtkGetMacro(LogTraceToStdout, bool);

  // Description:
  // Whether to trace properties on proxies that have already been created.
  vtkSetMacro(TracePropertiesOnExistingProxies, bool);
  vtkGetMacro(TracePropertiesOnExistingProxies, bool);

  enum
    {
    RECORD_ALL_PROPERTIES=0,
    RECORD_MODIFIED_PROPERTIES=1,
    RECORD_USER_MODIFIED_PROPERTIES=2
    };

  vtkSetClampMacro(PropertiesToTraceOnCreate, int,
    RECORD_ALL_PROPERTIES, RECORD_USER_MODIFIED_PROPERTIES);
  vtkGetMacro(PropertiesToTraceOnCreate, int);

  // Description:
  // Return the current trace.
  vtkStdString GetCurrentTrace();

  // ************** BEGIN INTERNAL *************************
  // Description:
  // Internal class not meant to be used directly.
  class TraceItem;
  class VTKPVSERVERMANAGERCORE_EXPORT TraceItemArgs
    {
  public:
    TraceItemArgs();
    ~TraceItemArgs();

    // Overloads for keyword arguments.
    TraceItemArgs& arg(const char* key, vtkObject* val);
    TraceItemArgs& arg(const char* key, const char* val);
    TraceItemArgs& arg(const char* key, int val);
    TraceItemArgs& arg(const char* key, double val);
    TraceItemArgs& arg(const char* key, bool val);

    // Overloads for positional arguments.
    TraceItemArgs& arg(vtkObject* val);
    TraceItemArgs& arg(const char* val);
    TraceItemArgs& arg(int val);
    TraceItemArgs& arg(double val);
    TraceItemArgs& arg(bool val);

  private:
    TraceItemArgs(const TraceItemArgs&);
    void operator=(const TraceItemArgs&);

    friend class TraceItem;
    class vtkInternals;
    vtkInternals* Internals;
    };

  class VTKPVSERVERMANAGERCORE_EXPORT TraceItem
    {
  public:
    TraceItem(const char* type);
    ~TraceItem();
    void operator=(const TraceItemArgs& arguments);
  private:
    TraceItem(const TraceItem&);
    void operator=(const TraceItem&);
    const char* Type;
    class TraceItemInternals;
    TraceItemInternals* Internals;
    };
  // ************** END INTERNAL *************************

protected:
  vtkSMTrace();
  virtual ~vtkSMTrace();

  // Description:
  // Returns true of there's an error. Otherwise, returns false.
  bool CheckForError();

  bool TraceXMLDefaults;
  bool LogTraceToStdout;
  int PropertiesToTraceOnCreate;
  bool TracePropertiesOnExistingProxies;

private:
  vtkSMTrace(const vtkSMTrace&); // Not implemented.
  void operator=(const vtkSMTrace&); // Not implemented.

  static vtkSmartPointer<vtkSMTrace> ActiveTracer;
  class vtkInternals;
  vtkInternals* Internals;

  friend class TraceItem;
  const SmartPyObject& GetTraceModule() const;
  const SmartPyObject& GetCreateItemFunction() const;
};

#define SM_SCOPED_TRACE_0(x, y) x ## y
#define SM_SCOPED_TRACE_1(x, y) SM_SCOPED_TRACE_0(x, y)
#define SM_SCOPED_TRACE(__A_TRACE_TYPE) \
  vtkSMTrace::TraceItem SM_SCOPED_TRACE_1(_trace_item,__LINE__)(#__A_TRACE_TYPE); \
  SM_SCOPED_TRACE_1(_trace_item,__LINE__) = vtkSMTrace::TraceItemArgs()


//namespace smtrace
//{
//  class VTKPVSERVERMANAGERCORE_EXPORT Action
//    {
//  public:
//    Action();
//    virtual ~Action();
//  protected:
//    ostream& GetStream() const { return *this->Stream; }
//
//    // Description:
//    void AssignVariable(vtkSMProxy* proxy, const char* name);
//
//    const char* GetVariable(vtkSMProxy*);
//
//    // Description:
//    // Trace the properties that must absolutely be set in the constructor
//    // for reliable scripts that don't raise warnings about missing inputs,
//    // filenames and such.
//    bool TraceCtorProperties(vtkSMProxy* proxy);
//
//    // Description:
//    // Trace properties on a proxy.
//    bool TraceProperties(vtkSMProxy* proxy, const char* varName, bool during_creation=false);
//
//    bool IsTracingEnabled() const
//      { return vtkSMTrace::GetActiveTracer() != NULL; }
//
//    bool AreOtherActionsActive() const;
//  private:
//    ostream* Stream;
//    };
//
//  class VTKPVSERVERMANAGERCORE_EXPORT RegisterPipelineProxy : public Action
//  {
//  vtkWeakPointer<vtkSMProxy> Proxy;
//public:
//  RegisterPipelineProxy(vtkSMProxy* proxy);
//  ~RegisterPipelineProxy();
//  };
//
//  class VTKPVSERVERMANAGERCORE_EXPORT UnRegisterPipelineProxy : public Action
//  {
//public:
//  UnRegisterPipelineProxy(vtkSMProxy* proxy);
//  ~UnRegisterPipelineProxy();
//  };
//
//  class VTKPVSERVERMANAGERCORE_EXPORT PropertyModified : public Action
//  {
//public:
//  PropertyModified(vtkSMProxy* proxy, const char* pname);
//  ~PropertyModified();
//  };
//}
//
//#define SM_SCOPED_TRACE(__A_TRACE_TYPE) \
//  smtrace::__A_TRACE_TYPE __temp_##__A_TRACE_TYPE
//
#endif
