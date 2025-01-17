---@meta vec

---intの配列。
---@class VI
VI = {}

---テーブルをintの配列に変換する。
---@param points table<number, integer>
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
