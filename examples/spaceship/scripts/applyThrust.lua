local entity = getEntity(entityID)
if(thrust < 0.001)
then
    playAnimation(entity, "rocket_idle")
else
    playAnimation(entity, "rocket_flight")
end

applyForceToEntity(entity, 0, thrust)