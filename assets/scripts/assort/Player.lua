---@class Player
---@field camera Camera
local Player = {}

---プレイヤーを出現させる。
---@return Player
function Player.spawn()
    local body = __CurrentWorld.root:append_empty_child()
    body.position = vec3.new(0, 0, 3)
    body.rotation = quat.angle_y(math.pi)
    local head = body:append_empty_child()
    head.position = vec3 { 0, 1.6, 0 }
    local camera = head:add_camera_component("Perspective")

    local rb = body:add_rigidbody()
    rb.mass = 1
    rb:box_shape(0.25, 0.15, 0.8)

    __CurrentCamera = camera

    body:add_update_component("move by CreateCamera", ForeverFun(function()
        local dt = 1 / Screen.refreshRate
        local speed = 2.2 * dt
        local angle_speed = 0.8 * dt

        if GetKey('W') then
            rb.linear_velocity = rb.linear_velocity + body:front() * speed
        end
        if GetKey('A') then
            rb.linear_velocity = rb.linear_velocity + body:left() * speed
        end
        if GetKey('S') then
            rb.linear_velocity = rb.linear_velocity + body:back() * speed
        end
        if GetKey('D') then
            rb.linear_velocity = rb.linear_velocity + body:right() * speed
        end

        if GetKeyDown('Space') then
            rb.linear_velocity = rb.linear_velocity + vec3 { 0, 5, 0 }
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

        if GetKeyDown('Q') then
            CloseWindow()
        end
    end))

    return { camera = camera }
end

return Player
