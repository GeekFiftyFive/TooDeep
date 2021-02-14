local warp = getEntity(entityID)

if (
    getCollisions(warp, "warp_region")[1]
    and getCollisions(warp, "warp_region")[1]["entityID"] == "player"
)
then
    setScene("Scene 2")
end
