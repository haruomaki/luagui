---@meta Box2D

-------------
-- libGUIから
-------------

---@class Rigidbody
---@field body b2Body Box2Dが管理するBody
local Rigidbody = {}

-------------
-- box2cppから
-------------

---@class b2World
---@field gravity b2Vec2 重力加速度のベクトル
local b2World = {}

---一つの剛体を意味するクラス
---@class b2Body
local b2Body = {}

---剛体に衝突形状を追加する。
---@param shape_options ShapeOptions
function b2Body:add_shape(shape_options) end

-------------
-- Box2Dから
-------------

---@class b2Vec2
local b2Vec2 = {}

-------------
-- その他
-------------

---add_shapeに指定できるオプション集
---@class ShapeOptions
---@field shape string? 形状のタイプ（"circle" または "rect"）。
---@field radius number? 円の半径（shapeが"circle"の場合に必要）。
---@field center table? 円の中心座標（shapeが"circle"の場合に必要）。
---@field halfWidth number? 矩形の半幅（shapeが"rect"の場合に必要）。
---@field halfHeight number? 矩形の半高さ（shapeが"rect"の場合に必要）。
local ShapeOptions = {}
