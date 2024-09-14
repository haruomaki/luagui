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
