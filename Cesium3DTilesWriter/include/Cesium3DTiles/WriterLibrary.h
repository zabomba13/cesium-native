#pragma once

#if defined(_WIN32) && defined(CESIUM_SHARED)
#ifdef CESIUM3DTILESWRITER_BUILDING
#define CESIUM3DTILESWRITER_API __declspec(dllexport)
#else
#define CESIUM3DTILESWRITER_API __declspec(dllimport)
#endif
#else
#define CESIUM3DTILESWRITER_API
#endif
