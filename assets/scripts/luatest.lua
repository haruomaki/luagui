-- プロパティのテスト

--- Point class definition
---@class Point
---@field x number
---@field y number
local Point = {}
Point.__index = Point

--- Constructor for Point
-- @param x Initial x coordinate
-- @param y Initial y coordinate
-- @return New Point object
function Point:new(x, y)
    local point = setmetatable({}, self)
    point.x = x or 0
    point.y = y or 0
    return point
end

--- Hoge class definition
---@class Hoge
---@field position Point
local Hoge = {}
Hoge.__index = Hoge

--- Constructor for Hoge
-- @return New Hoge object
function Hoge:new()
    local obj = setmetatable({}, self)
    obj._position = Point:new()
    return obj
end

--- __index metamethod for Hoge
-- @param key Property name
-- @return Property value
function Hoge:__index(key)
    if key == "position" then
        return rawget(self, "_position")
    else
        return rawget(self, key)
    end
end

--- __newindex metamethod for Hoge
-- @param key Property name
-- @param value New value
function Hoge:__newindex(key, value)
    if key == "position" then
        if type(value) == "table" and #value == 2 then
            self._position = Point:new(value[1], value[2])
        else
            self._position = value
        end
    else
        rawset(self, key, value)
    end
end

-- Usage example
local hoge = Hoge:new()
hoge.position = { 3, 7 }
print(hoge.position.x) -- 3
print(hoge.position.y) -- 7
