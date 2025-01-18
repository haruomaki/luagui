---@meta Camera

---透視投影／平行投影
---@alias ProjectionMode 'Perspective' | 'Orthographic'

---カメラコンポーネント。
---@class Camera : Component
---@field centering Camera.Centering 画面のどこを中心として風景を描画するか
---@field active boolean このカメラからの映像を実際に描画するかどうか
---@field projection_mode ProjectionMode 投影モード
local Camera = {}

---カメラの投影モード（透視投影 ↔ 平行投影）を切り替える。
function Camera:toggle_mode() end

---@type Camera
__CurrentCamera = nil

---カメラコンポーネントを追加する。
---@param projection_mode ProjectionMode
---@return Camera
function WorldObject:add_camera_component(projection_mode) end

---@alias Camera.Centering 'Center' | 'TopLeft' | 'BottomLeft' | 'TopRight' | 'BottomRight'
