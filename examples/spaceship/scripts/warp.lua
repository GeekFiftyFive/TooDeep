local warp = getEntity(entityID)

if (
    getCollisions(warp, "warp_region")[0]
    and getCollisions(warp, "warp_region")[0]["entityID"] == "player"
)
then
    setScene("Scene 2")
end
