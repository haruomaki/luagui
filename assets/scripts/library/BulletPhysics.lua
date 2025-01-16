---@meta BulletPhysics

---一つの剛体を表すコンポーネント。
---@class Rigidbody
---@field mass number 質量
---@field inertia Point 回転のしにくさ
---@field restitution number 反発係数
Rigidbody = {}

---物体に剛体を付与する。
---@return Rigidbody
function WorldObject:add_rigidbody() end
