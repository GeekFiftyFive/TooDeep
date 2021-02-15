local spaceship = getEntity(entityID)
local camera = getCamera("default")
local cameraPos = getCameraPosition(camera)
local spaceshipPos = getEntityPosition(spaceship)

toggleCursor(false)

if(spaceshipPos["y"] > tolerance)
then
    setCameraPosition(camera, cameraPos["x"], spaceshipPos["y"] + startY)
else
    setCameraPosition(camera, cameraPos["x"], startY)
end
