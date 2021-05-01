/**
 * @file
 * 
 * @brief Manages asset files such as texture, audio, block type definition file, etc.
 * @details
 * 
 */
#ifndef H_ASSET
#define H_ASSET

#ifdef EMSCRIPTEN
#include <emscripten.h>

#define ASSET_ROOD_DIR "/res/"
#else
#define ASSET_ROOT_DIR ""
#endif

#include <memory>
#include <string>
#include <filesystem>

template<typename T>
std::unique_ptr<T> GetAsset(std::wstring path);

#endif