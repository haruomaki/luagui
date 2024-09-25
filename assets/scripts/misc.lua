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

---カメラを作成する
---@return Camera camera
function supercamera_2d()
    local camera = __CurrentWorld:create_camera()
    __CurrentCamera = camera

    camera:add_update_component(function(self)
        Forever(function()
            -- TODO: Qキーでウィンドウを閉じるように
            if GetKeyDown("A") then
                print("Aキーです")
            end
        end)
    end)

    return camera
end
