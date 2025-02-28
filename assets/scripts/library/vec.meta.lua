---@meta vec

---二次元ベクトル。
---@class vec2
---@field x number
---@field y number
---@operator add(vec2): vec2
---@operator sub(vec2): vec2
---@operator mul(integer): vec2
---@operator div(integer): vec2
vec2 = {}

---空のvec2を作成する。
---@return vec2
function vec2.new() end

---vec2を作成する。
---@param x number
---@param y number
---@return vec2
function vec2.new(x, y) end

---vec2を作成する。
---@param point table<integer, number>
---@return vec2
function vec2(point) end

---三次元ベクトル。
---@class vec3
---@field x number
---@field y number
---@field z number
---@operator add(vec3): vec3
---@operator sub(vec3): vec3
---@operator mul(integer): vec3
---@operator div(integer): vec3
vec3 = {}

---空のvec3を作成する。
---@return vec3
function vec3.new() end

---vec3を作成する。
---@param x number
---@param y number
---@param z number
---@return vec3
function vec3.new(x, y, z) end

---vec3を作成する。
---@param point table<integer, number>
---@return vec3
function vec3(point) end

---ベクトルの2乗ノルム（長さ）を求める。
---@return number
function vec3:length() end

---ベクトルを正規化する。
---@return vec3
function vec3:normalize() end

---内積を求める。
---@param a vec3
---@param b vec3
function vec3.dot(a, b) end

---RGBAの色。
---@class RGBA
---@field r number
---@field g number
---@field b number
---@field a number
RGBA = {}

---空のRGBAを作成する。
---@return RGBA
function RGBA.new() end

---RGBAを作成する。
---@param r number
---@param g number
---@param b number
---@param a number
---@return RGBA
function RGBA.new(r, g, b, a) end

---RGBAを作成する。
---@param point table<integer, number>
---@return RGBA
function RGBA(point) end

---intの配列。
---@class VI
VI = {}

---テーブルをintの配列に変換する。
---@param points table<integer, integer>
---@return VI
function VI(points) end

---テーブルをintの配列に変換する。
---@param points table<number, integer>
---@return VI
function VI.new(points) end

---glm::vec2の配列。
---@class V2
V2 = {}

---テーブルをglm::vec2の配列に変換する。
---@param points Points
---@return V2
function V2(points) end

---テーブルをglm::vec2の配列に変換する。
---@param points Points
---@return V2
function V2.new(points) end

---glm::vec3の配列。
---@class V3
V3 = {}

---テーブルをglm::vec3の配列に変換する。
---@param points Points
---@return V3
function V3(points) end

---テーブルをglm::vec3の配列に変換する。
---@param points Points
---@return V3
function V3.new(points) end

---RGBAの配列。
---@class CV
CV = {}

---テーブルをRGBAの配列に変換する。
---@param points Points
---@return CV
function CV(points) end

---テーブルをRGBAの配列に変換する。
---@param points Points
---@return CV
function CV.new(points) end
