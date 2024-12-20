---@meta WorldObject

---@class WorldObject
---@field position vec3 物体の位置
---@field scale_prop number 物体の幾何平均スケール
WorldObject = {}

---@class MeshComponent : Component
MeshComponent = {}

---コンポーネント基底クラス
---@class Component
---@field id string コンポーネントのID
---@field owner WorldObject コンポーネントが所属するオブジェクト
Component = {}

---@class vec3
vec3 = {}

---空のオブジェクトを子として追加する
---@return WorldObject child
function WorldObject:append_empty_child() end

---子オブジェクトを列挙する
---@return table<number, WorldObject> children
function WorldObject:children() end

---オブジェクトを削除する
function WorldObject:erase() end

---Updateコンポーネントを追加する。（＝毎フレーム実行してほしい処理を追加する）
---@param id string コンポーネントのID
---@param f fun(self: Update) 毎フレームresumeされるコルーチン。このコルーチンが終了したらコンポーネントも削除される。
---@return Update
function WorldObject:add_update_component(id, f) end

---Rigidbodyコンポーネントを追加する。（＝物理演算を追加する）
---@param body_params? BodyOptions b2::Bodyを作成するときに指定するオプション。位置や速度、摩擦など。
---@return Rigidbody rbc
function WorldObject:add_rigidbody_component(body_params) end

---物体に付いているコンポーネントを取得する
---@generic T : Component
---@param typename `T` 取得したいコンポーネント名（型名を文字列で）
---@return T component
function WorldObject:get_component(typename) end

---IDを指定してコンポーネントを取得する
---@param id string コンポーネントのID
---@return Component comp キャスト済みのコンポーネント
function WorldObject:get_component_by_id(id) end

---コンポーネントを削除する
function Component:erase() end

-----------------------
-- Updateコンポーネント
-----------------------

---@class Update : Component
Update = {}
