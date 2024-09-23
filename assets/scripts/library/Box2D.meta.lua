---@meta Box2D

-------------
-- libGUIから
-------------

---コンポーネントおよび剛体そのものを表すクラス。RigidbodyComponentとb2::Bodyの機能を統合する。
---@class Rigidbody : Component
---@field linear_velocity b2Vec2 速度ベクトル
local Rigidbody = {}

---剛体に衝突形状を追加する。
---@param shape_options ShapeOptions
function Rigidbody:add_shape(shape_options) end

---衝突判定体を表すクラス。
---@class Collider : Component
---@field index integer Shapeのインデックス番号
local Collider = {}

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

---Box2Dにおける単位系（長さの単位）を指定する。
---@param lengthUnits number 単位長さ
function b2SetLengthUnitsPerMeter(lengthUnits) end

-------------
-- その他
-------------

---add_rigidbody_componentに指定できるオプション集
---@class BodyOptions
---@field type "static"|"kinematic"|"dynamic"? 演算タイプ
local BodyOptions = {}

---add_shapeに指定できるオプション集
---@class ShapeOptions
---@field shape ShapeType? 形状のタイプ
---@field radius number? 円の半径（shapeが"circle"の場合に必要）。
---@field center table? 円の中心座標（shapeが"circle"の場合に必要）。
---@field halfWidth number? 矩形の半幅（shapeが"rect"の場合に必要）。
---@field halfHeight number? 矩形の半高さ（shapeが"rect"の場合に必要）。
---@field points Points? 辺の端点（shapeが"edge"の場合に必要）。
---@field friction number? 摩擦係数
---@field restitution number? 反発係数
---@field on_collision_enter fun(self: Collider, other: Collider)? 衝突したときに実行するコールバック関数。
local ShapeOptions = {}

---衝突形状の種類
---@alias ShapeType "circle" | "rect" | "edge"
