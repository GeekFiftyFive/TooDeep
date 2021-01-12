local entity = getEntity(entityID)
local velocity = getEntityVelocity(entity)
setEntityVelocity(entity, x, velocity["y"])