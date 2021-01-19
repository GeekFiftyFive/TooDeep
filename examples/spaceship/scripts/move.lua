local entity = getEntity(entityID)
local velocity = getEntityVelocity(entity)

if(velocity["x"] > 0.0 and direction == "right" or velocity["x"] < 0.0 and direction == "left")
then
    setEntityVelocity(entity, x, velocity["y"])
end

if(x > 0.0 or -1 * x > 0.0)
then
    setEntityVelocity(entity, x, velocity["y"])
end
