---@meta Camera

---@alias ProjectionMode 'Perspective' | 'Orthographic'

---カメラコンポーネント。
---@class Camera : Component
---@field active boolean このカメラからの映像を実際に描画するかどうか
---@field projection_mode ProjectionMode 投影モード
local Camera = {}

---@type Camera
__CurrentCamera = nil

---カメラコンポーネントを追加する。
---@param projection_mode ProjectionMode
---@return Camera
function WorldObject:add_camera_component(projection_mode) end
