local warp = getEntity(entityID)

if(checkCollision(warp, "warp_region"))
then
    setScene("Scene 2")
end
