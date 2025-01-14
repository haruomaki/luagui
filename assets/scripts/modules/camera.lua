---カメラを作成する
---@param ... "quit"|"move"|"zoom"
---@return Camera camera
function CreateCamera(...)
    local cobj = __CurrentWorld.root:append_empty_child()
    local camera = cobj:add_camera_component("Perspective")

    __CurrentCamera = camera

    -- オプションに応じた機能を追加
    local options = to_set { ... }
    for option, _ in pairs(options) do
        if option == "quit" then
            -- Qキーでウィンドウを閉じる
            cobj:add_update_component("quit by CreateCamera", ForeverFun(function()
                if GetKeyDown "Q" then
                    print("Qキーが押されたためウィンドウを閉じます")
                    CloseWindow()
                end
            end))
        end

        -- if option == "zoom" then
        --     -- Z/Xキーでカメラを拡大縮小する
        --     cobj:add_update_component("zoom by supercamera_2d", ForeverFun(function()
        --         if GetKey('Z') then
        --             cobj.scale_prop = cobj.scale_prop * 0.99
        --         end
        --         if GetKey('X') then
        --             cobj.scale_prop = cobj.scale_prop * 1.01
        --         end
        --     end))
        -- end

        if option == "move" then
            local speed = 0.001
            cobj:add_update_component("move by CreateCamera", ForeverFun(function()
                if GetKey('W') then
                    cobj.position = cobj.position + vec3.new(0, 0, speed)
                end
                if GetKey('A') then
                    cobj.position = cobj.position + vec3.new(-speed, 0, 0)
                end
                if GetKey('S') then
                    cobj.position = cobj.position + vec3.new(0, 0, -speed)
                end
                if GetKey('D') then
                    cobj.position = cobj.position + vec3.new(speed, 0, 0)
                end
                if GetKey('Space') then
                    cobj.position = cobj.position + vec3.new(0, speed, 0)
                end
            end))
        end
    end

    return camera
end
