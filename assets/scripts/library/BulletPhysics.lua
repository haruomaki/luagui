---@meta BulletPhysics

---一つの剛体を表すコンポーネント。
---@class Rigidbody : Component
---@field mass number 質量
---@field linear_velocity vec3 質量
---@field angular_factor vec3 回転制限
---@field inertia vec3 回転のしにくさ
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
