#pragma once

#include <Draw.hpp>
#include <GUI.hpp>
#include <KeyCallback.hpp>
#include <Shader.hpp>
#include <SizeCallback.hpp>
#include <Timer.hpp>
#include <Update.hpp>
#include <Window.hpp>
#include <World.hpp>
#include <WorldObject.hpp>

template <typename T>
struct Rect {
    T left_, right_, bottom_, top_;
};

// Windowを利用するヘッダ。Windowの宣言のあとにインクルード
#include <Camera.hpp>

#include <DynamicArray.hpp>
#include <Polygon.hpp>
#include <Text.hpp>
