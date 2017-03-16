#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_CONTRIB_C
#define HS_HEADER_DEF_CONTRIB_C
//////////////////////////////////////////////////////////////////////////

#ifdef USE_FUNCS_WIN32ICONS
#include "DIB.C"
#include "ICONS.C"
#include "SAVEICON.CPP"
#endif //USE_FUNCS_WIN32ICONS

//////////////////////////////////////////////////////////////////////////

#ifdef USE_FUNCS_HTMLHELPER
#include "HtmlHelper.cpp"
#endif //USE_FUNCS_HTMLHELPER

#ifdef USE_FUNCS_REMOTETEXT
#include "RemoteText.cpp"
#endif //USE_FUNCS_REMOTETEXT

//////////////////////////////////////////////////////////////////////////
#endif //HS_HEADER_DEF_CONTRIB_C

