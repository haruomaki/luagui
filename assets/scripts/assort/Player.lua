---@class Player
---@field camera Camera
---@field head WorldObject
local Player = {}

---プレイヤーを出現させる。
---@return Player
function Player.spawn()
    local body = __CurrentWorld.root:append_empty_child()
    body.position = vec3.new(0, 0.75, 3)
    body.rotation = quat.angle_y(math.pi)
    local head = body:append_empty_child()
    head.position = vec3 { 0, 0.75, 0 }
    local camera = head:add_camera_component("Perspective")

    local rb = body:add_rigidbody()
    rb.mass = 1
    rb.friction = 0
    rb.restitution = 0
    rb:cylinder_shape(0.25, 0.75, 0.25)

    __CurrentCamera = camera

    body:add_update_component("move by CreateCamera", ForeverFun(function()
        local dt = 1 / Screen.refreshRate
        local speed = 2.2
        local angle_speed = 0.8

        -- local x = 0
        local y = rb.linear_velocity.y
        -- local z = 0
        local newv = vec3 { 0, 0, 0 }
        if GetKey('W') then newv = newv + body:front() * speed end
        if GetKey('A') then newv = newv + body:left() * speed end
        if GetKey('S') then newv = newv + body:back() * speed end
        if GetKey('D') then newv = newv + body:right() * speed end
        if GetKeyDown('Space') then y = y + 6 end
        rb.linear_velocity = vec3 { newv.x, y, newv.z }

        local newav = vec3 { 0, 0, 0 }
        if GetKey('Left') then newav = newav + vec3 { 0, angle_speed, 0 } end
        if GetKey('Right') then newav = newav + vec3 { 0, -angle_speed, 0 } end
        rb.angular_velocity = newav

        if GetKey('Up') then head.rotation = head.rotation * quat.angle_x(-angle_speed * dt) end
        if GetKey('Down') then head.rotation = head.rotation * quat.angle_x(angle_speed * dt) end

        if GetKeyDown('Q') or body.position.y < -100 then
            CloseWindow()
        end
    end))

    return { camera = camera, head = head }
end

return Player
