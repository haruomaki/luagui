---@meta Box2D

-------------
-- libGUIから
-------------

---コンポーネントおよび剛体そのものを表すクラス。RigidbodyComponentとb2::Bodyの機能を統合する。
---@class Rigidbody : Component
---@field position Point 《読み取り専用》位置
---@field rotation number 《読み取り専用》回転角
---@field transform b2Transform 剛体の空間的配置
---@field linear_velocity Point 速度ベクトル
---@field angular_velocity number 角速度のz軸成分
local Rigidbody = {}

---剛体に衝突形状を追加する。
---@param shape_options ShapeOptions
function Rigidbody:add_shape(shape_options) end

---衝突判定体を表すクラス。
---@class Collider : Component
---@field index integer Shapeのインデックス番号
local Collider = {}

---Chain（折れ線形の衝突判定体）を表すクラス。
---@class ChainCollider : Component
---@field index integer Shapeのインデックス番号
local ChainCollider = {}

---剛体にChainを追加する。
---@param chain_options ChainOptions
function Rigidbody:add_chain(chain_options) end

-------------
-- box2cppから
-------------

---@class b2World
---@field gravity Point 重力加速度のベクトル
local b2World = {}

-------------
-- Box2Dから
-------------

---@class b2Vec2
---@field x number x座標 INFO: プロパティとしてテーブルを代入するため、?を付けている
---@field y number y座標
local b2Vec2 = {}

---@class b2Transform
local b2Transform = {}

---Box2Dにおける単位系（長さの単位）を指定する。
---@param lengthUnits number 単位長さ
function b2SetLengthUnitsPerMeter(lengthUnits) end

-------------
-- その他
-------------

---add_rigidbody_componentに指定できるオプション集
---@class BodyOptions
---@field type "static"|"kinematic"|"dynamic"? 演算タイプ
---@field isBullet boolean? 連続的衝突判定：CCDをdynamicBody同士で行うかどうか
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

---add_chainに指定できるオプション集
---@class ChainOptions
---@field points Points チェーンの座標一覧（右手側に法線）
---@field isLoop boolean? チェーンの両端を閉じるかどうか
---@field friction number? 摩擦係数
---@field restitution number? 反発係数
---@field on_collision_enter fun(self: ChainCollider, collider: Collider)? 衝突したときに実行するコールバック関数。
local ChainOptions = {}
