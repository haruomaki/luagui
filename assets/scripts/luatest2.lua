-- local function title()
--     print("タイトルです")
--     coroutine.yield()
-- end

-- create_window(100, 100, "たいとるお", function()
--     poyo()
--     title()
--     while true do
--         coroutine.yield()
--     end
-- end)

create_window(800, 600, "Test Window", function()
    local world = create_world()
    print(world)

    local arr = { { 0, 0 }, { 0.03, 0.01 }, { 0.01, 0.03 }, { 0, 0 } }
    world:draw_line(arr)

    for i = 0, 300 do
        if i % 60 == 0 then
            print("Step: " .. i)
        end
        coroutine.yield() -- コルーチンを一時停止
    end
end)
