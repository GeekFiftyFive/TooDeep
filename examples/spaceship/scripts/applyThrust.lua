local entity = getEntity(entityID)
if(thrust < 0.001)
then
    setStateMachineValue(entity, "flying", false)
else
    playAudio("sounds/jump.wav");
    setStateMachineValue(entity, "flying", true)
end

applyForceToEntity(entity, 0, thrust)