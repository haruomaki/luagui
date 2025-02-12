---@meta BulletPhysics

---Bullet Physicsのシミュレーション世界を表すクラス。
---@class BulletWorld
---@field gravity vec3 重力加速度
BulletWorld = {}

---一つの剛体を表すコンポーネント。
---@class Rigidbody : Component
---@field mass number 質量
---@field linear_velocity vec3 速度
---@field linear_factor vec3 移動制限
---@field angular_velocity vec3 角速度
---@field angular_factor vec3 回転制限
---@field inertia vec3 回転のしにくさ
---@field friction number 摩擦係数
---@field restitution number 反発係数
Rigidbody = {}

---直方体の当たり判定を追加。
---@param x number
---@param y number
---@param z number
---@return Collider
function Rigidbody:box_shape(x, y, z) end

---無限に広がる平面の当たり判定を追加。
---@param x number
---@param y number
---@param z number
---@param distance number
---@return Collider
function Rigidbody:plane_shape(x, y, z, distance) end

---円柱状の当たり判定を追加。
---@param x number
---@param y number
---@param z number
---@return Collider
function Rigidbody:cylinder_shape(x, y, z) end

---カプセル型の当たり判定を追加。
---@param radius number
---@param height number
---@return Collider
function Rigidbody:capsule_shape(radius, height) end

---物体に剛体を付与する。
---@return Rigidbody
function WorldObject:add_rigidbody() end

---衝突判定形状を表すコンポーネント。
---@class Collider : Component
Collider = {}

---物体に剛体を付与する。
---@return Collider
function WorldObject:add_collider() end

-- lua.new_usertype<RaycastHit>(
--     "RaycastHit",
--     "hitObject", &RaycastHit::hit_object,
--     "hitPoint", sol::readonly_property([](RaycastHit h) { return bt_to_glm(h.hit_point); }),
--     "hitNormal", sol::readonly_property([](RaycastHit h) { return bt_to_glm(h.hit_normal); }),
--     "hitFraction", &RaycastHit::hit_fraction);

---レイキャストの結果の情報。
---@class RaycastHit
---@field hitObject Rigidbody
---@field hitPoint vec3
---@field hitNormal vec3
---@field hitFraction number
RaycastHit = {}
