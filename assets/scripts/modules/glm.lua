-----------------------------
-- glm由来の数学クラス・関数
-----------------------------

function quat.angle_x(angle) return quat.angle_axis(angle, vec3.new(1, 0, 0)) end

function quat.angle_y(angle) return quat.angle_axis(angle, vec3.new(0, 1, 0)) end

function quat.angle_z(angle) return quat.angle_axis(angle, vec3.new(0, 0, 1)) end
