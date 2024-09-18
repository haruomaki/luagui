---@meta Box2D

-------------
-- libGUIから
-------------

---コンポーネントおよび剛体そのものを表すクラス。RigidbodyComponentとb2::Bodyの機能を統合する。
---@class Rigidbody
local Rigidbody = {}

---剛体に衝突形状を追加する。
---@param shape_options ShapeOptions
function Rigidbody:add_shape(shape_options) end

-------------
-- box2cppから
-------------

---@class b2World
---@field gravity b2Vec2 重力加速度のベクトル
local b2World = {}

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
---@field shape ShapeType? 形状のタイプ
---@field radius number? 円の半径（shapeが"circle"の場合に必要）。
---@field center table? 円の中心座標（shapeが"circle"の場合に必要）。
---@field halfWidth number? 矩形の半幅（shapeが"rect"の場合に必要）。
---@field halfHeight number? 矩形の半高さ（shapeが"rect"の場合に必要）。
---@field points Points? 辺の端点（shapeが"edge"の場合に必要）。
local ShapeOptions = {}

---衝突形状の種類
---@alias ShapeType "circle" | "rect" | "edge"
