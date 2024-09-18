---@meta WorldObject

---@class WorldObject
---@field position vec3 物体の位置
local WorldObject = {}

---@class MeshObject : WorldObject
local MeshObject = {}

---@class vec3
local vec3 = {}

---Updateコンポーネントを追加する。（＝毎フレーム実行してほしい処理を追加する）
---@param f fun(self: WorldObject) 毎フレームresumeされるコルーチン。このコルーチンが終了したらコンポーネントも削除される。
function WorldObject:add_update_component(f) end

---Rigidbodyコンポーネントを追加する。（＝物理演算を追加する）
---@param body_params? table b2::Bodyを作成するときに指定するオプション。位置や速度、摩擦など。
---@return Rigidbody rbc
function WorldObject:add_rigidbody_component(body_params) end

---物体に付いているコンポーネントを取得する
---@generic T : Component
---@param component_type `T`
---@return T
function WorldObject:get_component(component_type) end
