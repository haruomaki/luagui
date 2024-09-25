---@meta misc

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

---カメラを作成する
---@param ... "quit"|"move"|"zoom"
---@return Camera camera
function supercamera_2d(...)
    local camera = __CurrentWorld:create_camera()
    __CurrentCamera = camera

    -- オプションに応じた機能を追加
    -- print(to_set({ 3, 3, 5, 5, 7, 8 }))
    local options = to_set { ... }
    for option, _ in pairs(options) do
        if option == "quit" then
            -- Qキーでウィンドウを閉じる
            camera:add_update_component(function(self)
                Forever(function()
                    if GetKeyDown "Q" then
                        print("Qキーが押されたためウィンドウを閉じます")
                        CloseWindow()
                    end
                end)
            end)
        end
    end

    return camera
end
