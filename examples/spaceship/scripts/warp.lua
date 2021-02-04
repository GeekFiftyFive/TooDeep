local warp = getEntity(entityID)

if(checkCollision(warp, "warp_region"))
then
    print("Contact!")
end
