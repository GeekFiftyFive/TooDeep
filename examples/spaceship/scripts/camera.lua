local spaceship = getEntity(entityID)
local camera = getCamera("default")
local cameraPos = getCameraPosition(camera)
local spaceshipPos = getEntityPosition(spaceship)

if(spaceshipPos["y"] > tolerance)
then
    setCameraPosition(camera, cameraPos["x"], spaceshipPos["y"] + startY)
else
    setCameraPosition(camera, cameraPos["x"], startY)
end
