---@meta World

---ワールドを生成する。
---@param debug boolean? デバッグ用のカメラを一緒に生成するか
---@return World
function create_world(debug) end

---@class World
---@field b2world b2World
---@field bullet_world BulletWorld
---@field root WorldObject
World = {}

---@type World
__CurrentWorld = nil

---レイキャストを行う。
---@param origin vec3
---@param direction vec3
---@return table<integer, RaycastHit>
function World:raycast(origin, direction) end

---直線を描画する。
---@param points Points 頂点の座標
---@return MeshComponent
---@param is_loop boolean? 端点をつなげてループにするかどうか
function WorldObject:draw_line(points, is_loop) end

---矩形を描画する。
---@param hx number 横幅の半分
---@param hy number 縦幅の半分
---@return MeshComponent
function WorldObject:draw_rect(hx, hy) end

---円を描画する。
---@param center table<number, number> 中心の座標
---@param radius number 半径
---@param segments? integer 分割数（デフォルト: 48）
---@return MeshComponent
function WorldObject:draw_circle(center, radius, segments) end
