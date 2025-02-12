---@meta WorldObject

---@class WorldObject
---@field id string オブジェクトのID
---@field world World この物体が属しているワールド
---@field parent WorldObject 親オブジェクト
---@field position vec3 物体の位置
---@field rotation quat 物体の回転
---@field scale_prop number 物体の幾何平均スケール
---@field absolute_position vec3 物体の絶対座標
WorldObject = {}

---コンポーネント基底クラス
---@class Component
---@field name string コンポーネントの名前
---@field owner WorldObject コンポーネントが所属するオブジェクト
Component = {}

---３次元空間内の回転を表す。
---@class quat
quat = {}

---指定の軸周りにangleラジアン回転させる四元数表現を得る。
---@param angle number
---@param axis vec3
---@return quat
function quat.angle_axis(angle, axis) end

---空のオブジェクトを子として追加する
---@return WorldObject child
function WorldObject:append_empty_child() end

---子オブジェクトを列挙する
---@return table<number, WorldObject> children
function WorldObject:children() end

---オブジェクトを削除する。
function WorldObject:erase() end

---オブジェクトをただちに削除する。
function WorldObject:force_erase() end

---C++上での生ポインタを取得する。
function WorldObject:ptr() end

---@return vec3
function WorldObject:front() end

---@return vec3
function WorldObject:back() end

---@return vec3
function WorldObject:right() end

---@return vec3
function WorldObject:left() end

---@return vec3
function WorldObject:up() end

---@return vec3
function WorldObject:down() end

---Updateコンポーネントを追加する。（＝毎フレーム実行してほしい処理を追加する）
---@param id string コンポーネントのID
---@param f fun(self: Update) 毎フレームresumeされるコルーチン。このコルーチンが終了したらコンポーネントも削除される。
---@return Update
function WorldObject:add_update_component(id, f) end

---Rigidbody2Dコンポーネントを追加する。（＝物理演算を追加する）
---@param body_params? Body2DOptions b2::Bodyを作成するときに指定するオプション。位置や速度、摩擦など。
---@return Rigidbody2D rbc
function WorldObject:add_rigidbody2d_component(body_params) end

---物体に付いているコンポーネントを取得する
---@generic T : Component
---@param typename `T` 取得したいコンポーネント名（型名を文字列で）
---@return T component
function WorldObject:get_component(typename) end

---IDを指定してコンポーネントを取得する
---@param name string コンポーネントの名前
---@return Component comp キャスト済みのコンポーネント
function WorldObject:get_component_by_name(name) end

---コンポーネントを削除する
function Component:erase() end

-----------------------
-- Updateコンポーネント
-----------------------

---@class Update : Component
Update = {}
