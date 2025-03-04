---2Dカメラを作成する
---@param ... "quit"|"move"|"zoom"
---@return Camera camera
function CreateCamera2D(...)
    local cobj = __CurrentWorld.root:append_empty_child()
    cobj.rotation = quat.angle_y(math.pi)
    local camera = cobj:add_camera_component("Orthographic")

    __CurrentCamera = camera

    -- オプションに応じた機能を追加
    -- print(to_set({ 3, 3, 5, 5, 7, 8 }))
    local options = to_set { ... }
    for option, _ in pairs(options) do
        if option == "quit" then
            -- Qキーでウィンドウを閉じる
            cobj:add_update_component("quit by CreateCamera2D", ForeverFun(function()
                if GetKeyDown "Q" then
                    print("Qキーが押されたためウィンドウを閉じます")
                    CloseWindow()
                end
            end))
        end

        if option == "zoom" then
            -- Z/Xキーでカメラを拡大縮小する
            cobj:add_update_component("zoom by supercamera_2d", ForeverFun(function()
                if GetKey('Z') then
                    cobj.scale_prop = cobj.scale_prop * 0.99
                end
                if GetKey('X') then
                    cobj.scale_prop = cobj.scale_prop * 1.01
                end
            end))
        end
    end

    return camera
end

---カメラを作成する
---@param ... "quit"|"move"|"zoom"
---@return Camera camera
function CreateCamera(...)
    local body = __CurrentWorld.root:append_empty_child()
    body.position = vec3.new(0, 0, 3)
    body.rotation = quat.angle_y(math.pi)
    body.scale_prop = 10
    local head = body:append_empty_child()
    local camera = head:add_camera_component("Perspective")

    __CurrentCamera = camera

    -- オプションに応じた機能を追加
    local options = to_set { ... }
    for option, _ in pairs(options) do
        if option == "quit" then
            -- Qキーでウィンドウを閉じる
            body:add_update_component("quit by CreateCamera", ForeverFun(function()
                if GetKeyDown "Q" then
                    print("Qキーが押されたためウィンドウを閉じます")
                    CloseWindow()
                end
            end))
        end

        if option == "zoom" then
            -- Z/Xキーでカメラを拡大縮小する
            body:add_update_component("zoom by CreateCamera", ForeverFun(function()
                local dt = 1 / Screen.refreshRate
                local zoom_speed = 1 + (1.0 * dt)
                if GetKey('Z') then
                    body.scale_prop = body.scale_prop * zoom_speed
                end
                if GetKey('X') then
                    body.scale_prop = body.scale_prop / zoom_speed
                end
            end))
        end

        if option == "move" then
            body:add_update_component("move by CreateCamera", ForeverFun(function()
                local dt = 1 / Screen.refreshRate
                local speed = 0.15 * dt
                local angle_speed = 0.8 * dt
                if GetKey('W') then
                    body.position = body.position + body:front() * speed
                end
                if GetKey('A') then
                    body.position = body.position + body:left() * speed
                end
                if GetKey('S') then
                    body.position = body.position + body:back() * speed
                end
                if GetKey('D') then
                    body.position = body.position + body:right() * speed
                end
                if GetKey('Space') then
                    body.position = body.position + body:up() * speed
                end
                if GetKey('LeftShift') then
                    body.position = body.position + body:down() * speed
                end
                if GetKey('Left') then
                    body.rotation = body.rotation * quat.angle_y(angle_speed)
                end
                if GetKey('Right') then
                    body.rotation = body.rotation * quat.angle_y(-angle_speed)
                end
                if GetKey('Up') then
                    head.rotation = head.rotation * quat.angle_x(-angle_speed)
                end
                if GetKey('Down') then
                    head.rotation = head.rotation * quat.angle_x(angle_speed)
                end
            end))
        end
    end

    return camera
end
