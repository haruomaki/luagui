---@meta WorldObject

---@class WorldObject
---@field position vec3 物体の位置
local WorldObject = {}

---@class MeshObject : WorldObject
local MeshObject = {}

---コンポーネント基底クラス
---@class Component
---@field owner WorldObject コンポーネントが所属するオブジェクト
local Component = {}

---@class vec3
local vec3 = {}

---オブジェクトを削除する
function WorldObject:erase() end

---Updateコンポーネントを追加する。（＝毎フレーム実行してほしい処理を追加する）
---@param f fun(self: WorldObject) 毎フレームresumeされるコルーチン。このコルーチンが終了したらコンポーネントも削除される。
function WorldObject:add_update_component(f) end

---Rigidbodyコンポーネントを追加する。（＝物理演算を追加する）
---@param body_params? BodyOptions b2::Bodyを作成するときに指定するオプション。位置や速度、摩擦など。
---@return Rigidbody rbc
function WorldObject:add_rigidbody_component(body_params) end

---物体に付いているコンポーネントを取得する
---@generic T : Component
---@param typename `T` 取得したいコンポーネント名（型名を文字列で）
---@return T component
function WorldObject:get_component(typename) end

---コンポーネントを削除する
function Component:erase() end
