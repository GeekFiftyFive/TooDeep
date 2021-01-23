local entity = getEntity(entityID)
local velocity = getEntityVelocity(entity)

if(velocity["x"] > 0.0 and direction == "right" or velocity["x"] < 0.0 and direction == "left")
then
    setEntityVelocity(entity, x, velocity["y"])
end

if(math.abs(x) > 0.0)
then
    if(checkCollision(entity, "bottom"))
    then
       return
    end
    setEntityVelocity(entity, x, velocity["y"])
end
