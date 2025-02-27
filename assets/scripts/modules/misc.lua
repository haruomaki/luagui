---@meta misc

------------
-- 汎用関数
------------

---書式に沿って文字列を表示する。
---@param format string|number
---@param ... any
function printf(format, ...)
    print(string.format(format, ...))
end

---配列のスライスを取得する
---@param tbl any
---@param first any
---@param last any
---@param step any
---@return table
function slice(tbl, first, last, step)
    local sliced = {}

    for i = first or 1, last or #tbl, step or 1 do
        sliced[#sliced + 1] = tbl[i]
    end

    return sliced
end

---リストを集合に変換する。
---@generic T 要素の型
---@param list table<integer, T> リスト
---@return table<T, boolean> set 集合
function to_set(list)
    local set = {}
    for _, value in ipairs(list) do
        set[value] = true
    end
    return set
end

--- 値を最小値と最大値の範囲内に制限する。
---@param value number 制限する値
---@param min number 最小値
---@param max number 最大値
---@return number 制限された値
function math.clamp(value, min, max)
    if value < min then
        return min
    elseif value > max then
        return max
    else
        return value
    end
end

-------------------------
-- ゲームエンジン用の関数
-------------------------

---音声を再生するだけの物体を生成する。再生し終わったら消滅する。
---@param music Music 再生する音声
function play_music(group_name, music)
    local obj = __CurrentWorld.root:append_empty_child()
    obj:add_update_component("_(play_music)音声再生用", function()
        local ss = obj:add_soundsource_component(group_name, music)
        ss:play()
        WaitWhile(function() return ss:is_playing() end)
        obj:erase()
    end)
end

---音声を再生するだけの物体を生成し、再生が終わるまで待機する。再生し終わったら消滅する。
---@param group_name string グループ名
---@param music Music 再生する音声
function play_music_blocking(group_name, music)
    local obj = __CurrentWorld.root:append_empty_child()
    local ss = obj:add_soundsource_component(group_name, music)
    ss:play()
    print("ぐるーぷ", ss.group_name)
    WaitWhile(function() return ss:is_playing() end)
    obj:erase()
end

---物体を基点にしてレイキャストを行う。
---@param direction vec3 レイの方向ベクトル（長さが射程となる）
---@return table<integer, RaycastHit>
function WorldObject:raycast(direction)
    return self.world:raycast(self.absolute_position, direction)
end

---物体の正面にレイキャストを行う。
---@param distance number レイの射程
---@return table<integer, RaycastHit>
function WorldObject:raycast_front(distance)
    return self:raycast(self:front() * distance)
end

--------------
-- 定数など
--------------

TopLeft = vec2 { -1, 1 }
TopRight = vec2 { 1, 1 }
BottomLeft = vec2 { -1, -1 }
BottomRight = vec2 { 1, -1 }
MiddleLeft = vec2 { -1, 0 }
MiddleRight = vec2 { 1, 0 }
TopMiddle = vec2 { 0, 1 }
BottomMiddle = vec2 { 0, -1 }
Center = vec2 { 0, 0 }

Front = vec3 { 0, 0, 1 }
Back = vec3 { 0, 0, -1 }
Left = vec3 { 1, 0, 0 }
Right = vec3 { -1, 0, 0 }
Up = vec3 { 0, 1, 0 }
Down = vec3 { 0, -1, 0 }

Direction = { Front = Front, Back = Back, Left = Left, Right = Right, Up = Up, Down = Down }
