---@meta Camera

---@alias ProjectionMode 'Normal' | 'Orthogonal'

---カメラコンポーネント。
---@class Camera : Component
---@field projection_mode ProjectionMode 投影モード
local Camera = {}

---@type Camera
__CurrentCamera = nil

---カメラコンポーネントを追加する。
---@param projection_mode "Normal" | "Orthogonal"
---@return Camera
function WorldObject:add_camera_component(projection_mode) end
