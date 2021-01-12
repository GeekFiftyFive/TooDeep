local entity = getEntity(entityID)
local velocity = getVelocity(entity)
print(string.format("x: %f, y: %f\n", velocity["x"], velocity["y"]))
moveEntity(entity, x, y)