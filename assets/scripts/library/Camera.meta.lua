---@meta Camera

---@alias ProjectionMode 'Perspective' | 'Orthographic'

---カメラコンポーネント。
---@class Camera : Component
---@field projection_mode ProjectionMode 投影モード
local Camera = {}

---@type Camera
__CurrentCamera = nil

---このカメラをアクティブにする。
function Camera:set_active() end

---カメラコンポーネントを追加する。
---@param projection_mode ProjectionMode
---@return Camera
function WorldObject:add_camera_component(projection_mode) end
