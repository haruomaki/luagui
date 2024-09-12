-- require("coroutines") C++側で読み込むので不要

create_window(800, 600, "Test Window", function()
    local world = create_world()
    print(world)

    local arr = { { 0, 0 }, { 0.03, 0.01 }, { 0.01, 0.03 }, { 0, 0 } }
    world:draw_line(arr)

    Wait(5)
end)
