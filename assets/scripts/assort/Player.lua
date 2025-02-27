---@class Player
---@field camera Camera
---@field head WorldObject
local Player = {}

local HEIGHT = 1.75

-- 足音の効果音。
local footprint_sounds = {}
for i = 1, 6 do
    local path = string.format("audio/足音/足音%d.mp3", i)
    footprint_sounds[i] = Music.load(path)
end

local walking_score = 0

---プレイヤーが地面に接地しているかどうか。
---@type boolean
Player.is_ground = false

---プレイヤーを出現させる。
---@return Player
function Player.spawn()
    local body = __CurrentWorld.root:append_empty_child()
    body.position = vec3.new(0, HEIGHT / 2, 5)
    body.rotation = quat.angle_y(math.pi)
    local head = body:append_empty_child()
    head.position = vec3 { 0, HEIGHT / 2 - 0.2, 0 }
    local camera = head:add_camera_component("Perspective")

    local rb = body:add_rigidbody()
    rb.group = 2
    rb.mass = 1
    rb.friction = 0
    rb.restitution = 0
    rb:cylinder_shape(0.25, HEIGHT / 2, 0.25)

    __CurrentCamera = camera

    body:add_update_component("move by CreateCamera", ForeverFun(function()
        -- 自機剛体がスリープ状態になるのを防ぐ
        rb:activate()

        -- 接地しているかどうかチェック。
        local results = body:raycast(vec3 { 0, -HEIGHT / 1.8, 0 })
        Player.is_ground = results[1] ~= nil

        local dt = 1 / Screen.refreshRate
        local speed = 2.8
        local angle_speed = 0.8

        -- ダッシュ
        if GetKey("LeftShift") then speed = speed * 1.65 end

        -- 移動とジャンプ
        local y = rb.linear_velocity.y
        local d = vec3 { 0, 0, 0 }
        if GetKey('W') then d = d + body:front() end
        if GetKey('A') then d = d + body:left() end
        if GetKey('S') then d = d + body:back() end
        if GetKey('D') then d = d + body:right() end
        if GetKeyDown('Space') and Player.is_ground then y = 6.7 end
        if d:length() > 0 then d = d:normalize() end -- ゼロベクトルなら正規化しない
        rb.linear_velocity = d * speed + vec3 { 0, y, 0 }

        -- 左右首振り
        local newav = vec3 { 0, 0, 0 }
        if GetKey('Left') then newav = newav + vec3 { 0, angle_speed, 0 } end
        if GetKey('Right') then newav = newav + vec3 { 0, -angle_speed, 0 } end
        newav = newav - vec3 { 0, Mouse.diff.x * 0.2, 0 }
        rb.angular_velocity = newav

        -- 上下首振り
        if GetKey('Up') then head.rotation = head.rotation * quat.angle_x(-angle_speed * dt) end
        if GetKey('Down') then head.rotation = head.rotation * quat.angle_x(angle_speed * dt) end
        head.rotation = head.rotation * quat.angle_x(Mouse.diff.y * 0.2 * dt)

        if GetKeyDown('Q') or body.position.y < -20 then
            CloseWindow()
        end

        -- 足音を鳴らす。
        local walk_vec = rb.linear_velocity
        walk_vec.y = 0
        if Player.is_ground then
            walking_score = walking_score + math.log(walk_vec:length() * dt + 1)
            if walking_score > 1.2 then
                -- print("ポコっ")
                walking_score = 0
                play_music(footprint_sounds[math.random(1, 6)])
            end
        end
    end))

    return { camera = camera, head = head }
end

return Player
