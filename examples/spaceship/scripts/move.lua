local velocity = getVelocity(entityID)
print(string.format("x: %f, y: %f\n", velocity["x"], velocity["y"]))
moveEntity(entityID, x, y)