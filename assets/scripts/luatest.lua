local function title()
    print("Title screen", window)
    local world = create_world()
    world:draw_line()
    print(type(world))
end

local function play()
    print("Game started")


    local game_time = 0
    while game_time < 60000 do -- 10秒間ゲームプレイ？
        if game_time % 60 == 0 then
            print("Game time: " .. game_time)
        end
        game_time = game_time + 1
        coroutine.yield()
    end
end

local function gameover()
    print("Game over")
    -- ゲームオーバー画面のロジック
end

create_window(400, 300, "Lua Window", function()
    title()    -- ゲームのタイトルを表示
    play()     -- ゲームをプレイ
    gameover() -- ゲームオーバー画面を表示
end)
