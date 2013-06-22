/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is 
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or 
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

//////////////////////////////////////////////////
//
// CPlugin class implementation
//

#include <string.h>
#include "plugin.h"
#include "util_pipeline.h"
#include "gesture_render.h"
#include <iostream>
#include <process.h>

class MyPipeline: public UtilPipeline {
public:
	MyPipeline(void):UtilPipeline(),m_render(L"Gesture Viewer") { 
		EnableGesture();
	}
	virtual void PXCAPI OnGesture(PXCGesture::Gesture *data) {
		if ( data->active && data->label == PXCGesture::Gesture::LABEL_POSE_BIG5 )
			std::cout << "Stop!" << std::endl;

		if (data->active) m_gdata=(*data); 
	}
	virtual bool OnNewFrame(void) {
		MyOnNewFrame();
		return m_render.RenderFrame(QueryImage(PXCImage::IMAGE_TYPE_DEPTH), QueryGesture(), &m_gdata);
	}
protected:
	void MyOnNewFrame();

	GestureRender		m_render;
	PXCGesture::Gesture m_gdata;
};

PXCGesture::GeoNode QueryNode( pxcUID user, PXCGesture* detector, PXCGesture::GeoNode::Label label )
{
	PXCGesture::GeoNode result;
    if (detector->QueryNodeData(user, 
								PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT|label,
								&result)<PXC_STATUS_NO_ERROR)
		if (detector->QueryNodeData(user, 
								   PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT|label,
								   &result)<PXC_STATUS_NO_ERROR)
			return PXCGesture::GeoNode();
	
	return result;
}

void MyPipeline::MyOnNewFrame()
{
	PXCGesture* detector = QueryGesture();
    pxcUID user=0;
    pxcStatus sts= detector->QueryUser(0,&user);
    if (sts<PXC_STATUS_NO_ERROR) return;
	
	PXCGesture::GeoNode thumb = QueryNode( user, detector, PXCGesture::GeoNode::LABEL_FINGER_THUMB );
	PXCGesture::GeoNode index = QueryNode( user, detector, PXCGesture::GeoNode::LABEL_FINGER_INDEX );
	PXCGesture::GeoNode middle = QueryNode( user, detector, PXCGesture::GeoNode::LABEL_FINGER_MIDDLE );
	PXCGesture::GeoNode ring = QueryNode( user, detector, PXCGesture::GeoNode::LABEL_FINGER_RING );
	PXCGesture::GeoNode pinky = QueryNode( user, detector, PXCGesture::GeoNode::LABEL_FINGER_PINKY );

	int fingerCount = ( thumb.confidence ? 1 : 0 )
					+ ( index.confidence ? 1 : 0 )
					+ ( middle.confidence ? 1 : 0 )
					+ ( ring.confidence ? 1 : 0 )
					+ ( pinky.confidence ? 1 : 0 );

	if ( fingerCount == 0 )
		return;
	
	PXCPoint3DF32 positionImage = thumb.confidence ? thumb.positionImage
								: index.confidence ? index.positionImage
								: middle.confidence ? middle.positionImage
								: ring.confidence ? ring.positionImage
								: pinky.positionImage;
	
	std::cout << fingerCount << ": (" << positionImage.x << "," << positionImage.y << ")" << std::endl;
}

const char* kSayHello = "sayHello";
const char* kAddEventHandler = "addEventHandler";
const char* kRemoveEventHandler = "removeEventHandler";

static NPClass plugin_ref_obj = {
  NP_CLASS_STRUCT_VERSION,
  ScriptablePluginObject::Allocate,
  ScriptablePluginObject::Deallocate,
  NULL,
  ScriptablePluginObject::HasMethod,
  ScriptablePluginObject::Invoke,
  ScriptablePluginObject::InvokeDefault,
  ScriptablePluginObject::HasProperty,
  ScriptablePluginObject::GetProperty,
  NULL,
  NULL,
};

ScriptablePluginObject::ScriptablePluginObject(NPP instance)
    : npp(instance) {
}

NPObject* ScriptablePluginObject::Allocate(NPP instance, NPClass* npclass) {
  return (NPObject*)(new ScriptablePluginObject(instance));
}

void ScriptablePluginObject::Deallocate(NPObject* obj) {
  delete (ScriptablePluginObject*)obj;
}

bool ScriptablePluginObject::HasMethod(NPObject* obj, NPIdentifier methodName) {
  return true;
}

bool ScriptablePluginObject::InvokeDefault(NPObject* obj, const NPVariant* args,
                            uint32_t argCount, NPVariant* result) {
  return true;
}

bool ScriptablePluginObject::Invoke(NPObject* obj, NPIdentifier methodName,
                     const NPVariant* args, uint32_t argCount,
                     NPVariant* result) {
  ScriptablePluginObject *thisObj = (ScriptablePluginObject*)obj;
  char* name = npnfuncs->utf8fromidentifier(methodName);
  bool ret_val = false;
  if (!name) {
    return ret_val;
  }
  if (!strcmp(name, kSayHello)) {
    ret_val = true;
	//TODO ��дhelloworld
	const char* outString = "hello world!\ncall from my plugin";
    char* npOutString = (char *)npnfuncs->memalloc(strlen(outString) + 1);
    if (!npOutString)
      return false;
    strcpy(npOutString, outString);
    STRINGZ_TO_NPVARIANT(npOutString, *result);
  } else if ( strcmp(name, kAddEventHandler)==0 ) {
	  if ( argCount != 2 || args[0].type != NPVariantType_String || args[1].type != NPVariantType_Object ) {
		  npnfuncs->setexception(obj, "Invalid parameters to addEventHandler method");
	  }
	  NPString type = args[0].value.stringValue;
	  std::string handler_name( type.UTF8Characters, type.UTF8Length );
	  NPObject* callback = npnfuncs->retainobject( args[1].value.objectValue );

	  m_callback = callback;
	  theNPP = thisObj->npp;
	  DoJavascriptCallback();
	  //TODO: Add to map
  } else if ( strcmp(name, kAddEventHandler)==0 ) {
	  if ( argCount != 2 || args[0].type != NPVariantType_String || args[1].type != NPVariantType_Object ) {
		  npnfuncs->setexception(obj, "Invalid parameters to addEventHandler method");
	  }
	  NPString type = args[0].value.stringValue;
	  std::string handler_name( type.UTF8Characters, type.UTF8Length );

	  //TODO: Remove from map
  } else {
    // Exception handling. 
    npnfuncs->setexception(obj, "Unknown method");
  }
  npnfuncs->memfree(name);
  return ret_val;
}

void DoJavascriptCallback() {
	NPVariant result;
	NPVariant args[1];
	args[0].type = NPVariantType_Int32;
	args[0].value.intValue = 42;
	npnfuncs->invokeDefault( theNPP, m_callback, args, 1, &result );
}

bool ScriptablePluginObject::HasProperty(NPObject* obj, NPIdentifier propertyName) {
  return false;
}

bool ScriptablePluginObject::GetProperty(NPObject* obj, NPIdentifier propertyName,
                          NPVariant* result) {
  return false;
}

CPlugin::CPlugin(NPP pNPInstance) :
  m_pNPInstance(pNPInstance),
  m_bInitialized(false),
  m_pScriptableObject(NULL) {
#ifdef _WINDOWS
  m_hWnd = NULL;
#endif
}

CPlugin::~CPlugin() {
  if (m_pScriptableObject)
    npnfuncs->releaseobject((NPObject*)m_pScriptableObject);
#ifdef _WINDOWS
  m_hWnd = NULL;
#endif
  m_bInitialized = false;
}

void LoopFrames(void*)
{
  MyPipeline pipeline;
  if (!pipeline.LoopFrames()) wprintf_s(L"Failed to initialize or stream data"); 
}

NPBool CPlugin::init(NPWindow* pNPWindow) {
  if(pNPWindow == NULL)
    return false;
#ifdef _WINDOWS
  m_hWnd = (HWND)pNPWindow->window;
  if(m_hWnd == NULL)
    return false;
#endif
  m_Window = pNPWindow;
  m_bInitialized = true;

  _beginthread( &LoopFrames, 0, 0 );

  return true;
}

NPBool CPlugin::isInitialized() {
  return m_bInitialized;
}

ScriptablePluginObject * CPlugin::GetScriptableObject() {
  if (!m_pScriptableObject) {
    m_pScriptableObject = (ScriptablePluginObject*)npnfuncs->createobject(m_pNPInstance, &plugin_ref_obj);

    // Retain the object since we keep it in plugin code
    // so that it won't be freed by browser.
    npnfuncs->retainobject((NPObject*)m_pScriptableObject);
  }

  return m_pScriptableObject;
}

#ifdef _WINDOWS
HWND CPlugin::GetHWnd() {
  return m_hWnd;
}
#endif